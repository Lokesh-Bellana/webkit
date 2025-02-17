/*
    Copyright (C) 1999 Lars Knoll (knoll@mpi-hd.mpg.de)
    Copyright (C) 2006 Alexey Proskuryakov (ap@nypop.com)
    Copyright (C) 2006, 2008 Apple Inc. All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#pragma once

#include "TextEncoding.h"
#include <wtf/RefCounted.h>

namespace WebCore {

class HTMLMetaCharsetParser;

class TextResourceDecoder : public RefCounted<TextResourceDecoder> {
public:
    enum EncodingSource {
        DefaultEncoding,
        AutoDetectedEncoding,
        EncodingFromXMLHeader,
        EncodingFromMetaTag,
        EncodingFromCSSCharset,
        EncodingFromHTTPHeader,
        UserChosenEncoding,
        EncodingFromParentFrame
    };

    static Ref<TextResourceDecoder> create(const String& mimeType, const TextEncoding& defaultEncoding = TextEncoding(), bool usesEncodingDetector = false)
    {
        return adoptRef(*new TextResourceDecoder(mimeType, defaultEncoding, usesEncodingDetector));
    }
    WEBCORE_EXPORT ~TextResourceDecoder();

    void setEncoding(const TextEncoding&, EncodingSource);
    const TextEncoding& encoding() const { return m_encoding; }

    bool hasEqualEncodingForCharset(const String&) const;

    WEBCORE_EXPORT String decode(const char* data, size_t length);
    WEBCORE_EXPORT String flush();

    WEBCORE_EXPORT String decodeAndFlush(const char* data, size_t length);

    void setHintEncoding(const TextResourceDecoder* hintDecoder)
    {
        // hintEncoding is for use with autodetection, which should be 
        // only invoked when hintEncoding comes from auto-detection.
        if (hintDecoder && hintDecoder->m_source == AutoDetectedEncoding)
            m_hintEncoding = hintDecoder->encoding().name();
    }
   
    void useLenientXMLDecoding() { m_useLenientXMLDecoding = true; }
    bool sawError() const { return m_sawError; }
    bool encodingSet() const { return m_encodingSet; }

private:
    WEBCORE_EXPORT TextResourceDecoder(const String& mimeType, const TextEncoding& defaultEncoding, bool usesEncodingDetector);

    enum ContentType { PlainText, HTML, XML, CSS }; // PlainText only checks for BOM.
    static ContentType determineContentType(const String& mimeType);
    static const TextEncoding& defaultEncoding(ContentType, const TextEncoding& defaultEncoding);

    size_t checkForBOM(const char*, size_t);
    bool checkForCSSCharset(const char*, size_t, bool& movedDataToBuffer);
    bool checkForHeadCharset(const char*, size_t, bool& movedDataToBuffer);
    bool checkForMetaCharset(const char*, size_t);
    void detectJapaneseEncoding(const char*, size_t);
    bool shouldAutoDetect() const;

    ContentType m_contentType;
    TextEncoding m_encoding;
    std::unique_ptr<TextCodec> m_codec;
    EncodingSource m_source;
    const char* m_hintEncoding;
    Vector<char> m_buffer;
    bool m_checkedForBOM;
    bool m_checkedForCSSCharset;
    bool m_checkedForHeadCharset;
    bool m_useLenientXMLDecoding; // Don't stop on XML decoding errors.
    bool m_sawError;
    bool m_usesEncodingDetector;
    bool m_encodingSet { false };

    std::unique_ptr<HTMLMetaCharsetParser> m_charsetParser;
};

} // namespace WebCore
