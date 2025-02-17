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

#include "APIObject.h"
#include <system_error>
#include <wtf/text/WTFString.h>

namespace WTF {
class WorkQueue;
}

namespace API {

class ContentExtension;

class ContentExtensionStore final : public ObjectImpl<Object::Type::ContentExtensionStore> {
public:
    enum class Error {
        LookupFailed = 1,
        VersionMismatch,
        CompileFailed,
        RemoveFailed
    };
    
    // This should be incremented every time a functional change is made to the bytecode, file format, etc.
    // to prevent crashing while loading old data.
    // Also update ContentExtensionStore::getContentExtensionSource to be able to find the original JSON
    // source from old versions.
    const static uint32_t CurrentContentExtensionFileVersion = 9;

    static ContentExtensionStore& defaultStore();
    static Ref<ContentExtensionStore> storeWithPath(const WTF::String& storePath);

    explicit ContentExtensionStore();
    explicit ContentExtensionStore(const WTF::String& storePath);
    virtual ~ContentExtensionStore();

    void compileContentExtension(const WTF::String& identifier, WTF::String&& json, Function<void(RefPtr<API::ContentExtension>, std::error_code)>);
    void lookupContentExtension(const WTF::String& identifier, Function<void(RefPtr<API::ContentExtension>, std::error_code)>);
    void removeContentExtension(const WTF::String& identifier, Function<void(std::error_code)>);
    void getAvailableContentExtensionIdentifiers(Function<void(WTF::Vector<WTF::String>)>);

    // For testing only.
    void synchronousRemoveAllContentExtensions();
    void invalidateContentExtensionVersion(const WTF::String& identifier);
    void getContentExtensionSource(const WTF::String& identifier, Function<void(WTF::String)>);

private:
    WTF::String defaultStorePath();

    const WTF::String m_storePath;
    Ref<WTF::WorkQueue> m_compileQueue;
    Ref<WTF::WorkQueue> m_readQueue;
    Ref<WTF::WorkQueue> m_removeQueue;
};

const std::error_category& contentExtensionStoreErrorCategory();

inline std::error_code make_error_code(ContentExtensionStore::Error error)
{
    return { static_cast<int>(error), contentExtensionStoreErrorCategory() };
}

} // namespace API

namespace std {
template<> struct is_error_code_enum<API::ContentExtensionStore::Error> : public true_type { };
}

#endif // ENABLE(CONTENT_EXTENSIONS)
