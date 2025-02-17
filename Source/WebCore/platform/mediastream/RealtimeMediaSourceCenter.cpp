/*
 * Copyright (C) 2011 Ericsson AB. All rights reserved.
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Ericsson nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#if ENABLE(MEDIA_STREAM)
#include "RealtimeMediaSourceCenter.h"

#include "MediaStreamPrivate.h"

namespace WebCore {

static RealtimeMediaSourceCenter*& mediaStreamCenterOverride()
{
    static RealtimeMediaSourceCenter* override;
    return override;
}

RealtimeMediaSourceCenter& RealtimeMediaSourceCenter::singleton()
{
    RealtimeMediaSourceCenter* override = mediaStreamCenterOverride();
    if (override)
        return *override;
    
    return RealtimeMediaSourceCenter::platformCenter();
}

void RealtimeMediaSourceCenter::setSharedStreamCenterOverride(RealtimeMediaSourceCenter* center)
{
    mediaStreamCenterOverride() = center;
}

RealtimeMediaSourceCenter::RealtimeMediaSourceCenter()
{
}

RealtimeMediaSourceCenter::~RealtimeMediaSourceCenter()
{
}

void RealtimeMediaSourceCenter::setAudioFactory(RealtimeMediaSource::CaptureFactory& factory)
{
    m_audioFactory = &factory;
}

void RealtimeMediaSourceCenter::unsetAudioFactory(RealtimeMediaSource::CaptureFactory& factory)
{
    if (m_audioFactory == &factory)
        m_audioFactory = nullptr;
}

void RealtimeMediaSourceCenter::setVideoFactory(RealtimeMediaSource::CaptureFactory& factory)
{
    m_videoFactory = &factory;
}

void RealtimeMediaSourceCenter::unsetVideoFactory(RealtimeMediaSource::CaptureFactory& factory)
{
    if (m_videoFactory == &factory)
        m_videoFactory = nullptr;
}

void RealtimeMediaSourceCenter::setAudioCaptureDeviceManager(CaptureDeviceManager& deviceManager)
{
    m_audioCaptureDeviceManager = &deviceManager;
}

void RealtimeMediaSourceCenter::unsetAudioCaptureDeviceManager(CaptureDeviceManager& deviceManager)
{
    if (m_audioCaptureDeviceManager == &deviceManager)
        m_audioCaptureDeviceManager = nullptr;
}

void RealtimeMediaSourceCenter::setVideoCaptureDeviceManager(CaptureDeviceManager& deviceManager)
{
    m_videoCaptureDeviceManager = &deviceManager;
}

void RealtimeMediaSourceCenter::unsetVideoCaptureDeviceManager(CaptureDeviceManager& deviceManager)
{
    if (m_videoCaptureDeviceManager == &deviceManager)
        m_videoCaptureDeviceManager = nullptr;
}
    
} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM)
