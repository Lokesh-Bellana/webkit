/*
 *  Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
 *  Copyright (C) 2015 Ericsson AB. All rights reserved.
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
#include "MediaStreamTrackPrivate.h"

#if ENABLE(MEDIA_STREAM)

#include "AudioSourceProvider.h"
#include "GraphicsContext.h"
#include "IntRect.h"
#include "UUID.h"

namespace WebCore {

Ref<MediaStreamTrackPrivate> MediaStreamTrackPrivate::create(Ref<RealtimeMediaSource>&& source)
{
    return adoptRef(*new MediaStreamTrackPrivate(WTFMove(source), createCanonicalUUIDString()));
}

Ref<MediaStreamTrackPrivate> MediaStreamTrackPrivate::create(Ref<RealtimeMediaSource>&& source, String&& id)
{
    return adoptRef(*new MediaStreamTrackPrivate(WTFMove(source), WTFMove(id)));
}

MediaStreamTrackPrivate::MediaStreamTrackPrivate(Ref<RealtimeMediaSource>&& source, String&& id)
    : m_source(WTFMove(source))
    , m_id(WTFMove(id))
{
    m_source->addObserver(*this);
}

MediaStreamTrackPrivate::~MediaStreamTrackPrivate()
{
    m_source->removeObserver(*this);
}

void MediaStreamTrackPrivate::addObserver(MediaStreamTrackPrivate::Observer& observer)
{
    m_observers.append(&observer);
}

void MediaStreamTrackPrivate::removeObserver(MediaStreamTrackPrivate::Observer& observer)
{
    size_t pos = m_observers.find(&observer);
    if (pos != notFound)
        m_observers.remove(pos);
}

const String& MediaStreamTrackPrivate::label() const
{
    return m_source->name();
}

bool MediaStreamTrackPrivate::muted() const
{
    return m_source->muted();
}

bool MediaStreamTrackPrivate::isCaptureTrack() const
{
    return m_source->isCaptureSource();
}

void MediaStreamTrackPrivate::setEnabled(bool enabled)
{
    if (m_isEnabled == enabled)
        return;

    // Always update the enabled state regardless of the track being ended.
    m_isEnabled = enabled;

    m_source->setEnabled(enabled);

    for (auto& observer : m_observers)
        observer->trackEnabledChanged(*this);
}

void MediaStreamTrackPrivate::endTrack()
{
    if (m_isEnded)
        return;

    // Set m_isEnded to true before telling the source it can stop, so if this is the
    // only track using the source and it does stop, we will only call each observer's
    // trackEnded method once.
    m_isEnded = true;
    updateReadyState();

    m_source->requestStop(this);

    for (auto& observer : m_observers)
        observer->trackEnded(*this);
}

Ref<MediaStreamTrackPrivate> MediaStreamTrackPrivate::clone()
{
    auto clonedMediaStreamTrackPrivate = create(m_source.copyRef());
    clonedMediaStreamTrackPrivate->m_isEnabled = this->m_isEnabled;
    clonedMediaStreamTrackPrivate->m_isEnded = this->m_isEnded;
    clonedMediaStreamTrackPrivate->updateReadyState();

    return clonedMediaStreamTrackPrivate;
}

RealtimeMediaSource::Type MediaStreamTrackPrivate::type() const
{
    return m_source->type();
}

const RealtimeMediaSourceSettings& MediaStreamTrackPrivate::settings() const
{
    return m_source->settings();
}

const RealtimeMediaSourceCapabilities& MediaStreamTrackPrivate::capabilities() const
{
    return m_source->capabilities();
}

void MediaStreamTrackPrivate::applyConstraints(const MediaConstraints& constraints, RealtimeMediaSource::SuccessHandler successHandler, RealtimeMediaSource::FailureHandler failureHandler)
{
    m_source->applyConstraints(constraints, successHandler, failureHandler);
}

AudioSourceProvider* MediaStreamTrackPrivate::audioSourceProvider()
{
    return m_source->audioSourceProvider();
}

void MediaStreamTrackPrivate::sourceStopped()
{
    if (m_isEnded)
        return;

    m_isEnded = true;
    updateReadyState();

    for (auto& observer : m_observers)
        observer->trackEnded(*this);
}

void MediaStreamTrackPrivate::sourceMutedChanged()
{
    for (auto& observer : m_observers)
        observer->trackMutedChanged(*this);
}

void MediaStreamTrackPrivate::sourceEnabledChanged()
{
    for (auto& observer : m_observers)
        observer->trackEnabledChanged(*this);
}

void MediaStreamTrackPrivate::sourceSettingsChanged()
{
    for (auto& observer : m_observers)
        observer->trackSettingsChanged(*this);
}

bool MediaStreamTrackPrivate::preventSourceFromStopping()
{
    // Do not allow the source to stop if we are still using it.
    return !m_isEnded;
}

void MediaStreamTrackPrivate::videoSampleAvailable(MediaSample& mediaSample)
{
    if (!m_haveProducedData) {
        m_haveProducedData = true;
        updateReadyState();
    }

    mediaSample.setTrackID(id());
    for (auto& observer : m_observers)
        observer->sampleBufferUpdated(*this, mediaSample);
}

void MediaStreamTrackPrivate::audioSamplesAvailable(const MediaTime&, const PlatformAudioData&, const AudioStreamDescription&, size_t)
{
    if (!m_haveProducedData) {
        m_haveProducedData = true;
        updateReadyState();
    }

    for (auto& observer : m_observers)
        observer->audioSamplesAvailable(*this);
}


void MediaStreamTrackPrivate::updateReadyState()
{
    ReadyState state = ReadyState::None;

    if (m_isEnded)
        state = ReadyState::Ended;
    else if (m_haveProducedData)
        state = ReadyState::Live;

    if (state == m_readyState)
        return;

    m_readyState = state;
    for (auto& observer : m_observers)
        observer->readyStateChanged(*this);
}

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM)
