# Copyright (c) 2009, Google Inc. All rights reserved.
# Copyright (c) 2009 Apple Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import StringIO
import errno
import logging
import multiprocessing
import os
import signal
import subprocess
import sys
import time

from webkitpy.common.system.outputtee import Tee
from webkitpy.common.system.filesystem import FileSystem


_log = logging.getLogger(__name__)


class ScriptError(Exception):

    def __init__(self,
                 message=None,
                 script_args=None,
                 exit_code=None,
                 output=None,
                 cwd=None):
        if not message:
            message = 'Failed to run "%s"' % repr(script_args)
            if exit_code:
                message += " exit_code: %d" % exit_code
            if cwd:
                message += " cwd: %s" % cwd

        Exception.__init__(self, message)
        self.script_args = script_args  # 'args' is already used by Exception
        self.exit_code = exit_code
        self.output = output
        self.cwd = cwd

    def message_with_output(self, output_limit=500):
        if self.output:
            if output_limit and len(self.output) > output_limit:
                return u"%s\n\nLast %s characters of output:\n%s" % \
                    (self, output_limit, self.output[-output_limit:])
            return u"%s\n\n%s" % (self, self.output)
        return unicode(self)

    def command_name(self):
        command_path = self.script_args
        if type(command_path) is list:
            command_path = command_path[0]
        return os.path.basename(command_path)


class Executive(object):
    PIPE = subprocess.PIPE
    STDOUT = subprocess.STDOUT

    def __init__(self):
        self.pid_to_system_pid = {}

    def _should_close_fds(self):
        # We need to pass close_fds=True to work around Python bug #2320
        # (otherwise we can hang when we kill DumpRenderTree when we are running
        # multiple threads). See http://bugs.python.org/issue2320 .
        # In Python 2.7.10, close_fds is also supported on Windows.
        # However, "you cannot set close_fds to true and also redirect the standard
        # handles by setting stdin, stdout or stderr.".
        if sys.platform.startswith('win'):
            return False
        else:
            return True

    def _run_command_with_teed_output(self, args, teed_output, **kwargs):
        child_process = self.popen(args,
                                   stdout=self.PIPE,
                                   stderr=self.STDOUT,
                                   close_fds=self._should_close_fds(),
                                   **kwargs)

        # Use our own custom wait loop because Popen ignores a tee'd
        # stderr/stdout.
        # FIXME: This could be improved not to flatten output to stdout.
        while True:
            output_line = child_process.stdout.readline()
            if output_line == "" and child_process.poll() != None:
                # poll() is not threadsafe and can throw OSError due to:
                # http://bugs.python.org/issue1731717
                return child_process.poll()
            # We assume that the child process wrote to us in utf-8,
            # so no re-encoding is necessary before writing here.
            teed_output.write(output_line)

    # FIXME: Remove this deprecated method and move callers to run_command.
    # FIXME: This method is a hack to allow running command which both
    # capture their output and print out to stdin.  Useful for things
    # like "build-webkit" where we want to display to the user that we're building
    # but still have the output to stuff into a log file.
    def run_and_throw_if_fail(self, args, quiet=False, decode_output=True, **kwargs):
        # Cache the child's output locally so it can be used for error reports.
        child_out_file = StringIO.StringIO()
        tee_stdout = sys.stdout
        if quiet:
            dev_null = open(os.devnull, "w")  # FIXME: Does this need an encoding?
            tee_stdout = dev_null
        child_stdout = Tee(child_out_file, tee_stdout)
        exit_code = self._run_command_with_teed_output(args, child_stdout, **kwargs)
        if quiet:
            dev_null.close()

        child_output = child_out_file.getvalue()
        child_out_file.close()

        if decode_output:
            child_output = child_output.decode(self._child_process_encoding())

        if exit_code:
            raise ScriptError(script_args=args,
                              exit_code=exit_code,
                              output=child_output)
        return child_output

    def cpu_count(self):
        try:
            cpus = int(os.environ.get('NUMBER_OF_PROCESSORS'))
            if cpus > 0:
                return cpus
        except (ValueError, TypeError):
            pass
        return multiprocessing.cpu_count()

    @staticmethod
    def interpreter_for_script(script_path, fs=None):
        fs = fs or FileSystem()
        lines = fs.read_text_file(script_path).splitlines()
        if not len(lines):
            return None
        first_line = lines[0]
        if not first_line.startswith('#!'):
            return None
        if first_line.find('python') > -1:
            return sys.executable
        if first_line.find('perl') > -1:
            return 'perl'
        if first_line.find('ruby') > -1:
            return 'ruby'
        return None

    @staticmethod
    def shell_command_for_script(script_path, fs=None):
        fs = fs or FileSystem()
        # Win32 does not support shebang. We need to detect the interpreter ourself.
        if sys.platform.startswith('win'):
            interpreter = Executive.interpreter_for_script(script_path, fs)
            if interpreter:
                return [interpreter, script_path]
        return [script_path]

    def kill_process(self, pid):
        """Attempts to kill the given pid.
        Will fail silently if pid does not exist or insufficient permisssions."""
        if sys.platform.startswith('win32'):
            # We only use taskkill.exe on windows (not cygwin) because subprocess.pid
            # is a CYGWIN pid and taskkill.exe expects a windows pid.
            # Thankfully os.kill on CYGWIN handles either pid type.
            task_kill_executable = os.path.join('C:', os.sep, 'WINDOWS', 'system32', 'taskkill.exe')
            command = [task_kill_executable, "/f", "/t", "/pid", pid]
            # taskkill will exit 128 if the process is not found.  We should log.
            self.run_command(command, error_handler=self.ignore_error)
            return

        # According to http://docs.python.org/library/os.html
        # os.kill isn't available on Windows. python 2.5.5 os.kill appears
        # to work in cygwin, however it occasionally raises EAGAIN.
        retries_left = 10 if sys.platform == "cygwin" else 1
        while retries_left > 0:
            try:
                retries_left -= 1
                # Give processes one change to clean up quickly before exiting.
                # Following up with a kill should have no effect if the process
                # already exited, and forcefully kill it if SIGTERM wasn't enough.
                os.kill(pid, signal.SIGTERM)
                os.kill(pid, signal.SIGKILL)
            except OSError, e:
                if e.errno == errno.EAGAIN:
                    if retries_left <= 0:
                        _log.warn("Failed to kill pid %s.  Too many EAGAIN errors." % pid)
                    continue
                if e.errno == errno.ESRCH:  # The process does not exist.
                    return
                if e.errno == errno.EPIPE:  # The process has exited already on cygwin
                    return
                if e.errno == errno.ECHILD:
                    # Can't wait on a non-child process, but the kill worked.
                    return
                if e.errno == errno.EACCES and sys.platform == 'cygwin':
                    # Cygwin python sometimes can't kill native processes.
                    return
                raise

    def _win32_check_running_pid(self, pid):
        # importing ctypes at the top-level seems to cause weird crashes at
        # exit under cygwin on apple's win port. Only win32 needs cygwin, so
        # we import it here instead. See https://bugs.webkit.org/show_bug.cgi?id=91682
        import ctypes

        class PROCESSENTRY32(ctypes.Structure):
            _fields_ = [("dwSize", ctypes.c_ulong),
                        ("cntUsage", ctypes.c_ulong),
                        ("th32ProcessID", ctypes.c_ulong),
                        ("th32DefaultHeapID", ctypes.POINTER(ctypes.c_ulong)),
                        ("th32ModuleID", ctypes.c_ulong),
                        ("cntThreads", ctypes.c_ulong),
                        ("th32ParentProcessID", ctypes.c_ulong),
                        ("pcPriClassBase", ctypes.c_ulong),
                        ("dwFlags", ctypes.c_ulong),
                        ("szExeFile", ctypes.c_char * 260)]

        CreateToolhelp32Snapshot = ctypes.windll.kernel32.CreateToolhelp32Snapshot
        Process32First = ctypes.windll.kernel32.Process32First
        Process32Next = ctypes.windll.kernel32.Process32Next
        CloseHandle = ctypes.windll.kernel32.CloseHandle
        TH32CS_SNAPPROCESS = 0x00000002  # win32 magic number
        hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)
        pe32 = PROCESSENTRY32()
        pe32.dwSize = ctypes.sizeof(PROCESSENTRY32)
        result = False
        if not Process32First(hProcessSnap, ctypes.byref(pe32)):
            _log.debug("Failed getting first process.")
            CloseHandle(hProcessSnap)
            return result
        while True:
            if pe32.th32ProcessID == pid:
                result = True
                break
            if not Process32Next(hProcessSnap, ctypes.byref(pe32)):
                break
        CloseHandle(hProcessSnap)
        return result

    def check_running_pid(self, pid):
        """Return True if pid is alive, otherwise return False."""
        if sys.platform.startswith('win'):
            return self._win32_check_running_pid(pid)

        try:
            os.kill(pid, 0)
            return True
        except OSError:
            return False

    def running_pids(self, process_name_filter=None):
        if sys.platform.startswith('win'):
            # FIXME: running_pids isn't implemented on native Windows yet...
            return []

        if not process_name_filter:
            process_name_filter = lambda process_name: True

        running_pids = []
        if sys.platform in ("cygwin"):
            ps_process = self.run_command(['ps', '-e'], error_handler=Executive.ignore_error)
            for line in ps_process.splitlines():
                tokens = line.strip().split()
                try:
                    pid, ppid, pgid, winpid, tty, uid, stime, process_name = tokens
                    if process_name_filter(process_name):
                        running_pids.append(int(pid))
                        self.pid_to_system_pid[int(pid)] = int(winpid)
                except ValueError, e:
                    pass
        else:
            ps_process = self.popen(['ps', '-eo', 'pid,comm'], stdout=self.PIPE, stderr=self.PIPE)
            stdout, _ = ps_process.communicate()
            for line in stdout.splitlines():
                try:
                    # In some cases the line can contain one or more
                    # leading white-spaces, so strip it before split.
                    pid, process_name = line.strip().split(' ', 1)
                    if process_name_filter(process_name):
                        running_pids.append(int(pid))
                except ValueError, e:
                    pass

        return sorted(running_pids)

    def wait_newest(self, process_name_filter=None):
        if not process_name_filter:
            process_name_filter = lambda process_name: True

        running_pids = self.running_pids(process_name_filter)
        if not running_pids:
            return
        pid = running_pids[-1]

        while self.check_running_pid(pid):
            time.sleep(0.25)

    def wait_limited(self, pid, limit_in_seconds=None, check_frequency_in_seconds=None):
        seconds_left = limit_in_seconds or 10
        sleep_length = check_frequency_in_seconds or 1
        while seconds_left > 0 and self.check_running_pid(pid):
            seconds_left -= sleep_length
            time.sleep(sleep_length)

    def _windows_image_name(self, process_name):
        name, extension = os.path.splitext(process_name)
        if not extension:
            # taskkill expects processes to end in .exe
            # If necessary we could add a flag to disable appending .exe.
            process_name = "%s.exe" % name
        return process_name

    def interrupt(self, pid):
        interrupt_signal = signal.SIGINT
        # FIXME: The python docs seem to imply that platform == 'win32' may need to use signal.CTRL_C_EVENT
        # http://docs.python.org/2/library/signal.html
        try:
            os.kill(pid, interrupt_signal)
        except OSError:
            # Silently ignore when the pid doesn't exist.
            # It's impossible for callers to avoid race conditions with process shutdown.
            pass

    def kill_all(self, process_name):
        """Attempts to kill processes matching process_name.
        Will fail silently if no process are found."""
        if sys.platform == 'cygwin' or sys.platform.startswith('win'):
            image_name = self._windows_image_name(process_name)
            killCommmand = 'taskkill.exe'
            if sys.platform.startswith('win'):
                killCommand = os.path.join('C:', os.sep, 'WINDOWS', 'system32', 'taskkill.exe')
            command = [killCommmand, "/f", "/im", image_name]
            # taskkill will exit 128 if the process is not found.  We should log.
            self.run_command(command, error_handler=self.ignore_error)
            return

        # FIXME: This is inconsistent that kill_all uses TERM and kill_process
        # uses KILL.  Windows is always using /f (which seems like -KILL).
        # We should pick one mode, or add support for switching between them.
        # Note: Mac OS X 10.6 requires -SIGNALNAME before -u USER
        command = ["killall", "-TERM", "-u", os.getenv("USER"), process_name]
        # killall returns 1 if no process can be found and 2 on command error.
        # FIXME: We should pass a custom error_handler to allow only exit_code 1.
        # We should log in exit_code == 1
        self.run_command(command, error_handler=self.ignore_error)

    # Error handlers do not need to be static methods once all callers are
    # updated to use an Executive object.

    @staticmethod
    def default_error_handler(error):
        raise error

    @staticmethod
    def ignore_error(error):
        pass

    def _compute_stdin(self, input):
        """Returns (stdin, string_to_communicate)"""
        # FIXME: We should be returning /dev/null for stdin
        # or closing stdin after process creation to prevent
        # child processes from getting input from the user.
        if not input:
            return (None, None)
        if hasattr(input, "read"):  # Check if the input is a file.
            return (input, None)  # Assume the file is in the right encoding.

        # Popen in Python 2.5 and before does not automatically encode unicode objects.
        # http://bugs.python.org/issue5290
        # See https://bugs.webkit.org/show_bug.cgi?id=37528
        # for an example of a regresion caused by passing a unicode string directly.
        # FIXME: We may need to encode differently on different platforms.
        if isinstance(input, unicode):
            input = input.encode(self._child_process_encoding())
        return (self.PIPE, input)

    def command_for_printing(self, args):
        """Returns a print-ready string representing command args.
        The string should be copy/paste ready for execution in a shell."""
        args = self._stringify_args(args)
        escaped_args = []
        for arg in args:
            if isinstance(arg, unicode):
                # Escape any non-ascii characters for easy copy/paste
                arg = arg.encode("unicode_escape")
            # FIXME: Do we need to fix quotes here?
            escaped_args.append(arg)
        return " ".join(escaped_args)

    # FIXME: run_and_throw_if_fail should be merged into this method.
    def run_command(self,
                    args,
                    cwd=None,
                    env=None,
                    input=None,
                    error_handler=None,
                    return_exit_code=False,
                    return_stderr=True,
                    decode_output=True):
        """Popen wrapper for convenience and to work around python bugs."""
        assert(isinstance(args, list) or isinstance(args, tuple))
        start_time = time.time()

        stdin, string_to_communicate = self._compute_stdin(input)
        stderr = self.STDOUT if return_stderr else None

        process = self.popen(args,
                             stdin=stdin,
                             stdout=self.PIPE,
                             stderr=stderr,
                             cwd=cwd,
                             env=env,
                             close_fds=self._should_close_fds())
        output = process.communicate(string_to_communicate)[0]

        # run_command automatically decodes to unicode() unless explicitly told not to.
        if decode_output:
            output = output.decode(self._child_process_encoding())

        # wait() is not threadsafe and can throw OSError due to:
        # http://bugs.python.org/issue1731717
        exit_code = process.wait()

        _log.debug('"%s" took %.2fs' % (self.command_for_printing(args), time.time() - start_time))

        if return_exit_code:
            return exit_code

        if exit_code:
            script_error = ScriptError(script_args=args,
                                       exit_code=exit_code,
                                       output=output,
                                       cwd=cwd)
            (error_handler or self.default_error_handler)(script_error)
        return output

    def _child_process_encoding(self):
        # Win32 Python 2.x uses CreateProcessA rather than CreateProcessW
        # to launch subprocesses, so we have to encode arguments using the
        # current code page.
        if sys.platform.startswith('win') and sys.version < '3':
            return 'mbcs'
        # All other platforms use UTF-8.
        # FIXME: Using UTF-8 on Cygwin will confuse Windows-native commands
        # which will expect arguments to be encoded using the current code
        # page.
        return 'utf-8'

    def _should_encode_child_process_arguments(self):
        # Cygwin's Python's os.execv doesn't support unicode command
        # arguments, and neither does Cygwin's execv itself.
        if sys.platform == 'cygwin':
            return True

        # Win32 Python 2.x uses CreateProcessA rather than CreateProcessW
        # to launch subprocesses, so we have to encode arguments using the
        # current code page.
        if sys.platform.startswith('win') and sys.version < '3':
            return True

        return False

    def _encode_argument_if_needed(self, argument):
        if not self._should_encode_child_process_arguments():
            return argument
        return argument.encode(self._child_process_encoding())

    def _stringify_args(self, args):
        # Popen will throw an exception if args are non-strings (like int())
        string_args = map(unicode, args)
        # The Windows implementation of Popen cannot handle unicode strings. :(
        return map(self._encode_argument_if_needed, string_args)

    def _needs_interpreter_check(self, argument):
        return not argument.endswith(('perl', 'python', 'ruby', 'taskkill.exe', 'git', 'svn'))

    # The only required argument to popen is named "args", the rest are optional keyword arguments.
    def popen(self, args, **kwargs):
        if sys.platform.startswith('win'):
            _log.debug("Looking at {0}".format(args))
            # Must include proper interpreter
            if self._needs_interpreter_check(args[0]):
                try:
                    with open(args[0], 'r') as f:
                        line = f.readline()
                        if "perl" in line:
                            args.insert(0, "perl")
                        elif "python" in line:
                            args.insert(0, "python")
                        elif "ruby" in line:
                            args.insert(0, "ruby")
                except IOError:
                    pass

        # FIXME: We should always be stringifying the args, but callers who pass shell=True
        # expect that the exact bytes passed will get passed to the shell (even if they're wrongly encoded).
        # shell=True is wrong for many other reasons, and we should remove this
        # hack as soon as we can fix all callers to not use shell=True.
        if kwargs.get('shell') == True:
            string_args = args
        else:
            string_args = self._stringify_args(args)
        return subprocess.Popen(string_args, **kwargs)

    def run_in_parallel(self, command_lines_and_cwds, processes=None):
        """Runs a list of (cmd_line list, cwd string) tuples in parallel and returns a list of (retcode, stdout, stderr) tuples."""
        assert len(command_lines_and_cwds)

        if sys.platform == 'cygwin' or sys.platform.startswith('win'):
            return map(_run_command_thunk, command_lines_and_cwds)
        pool = multiprocessing.Pool(processes=processes)
        results = pool.map(_run_command_thunk, command_lines_and_cwds)
        pool.close()
        pool.join()
        return results


def _run_command_thunk(cmd_line_and_cwd):
    # Note that this needs to be a bare module (and hence Picklable) method to work with multiprocessing.Pool.
    (cmd_line, cwd) = cmd_line_and_cwd
    proc = subprocess.Popen(cmd_line, cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = proc.communicate()
    return (proc.returncode, stdout, stderr)
