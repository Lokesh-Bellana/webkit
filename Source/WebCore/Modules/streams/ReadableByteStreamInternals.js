/*
 * Copyright (C) 2016 Canon Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// @conditional=ENABLE(STREAMS_API)
// @internal

function privateInitializeReadableByteStreamController(stream, underlyingByteSource, highWaterMark)
{
    "use strict";

    if (!@isReadableStream(stream))
        @throwTypeError("ReadableByteStreamController needs a ReadableStream");

    // readableStreamController is initialized with null value.
    if (stream.@readableStreamController !== null)
        @throwTypeError("ReadableStream already has a controller");

    this.@controlledReadableStream = stream;
    this.@underlyingByteSource = underlyingByteSource;
    this.@pullAgain = false;
    this.@pulling = false;
    @readableByteStreamControllerClearPendingPullIntos(this);
    this.@queue = [];
    this.@totalQueuedBytes = 0;
    this.@started = false;
    this.@closeRequested = false;

    let hwm = @Number(highWaterMark);
    if (@isNaN(hwm) || hwm < 0)
        @throwRangeError("highWaterMark value is negative or not a number");
    this.@strategyHWM = hwm;

    let autoAllocateChunkSize = underlyingByteSource.autoAllocateChunkSize;
    if (autoAllocateChunkSize !== @undefined) {
        autoAllocateChunkSize = @Number(autoAllocateChunkSize);
        if (autoAllocateChunkSize <= 0 || autoAllocateChunkSize === @Number.POSITIVE_INFINITY || autoAllocateChunkSize === @Number.NEGATIVE_INFINITY)
            @throwRangeError("autoAllocateChunkSize value is negative or equal to positive or negative infinity");
    }
    this.@autoAllocateChunkSize = autoAllocateChunkSize;
    this.@pendingPullIntos = [];

    const controller = this;
    const startResult = @promiseInvokeOrNoopNoCatch(underlyingByteSource, "start", [this]).@then(() => {
        controller.@started = true;
        @assert(!controller.@pulling);
        @assert(!controller.@pullAgain);
        @readableByteStreamControllerCallPullIfNeeded(controller);
    }, (error) => {
        if (stream.@state === @streamReadable)
            @readableByteStreamControllerError(controller, error);
    });

    this.@cancel = @readableByteStreamControllerCancel;
    this.@pull = @readableByteStreamControllerPull;

    return this;
}

function privateInitializeReadableStreamBYOBRequest(controller, view)
{
    "use strict";

    this.@associatedReadableByteStreamController = controller;
    this.@view = view;
}

function isReadableByteStreamController(controller)
{
    "use strict";

    // Same test mechanism as in isReadableStreamDefaultController (ReadableStreamInternals.js).
    // See corresponding function for explanations.
    return @isObject(controller) && !!controller.@underlyingByteSource;
}

function isReadableStreamBYOBRequest(byobRequest)
{
    "use strict";

    // Same test mechanism as in isReadableStreamDefaultController (ReadableStreamInternals.js).
    // See corresponding function for explanations.
    return @isObject(byobRequest) && !!byobRequest.@associatedReadableByteStreamController;
}

function isReadableStreamBYOBReader(reader)
{
    "use strict";

    // FIXME: Since BYOBReader is not yet implemented, always return false.
    // To be implemented at the same time as BYOBReader (see isReadableStreamDefaultReader
    // to apply same model).
    return false;
}

function readableByteStreamControllerCancel(controller, reason)
{
    "use strict";

    if (controller.@pendingPullIntos.length > 0)
        controller.@pendingPullIntos[0].bytesFilled = 0;
    controller.@queue = [];
    controller.@totalQueuedBytes = 0;
    return @promiseInvokeOrNoop(controller.@underlyingByteSource, "cancel", [reason]);
}

function readableByteStreamControllerError(controller, e)
{
    "use strict";

    @assert(controller.@controlledReadableStream.@state === @streamReadable);
    @readableByteStreamControllerClearPendingPullIntos(controller);
    controller.@queue = [];
    @readableStreamError(controller.@controlledReadableStream, e);
}

function readableByteStreamControllerClose(controller)
{
    "use strict";

    @assert(!controller.@closeRequested);
    @assert(controller.@controlledReadableStream.@state === @streamReadable);

    if (controller.@totalQueuedBytes > 0) {
        controller.@closeRequested = true;
        return;
    }

    if (controller.@pendingPullIntos.length > 0) {
        if (controller.@pendingPullIntos[0].bytesFilled > 0) {
            const e = new @TypeError("Close requested while there remain pending bytes");
            @readableByteStreamControllerError(controller, e);
            throw e;
        }
    }

    @readableStreamClose(controller.@controlledReadableStream);
}

function readableByteStreamControllerClearPendingPullIntos(controller)
{
    "use strict";

    // FIXME: To be implemented in conjunction with ReadableStreamBYOBRequest.
}

function readableByteStreamControllerGetDesiredSize(controller)
{
   "use strict";

   return controller.@strategyHWM - controller.@totalQueuedBytes;
}

function readableStreamHasBYOBReader(stream)
{
    "use strict";

    return stream.@reader !== @undefined && @isReadableStreamBYOBReader(stream.@reader);
}

function readableStreamHasDefaultReader(stream)
{
    "use strict";

    return stream.@reader !== @undefined && @isReadableStreamDefaultReader(stream.@reader);
}

function readableByteStreamControllerHandleQueueDrain(controller) {

    "use strict";

    @assert(controller.@controlledReadableStream.@state === @streamReadable);
    if (!controller.@totalQueuedBytes && controller.@closeRequested)
        @readableStreamClose(controller.@controlledReadableStream);
    else
        @readableByteStreamControllerCallPullIfNeeded(controller);
}

function readableByteStreamControllerPull(controller)
{
    "use strict";

    const stream = controller.@controlledReadableStream;
    @assert(@readableStreamHasDefaultReader(stream));

    if (controller.@totalQueuedBytes > 0) {
        @assert(stream.@reader.@readRequests.length === 0);
        const entry = controller.@queue.@shift();
        controller.@totalQueuedBytes -= entry.byteLength;
        @readableByteStreamControllerHandleQueueDrain(controller);
        let view;
        try {
            view = new @Uint8Array(entry.buffer, entry.byteOffset, entry.byteLength);
        } catch (error) {
            return @Promise.@reject(error);
        }
        return @Promise.@resolve({value: view, done: false});
    }

    if (controller.@autoAllocateChunkSize !== @undefined) {
        let buffer;
        try {
            buffer = new @ArrayBuffer(controller.@autoAllocateChunkSize);
        } catch (error) {
            return @Promise.@reject(error);
        }
        const pullIntoDescriptor = {
            buffer,
            byteOffset: 0,
            byteLength: controller.@autoAllocateChunkSize,
            bytesFilled: 0,
            elementSize: 1,
            ctor: @Uint8Array,
            readerType: 'default'
        };
        controller.@pendingPullIntos.@push(pullIntoDescriptor);
    }

    const promise = @readableStreamAddReadRequest(stream);
    @readableByteStreamControllerCallPullIfNeeded(controller);
    return promise;
}

function readableByteStreamControllerShouldCallPull(controller)
{
    "use strict";

    const stream = controller.@controlledReadableStream;

    if (stream.@state !== @streamReadable)
        return false;
    if (controller.@closeRequested)
        return false;
    if (!controller.@started)
        return false;
    if (@readableStreamHasDefaultReader(stream) && stream.@reader.@readRequests.length > 0)
        return true;
    if (@readableStreamHasBYOBReader(stream) && stream.@reader.@readIntoRequests.length > 0)
        return true;
    if (@readableByteStreamControllerGetDesiredSize(controller) > 0)
        return true;
    return false;
}

function readableByteStreamControllerCallPullIfNeeded(controller)
{
    "use strict";

    if (!@readableByteStreamControllerShouldCallPull(controller))
        return;

    if (controller.@pulling) {
        controller.@pullAgain = true;
        return;
    }

    @assert(!controller.@pullAgain);
    controller.@pulling = true;
    @promiseInvokeOrNoop(controller.@underlyingByteSource, "pull", [controller]).@then(() => {
        controller.@pulling = false;
        if (controller.@pullAgain) {
            controller.@pullAgain = false;
            @readableByteStreamControllerCallPullIfNeeded(controller);
        }
    }, (error) => {
        if (controller.@controlledReadableStream.@state === @streamReadable)
            @readableByteStreamControllerError(controller, error);
    });
}

function transferBufferToCurrentRealm(buffer)
{
    "use strict";

    // FIXME: Determine what should be done here exactly (what is already existing in current
    // codebase and what has to be added). According to spec, Transfer operation should be
    // performed in order to transfer buffer to current realm. For the moment, simply return
    // received buffer.
    return buffer;
}

function readableByteStreamControllerEnqueue(controller, chunk)
{
    "use strict";

    const stream = controller.@controlledReadableStream;
    @assert(!controller.@closeRequested);
    @assert(stream.@state === @streamReadable);
    const buffer = chunk.buffer;
    const byteOffset = chunk.byteOffset;
    const byteLength = chunk.byteLength;
    const transferredBuffer = @transferBufferToCurrentRealm(buffer);

    if (@readableStreamHasDefaultReader(stream)) {
        if (!stream.@reader.@readRequests.length)
            @readableByteStreamControllerEnqueueChunk(controller, transferredBuffer, byteOffset, byteLength);
        else {
            @assert(!controller.@queue.length);
            let transferredView = new @Uint8Array(transferredBuffer, byteOffset, byteLength);
            @readableStreamFulfillReadRequest(stream, transferredView, false);
        }
        return;
    }

    if (@readableStreamHasBYOBReader(stream)) {
        // FIXME: To be implemented once ReadableStreamBYOBReader has been implemented (for the moment,
        // test cannot be true).
        @throwTypeError("ReadableByteStreamController enqueue operation has no support for BYOB reader");
        return;
    }

    @assert(!@isReadableStreamLocked(stream));
    @readableByteStreamControllerEnqueueChunk(controller, transferredBuffer, byteOffset, byteLength);
}

// Spec name: readableByteStreamControllerEnqueueChunkToQueue.
function readableByteStreamControllerEnqueueChunk(controller, buffer, byteOffset, byteLength)
{
    "use strict";

    controller.@queue.@push({
        buffer: buffer,
        byteOffset: byteOffset,
        byteLength: byteLength
    });
    controller.@totalQueuedBytes += byteLength;
}

function readableByteStreamControllerRespondWithNewView(controller, view)
{
    "use strict";

    @assert(controller.@pendingPullIntos.length > 0);

    let firstDescriptor = controller.@pendingPullIntos[0];

    if (firstDescriptor.byteOffset + firstDescriptor.bytesFilled !== view.byteOffset)
        @throwRangeError("Invalid value for view.byteOffset");

    if (firstDescriptor.byteLength !== view.byteLength)
        @throwRangeError("Invalid value for view.byteLength");

    firstDescriptor.buffer = view.buffer;
    @readableByteStreamControllerRespondInternal(controller, view.byteLength);
}

function readableByteStreamControllerRespond(controller, bytesWritten)
{
    "use strict";

    bytesWritten = @Number(bytesWritten);

    if (@isNaN(bytesWritten) || bytesWritten === @Number.POSITIVE_INFINITY || bytesWritten < 0 )
        @throwRangeError("bytesWritten has an incorrect value");

    @assert(controller.@pendingPullIntos.length > 0);

    @readableByteStreamControllerRespondInternal(controller, bytesWritten);
}

function readableByteStreamControllerRespondInternal(controller, bytesWritten)
{
    "use strict";

    let firstDescriptor = controller.@pendingPullIntos[0];
    let stream = controller.@controlledReadableStream;

    if (stream.@state === @streamClosed) {
        if (bytesWritten !== 0)
            @throwTypeError("bytesWritten is different from 0 even though stream is closed");
        @readableByteStreamControllerRespondInClosedState(controller, firstDescriptor);
    } else {
        @assert(stream.@state === @streamReadable);
        @readableByteStreamControllerRespondInReadableState(controller, bytesWritten, firstDescriptor);
    }
}

function readableByteStreamControllerRespondInReadableState(controller, bytesWritten, pullIntoDescriptor)
{
    "use strict";

    if (pullIntoDescriptor.bytesFilled + bytesWritten > pullIntoDescriptor.byteLength)
        @throwRangeError("bytesWritten value is too great");

    @assert(controller.@pendingPullIntos.length === 0 || controller.@pendingPullIntos[0] === pullIntoDescriptor);
    @readableByteStreamControllerInvalidateBYOBRequest(controller);
    pullIntoDescriptor.bytesFilled += bytesWritten;

    if (pullIntoDescriptor.bytesFilled < pullIntoDescriptor.elementSize)
        return;

    @readableByteStreamControllerShiftPendingDescriptor(controller);
    const remainderSize = pullIntoDescriptor.bytesFilled % pullIntoDescriptor.elementSize;

    if (remainderSize > 0) {
        const end = pullIntoDescriptor.byteOffset + pullIntoDescriptor.bytesFilled;
        const remainder = @cloneArrayBuffer(pullIntoDescriptor.buffer, end - remainderSize, remainderSize);
        @readableByteStreamControllerEnqueueChunk(controller, remainder, 0, remainder.byteLength);
    }

    pullIntoDescriptor.buffer = @transferBufferToCurrentRealm(pullIntoDescriptor.buffer);
    pullIntoDescriptor.bytesFilled -= remainderSize;
    @readableByteStreamControllerCommitDescriptor(controller.@controlledReadableStream, pullIntoDescriptor);
    @readableByteStreamControllerProcessPullDescriptors(controller);
}

function readableByteStreamControllerRespondInClosedState(controller, firstDescriptor)
{
    "use strict";

    firstDescriptor.buffer = @transferBufferToCurrentRealm(firstDescriptor.buffer);
    @assert(firstDescriptor.bytesFilled === 0);

    // FIXME: Spec does not describe below test. However, only ReadableStreamBYOBReader has a readIntoRequests
    // property. This issue has been reported through WHATWG/streams GitHub
    // (https://github.com/whatwg/streams/issues/686), but no solution has been provided for the moment.
    // Therefore, below test is added as a temporary fix.
    if (!@isReadableStreamBYOBReader(controller.@reader))
        return;

    while (controller.@reader.@readIntoRequests.length > 0) {
        let pullIntoDescriptor = @readableByteStreamControllerShiftPendingDescriptor(controller);
        @readableByteStreamControllerCommitDescriptor(controller.@controlledReadableStream, pullIntoDescriptor);
    }
}

// Spec name: readableByteStreamControllerProcessPullIntoDescriptorsUsingQueue (shortened for readability).
function readableByteStreamControllerProcessPullDescriptors(controller)
{
    "use strict";

    @assert(!controller.@closeRequested);
    while (controller.@pendingPullIntos.length > 0) {
        if (controller.@totalQueuedBytes === 0)
            return;
        let pullIntoDescriptor = controller.@pendingPullIntos[0];
        if (@readableByteStreamControllerFillDescriptorFromQueue(controller, pullIntoDescriptor)) {
            @readableByteStreamControllerShiftPendingDescriptor(controller);
            @readableByteStreamControllerCommitDescriptor(controller.@controlledReadableStream, pullIntoDescriptor);
        }
    }
}

// Spec name: readableByteStreamControllerFillPullIntoDescriptorFromQueue (shortened for readability).
function readableByteStreamControllerFillDescriptorFromQueue(controller, pullIntoDescriptor)
{
    "use strict";

    const currentAlignedBytes = pullIntoDescriptor.bytesFilled - (pullIntoDescriptor.bytesFilled % pullIntoDescriptor.elementSize);
    const maxBytesToCopy = controller.@totalQueuedBytes < pullIntoDescriptor.byteLength - pullIntoDescriptor.bytesFilled ?
                controller.@totalQueuedBytes : pullIntoDescriptor.byteLength - pullIntoDescriptor.bytesFilled;
    const maxBytesFilled = pullIntoDescriptor.bytesFilled + maxBytesToCopy;
    const maxAlignedBytes = maxBytesFilled - (maxBytesFilled % pullIntoDescriptor.elementSize);
    let totalBytesToCopyRemaining = maxBytesToCopy;
    let ready = false;

    if (maxAlignedBytes > currentAlignedBytes) {
        totalBytesToCopyRemaining = maxAlignedBytes - pullIntoDescriptor.bytesFilled;
        ready = true;
    }

    while (totalBytesToCopyRemaining > 0) {
        let headOfQueue = controller.@queue[0];
        const bytesToCopy = totalBytesToCopyRemaining < headOfQueue.byteLength ? totalBytesToCopyRemaining : headOfQueue.byteLength;
        // Copy appropriate part of pullIntoDescriptor.buffer to headOfQueue.buffer.
        // Remark: this implementation is not completely aligned on the definition of CopyDataBlockBytes
        // operation of ECMAScript (the case of Shared Data Block is not considered here, but it doesn't seem to be an issue).
        let fromIndex = pullIntoDescriptor.byteOffset + pullIntoDescriptor.bytesFilled;
        let count = bytesToCopy;
        let toIndex = headOfQueue.byteOffset;
        while (count > 0) {
            headOfQueue.buffer[toIndex] = pullIntoDescriptor.buffer[fromIndex];
            toIndex++;
            fromIndex++;
            count--;
        }

        if (headOfQueue.byteLength === bytesToCopy)
            controller.@queue.@shift();
        else {
            headOfQueue.byteOffset += bytesToCopy;
            headOfQueue.byteLength -= bytesToCopy;
        }

        controller.@totalQueuedBytes -= bytesToCopy;
        @assert(controller.@pendingPullIntos.length === 0 || controller.@pendingPullIntos[0] === pullIntoDescriptor);
        @readableByteStreamControllerInvalidateBYOBRequest(controller);
        pullIntoDescriptor.bytesFilled += bytesToCopy;
        totalBytesToCopyRemaining -= bytesToCopy;
    }

    if (!ready) {
        @assert(controller.@totalQueuedBytes === 0);
        @assert(pullIntoDescriptor.bytesFilled > 0);
        @assert(pullIntoDescriptor.bytesFilled < pullIntoDescriptor.elementSize);
    }

    return ready;
}

// Spec name: readableByteStreamControllerShiftPendingPullInto (renamed for consistency).
function readableByteStreamControllerShiftPendingDescriptor(controller)
{
    "use strict";

    let descriptor = controller.@pendingPullIntos.@shift();
    @readableByteStreamControllerInvalidateBYOBRequest(controller);
    return descriptor;
}

function readableByteStreamControllerInvalidateBYOBRequest(controller)
{
    "use strict";

    if (controller.@byobRequest === @undefined)
        return;
    controller.@byobRequest.@associatedReadableByteStreamController = @undefined;
    controller.@byobRequest.@view = @undefined;
    controller.@byobRequest = @undefined;
}

// Spec name: readableByteStreamControllerCommitPullIntoDescriptor (shortened for readability).
function readableByteStreamControllerCommitDescriptor(stream, pullIntoDescriptor)
{
    "use strict";

    @assert(stream.@state !== @streamErrored);
    let done = false;
    if (stream.@state === @streamClosed) {
        @assert(!pullIntoDescriptor.bytesFilled);
        done = true;
    }
    let filledView = @readableByteStreamControllerConvertDescriptor(pullIntoDescriptor);
    if (pullIntoDescriptor.readerType === "default")
        @readableStreamFulfillReadRequest(stream, filledView, done);
    else {
        @assert(pullIntoDescriptor.readerType === "byob");
        @readableStreamFulfillReadIntoRequest(stream, filledView, done);
    }
}

// Spec name: readableByteStreamControllerConvertPullIntoDescriptor (shortened for readability).
function readableByteStreamControllerConvertDescriptor(pullIntoDescriptor)
{
    "use strict";

    @assert(pullIntoDescriptor.bytesFilled <= pullIntoDescriptor.byteLength);
    @assert(pullIntoDescriptor.bytesFilled % pullIntoDescriptor.elementSize === 0);

    return new pullIntoDescriptor.ctor(pullIntoDescriptor.buffer, pullIntoDescriptor.byteOffset, pullIntoDescriptor.bytesFilled / pullIntoDescriptor.elementSize);
}

function readableStreamFulfillReadIntoRequest(stream, chunk, done)
{
    "use strict";

    stream.@reader.@readIntoRequests.@shift().@resolve.@call(@undefined, {value: chunk, done: done});
}
