/*
 * Copyright (C) 2017 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <WebCore/ResourceRequest.h>

namespace WebCore {
class ResourceError;
class ResourceLoader;
class ResourceResponse;
}

namespace WebKit {

class WebURLSchemeHandlerProxy;

class WebURLSchemeHandlerTaskProxy {
    WTF_MAKE_NONCOPYABLE(WebURLSchemeHandlerTaskProxy);
public:
    WebURLSchemeHandlerTaskProxy(WebURLSchemeHandlerProxy&, WebCore::ResourceLoader&);

    const WebCore::ResourceRequest& request() const { return m_request; }

    void startLoading();
    void stopLoading();

    void didReceiveResponse(const WebCore::ResourceResponse&);
    void didReceiveData(size_t, const uint8_t* data);
    void didComplete(const WebCore::ResourceError&);

    unsigned long identifier() const { return m_identifier; }

private:
    bool hasLoader();

    WebURLSchemeHandlerProxy& m_urlSchemeHandler;
    RefPtr<WebCore::ResourceLoader> m_coreLoader;
    WebCore::ResourceRequest m_request;
    unsigned long m_identifier;
};

} // namespace WebKit
