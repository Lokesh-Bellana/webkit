/*
 * Copyright (C) 2015 Apple Inc. All rights reserved.
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

#if ENABLE(CONTENT_EXTENSIONS)

#include "WebCompiledContentExtensionData.h"
#include <WebCore/ContentExtensionCompiler.h>
#include <WebCore/CompiledContentExtension.h>

namespace WebKit {

class WebCompiledContentExtension final : public WebCore::ContentExtensions::CompiledContentExtension {
public:
    static Ref<WebCompiledContentExtension> create(WebCompiledContentExtensionData&&);
    virtual ~WebCompiledContentExtension();

    WebCompiledContentExtensionData data() const { return m_data; }

private:
    WebCompiledContentExtension(WebCompiledContentExtensionData&&);

    const WebCore::ContentExtensions::DFABytecode* filtersWithoutConditionsBytecode() const final;
    unsigned filtersWithoutConditionsBytecodeLength() const final;
    const WebCore::ContentExtensions::DFABytecode* filtersWithConditionsBytecode() const final;
    unsigned filtersWithConditionsBytecodeLength() const final;
    const WebCore::ContentExtensions::DFABytecode* topURLFiltersBytecode() const final;
    unsigned topURLFiltersBytecodeLength() const final;
    bool conditionsApplyOnlyToDomain() const final;
    
    const WebCore::ContentExtensions::SerializedActionByte* actions() const final;
    unsigned actionsLength() const final;
    
    WebCompiledContentExtensionData m_data;
};

} // namespace WebKit

#endif // ENABLE(CONTENT_EXTENSIONS)
