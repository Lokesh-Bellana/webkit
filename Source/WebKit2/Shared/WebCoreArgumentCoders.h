/*
 * Copyright (C) 2010-2016 Apple Inc. All rights reserved.
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

#include "ArgumentCoders.h"
#include <WebCore/CaptureDevice.h>
#include <WebCore/ColorSpace.h>
#include <WebCore/DiagnosticLoggingClient.h>
#include <WebCore/FrameLoaderTypes.h>
#include <WebCore/IndexedDB.h>
#include <WebCore/NetworkLoadMetrics.h>
#include <WebCore/PaymentHeaders.h>
#include <WebCore/ScrollSnapOffsetsInfo.h>

namespace WTF {
class MonotonicTime;
class Seconds;
}

namespace WebCore {
class AffineTransform;
class AuthenticationChallenge;
class BlobPart;
class CertificateInfo;
class Color;
class Credential;
class CubicBezierTimingFunction;
class Cursor;
class DatabaseDetails;
class DragData;
class FilterOperation;
class FilterOperations;
class FloatPoint;
class FloatPoint3D;
class FloatRect;
class FloatRoundedRect;
class FloatSize;
class FixedPositionViewportConstraints;
class HTTPHeaderMap;
class IntPoint;
class IntRect;
class IntSize;
class KeyframeValueList;
class LayoutSize;
class LayoutPoint;
class LinearTimingFunction;
class Notification;
class Path;
class ProtectionSpace;
class Region;
class ResourceError;
class ResourceRequest;
class ResourceResponse;
class SessionID;
class SpringTimingFunction;
class StepsTimingFunction;
class StickyPositionViewportConstraints;
class TextCheckingRequestData;
class TransformationMatrix;
class UserStyleSheet;
class UserScript;
class URL;
struct CompositionUnderline;
struct DictationAlternative;
struct DictionaryPopupInfo;
struct EventTrackingRegions;
struct ExceptionDetails;
struct FileChooserSettings;
struct Length;
struct GrammarDetail;
struct MimeClassInfo;
struct PasteboardImage;
struct PasteboardWebContent;
struct PluginInfo;
struct RecentSearch;
struct ResourceLoadStatistics;
struct ScrollableAreaParameters;
struct TextCheckingResult;
struct TextIndicatorData;
struct ViewportAttributes;
struct WindowFeatures;
    
template <typename> class BoxExtent;
using FloatBoxExtent = BoxExtent<float>;
}

#if PLATFORM(COCOA)
namespace WebCore {
class MachSendRight;
struct KeypressCommand;
}
#endif

#if PLATFORM(IOS)
namespace WebCore {
class FloatQuad;
class SelectionRect;
struct Highlight;
struct PasteboardImage;
struct PasteboardWebContent;
struct ViewportArguments;
}
#endif

#if USE(SOUP)
namespace WebCore {
struct SoupNetworkProxySettings;
}
#endif

#if ENABLE(CONTENT_FILTERING)
namespace WebCore {
class ContentFilterUnblockHandler;
}
#endif

#if ENABLE(WIRELESS_PLAYBACK_TARGET)
namespace WebCore {
class MediaPlaybackTargetContext;
}
#endif

#if ENABLE(MEDIA_SESSION)
namespace WebCore {
class MediaSessionMetadata;
}
#endif

#if ENABLE(MEDIA_STREAM)
namespace WebCore {
class CaptureDevice;
struct MediaConstraintsData;
}
#endif

#if ENABLE(INDEXED_DATABASE)
namespace WebCore {
using IDBKeyPath = Variant<String, Vector<String>>;
}
#endif

namespace IPC {

template<> struct ArgumentCoder<WTF::MonotonicTime> {
    static void encode(Encoder&, const WTF::MonotonicTime&);
    static bool decode(Decoder&, WTF::MonotonicTime&);
};

template<> struct ArgumentCoder<WTF::Seconds> {
    static void encode(Encoder&, const WTF::Seconds&);
    static bool decode(Decoder&, WTF::Seconds&);
};

template<> struct ArgumentCoder<WebCore::AffineTransform> {
    static void encode(Encoder&, const WebCore::AffineTransform&);
    static bool decode(Decoder&, WebCore::AffineTransform&);
};

template<> struct ArgumentCoder<WebCore::EventTrackingRegions> {
    static void encode(Encoder&, const WebCore::EventTrackingRegions&);
    static bool decode(Decoder&, WebCore::EventTrackingRegions&);
};

template<> struct ArgumentCoder<WebCore::TransformationMatrix> {
    static void encode(Encoder&, const WebCore::TransformationMatrix&);
    static bool decode(Decoder&, WebCore::TransformationMatrix&);
};

template<> struct ArgumentCoder<WebCore::LinearTimingFunction> {
    static void encode(Encoder&, const WebCore::LinearTimingFunction&);
    static bool decode(Decoder&, WebCore::LinearTimingFunction&);
};

template<> struct ArgumentCoder<WebCore::CubicBezierTimingFunction> {
    static void encode(Encoder&, const WebCore::CubicBezierTimingFunction&);
    static bool decode(Decoder&, WebCore::CubicBezierTimingFunction&);
};

template<> struct ArgumentCoder<WebCore::StepsTimingFunction> {
    static void encode(Encoder&, const WebCore::StepsTimingFunction&);
    static bool decode(Decoder&, WebCore::StepsTimingFunction&);
};

template<> struct ArgumentCoder<WebCore::SpringTimingFunction> {
    static void encode(Encoder&, const WebCore::SpringTimingFunction&);
    static bool decode(Decoder&, WebCore::SpringTimingFunction&);
};

template<> struct ArgumentCoder<WebCore::CertificateInfo> {
    static void encode(Encoder&, const WebCore::CertificateInfo&);
    static bool decode(Decoder&, WebCore::CertificateInfo&);
};

template<> struct ArgumentCoder<WebCore::FloatPoint> {
    static void encode(Encoder&, const WebCore::FloatPoint&);
    static bool decode(Decoder&, WebCore::FloatPoint&);
};

template<> struct ArgumentCoder<WebCore::FloatPoint3D> {
    static void encode(Encoder&, const WebCore::FloatPoint3D&);
    static bool decode(Decoder&, WebCore::FloatPoint3D&);
};

template<> struct ArgumentCoder<WebCore::FloatRect> {
    static void encode(Encoder&, const WebCore::FloatRect&);
    static bool decode(Decoder&, WebCore::FloatRect&);
};
    
template<> struct ArgumentCoder<WebCore::FloatBoxExtent> {
    static void encode(Encoder&, const WebCore::FloatBoxExtent&);
    static bool decode(Decoder&, WebCore::FloatBoxExtent&);
};

template<> struct ArgumentCoder<WebCore::FloatSize> {
    static void encode(Encoder&, const WebCore::FloatSize&);
    static bool decode(Decoder&, WebCore::FloatSize&);
};

template<> struct ArgumentCoder<WebCore::FloatRoundedRect> {
    static void encode(Encoder&, const WebCore::FloatRoundedRect&);
    static bool decode(Decoder&, WebCore::FloatRoundedRect&);
};

#if PLATFORM(IOS)
template<> struct ArgumentCoder<WebCore::FloatQuad> {
    static void encode(Encoder&, const WebCore::FloatQuad&);
    static bool decode(Decoder&, WebCore::FloatQuad&);
};

template<> struct ArgumentCoder<WebCore::ViewportArguments> {
    static void encode(Encoder&, const WebCore::ViewportArguments&);
    static bool decode(Decoder&, WebCore::ViewportArguments&);
};
#endif // PLATFORM(IOS)

template<> struct ArgumentCoder<WebCore::IntPoint> {
    static void encode(Encoder&, const WebCore::IntPoint&);
    static bool decode(Decoder&, WebCore::IntPoint&);
};

template<> struct ArgumentCoder<WebCore::IntRect> {
    static void encode(Encoder&, const WebCore::IntRect&);
    static bool decode(Decoder&, WebCore::IntRect&);
};

template<> struct ArgumentCoder<WebCore::IntSize> {
    static void encode(Encoder&, const WebCore::IntSize&);
    static bool decode(Decoder&, WebCore::IntSize&);
};

template<> struct ArgumentCoder<WebCore::LayoutSize> {
    static void encode(Encoder&, const WebCore::LayoutSize&);
    static bool decode(Decoder&, WebCore::LayoutSize&);
};

template<> struct ArgumentCoder<WebCore::LayoutPoint> {
    static void encode(Encoder&, const WebCore::LayoutPoint&);
    static bool decode(Decoder&, WebCore::LayoutPoint&);
};

template<> struct ArgumentCoder<WebCore::Path> {
    static void encode(Encoder&, const WebCore::Path&);
    static bool decode(Decoder&, WebCore::Path&);
};

template<> struct ArgumentCoder<WebCore::Region> {
    static void encode(Encoder&, const WebCore::Region&);
    static bool decode(Decoder&, WebCore::Region&);
};

template<> struct ArgumentCoder<WebCore::Length> {
    static void encode(Encoder&, const WebCore::Length&);
    static bool decode(Decoder&, WebCore::Length&);
};

template<> struct ArgumentCoder<WebCore::ViewportAttributes> {
    static void encode(Encoder&, const WebCore::ViewportAttributes&);
    static bool decode(Decoder&, WebCore::ViewportAttributes&);
};

template<> struct ArgumentCoder<WebCore::MimeClassInfo> {
    static void encode(Encoder&, const WebCore::MimeClassInfo&);
    static bool decode(Decoder&, WebCore::MimeClassInfo&);
};

template<> struct ArgumentCoder<WebCore::PluginInfo> {
    static void encode(Encoder&, const WebCore::PluginInfo&);
    static bool decode(Decoder&, WebCore::PluginInfo&);
};

template<> struct ArgumentCoder<WebCore::AuthenticationChallenge> {
    static void encode(Encoder&, const WebCore::AuthenticationChallenge&);
    static bool decode(Decoder&, WebCore::AuthenticationChallenge&);
};

template<> struct ArgumentCoder<WebCore::ProtectionSpace> {
    static void encode(Encoder&, const WebCore::ProtectionSpace&);
    static bool decode(Decoder&, WebCore::ProtectionSpace&);
    static void encodePlatformData(Encoder&, const WebCore::ProtectionSpace&);
    static bool decodePlatformData(Decoder&, WebCore::ProtectionSpace&);
};

template<> struct ArgumentCoder<WebCore::Credential> {
    static void encode(Encoder&, const WebCore::Credential&);
    static bool decode(Decoder&, WebCore::Credential&);
    static void encodePlatformData(Encoder&, const WebCore::Credential&);
    static bool decodePlatformData(Decoder&, WebCore::Credential&);
};

template<> struct ArgumentCoder<WebCore::Cursor> {
    static void encode(Encoder&, const WebCore::Cursor&);
    static bool decode(Decoder&, WebCore::Cursor&);
};

template<> struct ArgumentCoder<WebCore::ResourceRequest> {
    static void encode(Encoder&, const WebCore::ResourceRequest&);
    static bool decode(Decoder&, WebCore::ResourceRequest&);
    static void encodePlatformData(Encoder&, const WebCore::ResourceRequest&);
    static bool decodePlatformData(Decoder&, WebCore::ResourceRequest&);
};

template<> struct ArgumentCoder<WebCore::ResourceError> {
    static void encode(Encoder&, const WebCore::ResourceError&);
    static bool decode(Decoder&, WebCore::ResourceError&);
    static void encodePlatformData(Encoder&, const WebCore::ResourceError&);
    static bool decodePlatformData(Decoder&, WebCore::ResourceError&);
};

template<> struct ArgumentCoder<WebCore::WindowFeatures> {
    static void encode(Encoder&, const WebCore::WindowFeatures&);
    static bool decode(Decoder&, WebCore::WindowFeatures&);
};

template<> struct ArgumentCoder<WebCore::Color> {
    static void encode(Encoder&, const WebCore::Color&);
    static bool decode(Decoder&, WebCore::Color&);
};

#if ENABLE(DRAG_SUPPORT)
template<> struct ArgumentCoder<WebCore::DragData> {
    static void encode(Encoder&, const WebCore::DragData&);
    static bool decode(Decoder&, WebCore::DragData&);
};
#endif

#if PLATFORM(COCOA)
template<> struct ArgumentCoder<WebCore::MachSendRight> {
    static void encode(Encoder&, const WebCore::MachSendRight&);
    static void encode(Encoder&, WebCore::MachSendRight&&);
    static bool decode(Decoder&, WebCore::MachSendRight&);
};

template<> struct ArgumentCoder<WebCore::KeypressCommand> {
    static void encode(Encoder&, const WebCore::KeypressCommand&);
    static bool decode(Decoder&, WebCore::KeypressCommand&);
};
#endif

#if PLATFORM(IOS)
template<> struct ArgumentCoder<WebCore::SelectionRect> {
    static void encode(Encoder&, const WebCore::SelectionRect&);
    static bool decode(Decoder&, WebCore::SelectionRect&);
};

template<> struct ArgumentCoder<WebCore::Highlight> {
    static void encode(Encoder&, const WebCore::Highlight&);
    static bool decode(Decoder&, WebCore::Highlight&);
};

template<> struct ArgumentCoder<WebCore::PasteboardWebContent> {
    static void encode(Encoder&, const WebCore::PasteboardWebContent&);
    static bool decode(Decoder&, WebCore::PasteboardWebContent&);
};

template<> struct ArgumentCoder<WebCore::PasteboardImage> {
    static void encode(Encoder&, const WebCore::PasteboardImage&);
    static bool decode(Decoder&, WebCore::PasteboardImage&);
};
#endif

#if USE(SOUP)
template<> struct ArgumentCoder<WebCore::SoupNetworkProxySettings> {
    static void encode(Encoder&, const WebCore::SoupNetworkProxySettings&);
    static bool decode(Decoder&, WebCore::SoupNetworkProxySettings&);
};
#endif

template<> struct ArgumentCoder<WebCore::CompositionUnderline> {
    static void encode(Encoder&, const WebCore::CompositionUnderline&);
    static bool decode(Decoder&, WebCore::CompositionUnderline&);
};

template<> struct ArgumentCoder<WebCore::DatabaseDetails> {
    static void encode(Encoder&, const WebCore::DatabaseDetails&);
    static bool decode(Decoder&, WebCore::DatabaseDetails&);
};

template<> struct ArgumentCoder<WebCore::DictationAlternative> {
    static void encode(Encoder&, const WebCore::DictationAlternative&);
    static bool decode(Decoder&, WebCore::DictationAlternative&);
};

template<> struct ArgumentCoder<WebCore::FileChooserSettings> {
    static void encode(Encoder&, const WebCore::FileChooserSettings&);
    static bool decode(Decoder&, WebCore::FileChooserSettings&);
};

template<> struct ArgumentCoder<WebCore::GrammarDetail> {
    static void encode(Encoder&, const WebCore::GrammarDetail&);
    static bool decode(Decoder&, WebCore::GrammarDetail&);
};

template<> struct ArgumentCoder<WebCore::TextCheckingRequestData> {
    static void encode(Encoder&, const WebCore::TextCheckingRequestData&);
    static bool decode(Decoder&, WebCore::TextCheckingRequestData&);
};

template<> struct ArgumentCoder<WebCore::TextCheckingResult> {
    static void encode(Encoder&, const WebCore::TextCheckingResult&);
    static bool decode(Decoder&, WebCore::TextCheckingResult&);
};
    
template<> struct ArgumentCoder<WebCore::URL> {
    static void encode(Encoder&, const WebCore::URL&);
    static bool decode(Decoder&, WebCore::URL&);
};

template<> struct ArgumentCoder<WebCore::UserStyleSheet> {
    static void encode(Encoder&, const WebCore::UserStyleSheet&);
    static bool decode(Decoder&, WebCore::UserStyleSheet&);
};

template<> struct ArgumentCoder<WebCore::UserScript> {
    static void encode(Encoder&, const WebCore::UserScript&);
    static bool decode(Decoder&, WebCore::UserScript&);
};

template<> struct ArgumentCoder<WebCore::ScrollableAreaParameters> {
    static void encode(Encoder&, const WebCore::ScrollableAreaParameters&);
    static bool decode(Decoder&, WebCore::ScrollableAreaParameters&);
};

template<> struct ArgumentCoder<WebCore::FixedPositionViewportConstraints> {
    static void encode(Encoder&, const WebCore::FixedPositionViewportConstraints&);
    static bool decode(Decoder&, WebCore::FixedPositionViewportConstraints&);
};

template<> struct ArgumentCoder<WebCore::StickyPositionViewportConstraints> {
    static void encode(Encoder&, const WebCore::StickyPositionViewportConstraints&);
    static bool decode(Decoder&, WebCore::StickyPositionViewportConstraints&);
};

#if !USE(COORDINATED_GRAPHICS)
template<> struct ArgumentCoder<WebCore::FilterOperations> {
    static void encode(Encoder&, const WebCore::FilterOperations&);
    static bool decode(Decoder&, WebCore::FilterOperations&);
};
    
template<> struct ArgumentCoder<WebCore::FilterOperation> {
    static void encode(Encoder&, const WebCore::FilterOperation&);
};
bool decodeFilterOperation(Decoder&, RefPtr<WebCore::FilterOperation>&);
#endif

template<> struct ArgumentCoder<WebCore::SessionID> {
    static void encode(Encoder&, const WebCore::SessionID&);
    static bool decode(Decoder&, WebCore::SessionID&);
};

template<> struct ArgumentCoder<WebCore::BlobPart> {
    static void encode(Encoder&, const WebCore::BlobPart&);
    static bool decode(Decoder&, WebCore::BlobPart&);
};

#if ENABLE(CONTENT_FILTERING)
template<> struct ArgumentCoder<WebCore::ContentFilterUnblockHandler> {
    static void encode(Encoder&, const WebCore::ContentFilterUnblockHandler&);
    static bool decode(Decoder&, WebCore::ContentFilterUnblockHandler&);
};
#endif

#if ENABLE(MEDIA_SESSION)
template<> struct ArgumentCoder<WebCore::MediaSessionMetadata> {
    static void encode(Encoder&, const WebCore::MediaSessionMetadata&);
    static bool decode(Decoder&, WebCore::MediaSessionMetadata&);
};
#endif

template<> struct ArgumentCoder<WebCore::TextIndicatorData> {
    static void encode(Encoder&, const WebCore::TextIndicatorData&);
    static bool decode(Decoder&, WebCore::TextIndicatorData&);
};

template<> struct ArgumentCoder<WebCore::DictionaryPopupInfo> {
    static void encode(Encoder&, const WebCore::DictionaryPopupInfo&);
    static bool decode(Decoder&, WebCore::DictionaryPopupInfo&);
};

#if ENABLE(WIRELESS_PLAYBACK_TARGET)
template<> struct ArgumentCoder<WebCore::MediaPlaybackTargetContext> {
    static void encode(Encoder&, const WebCore::MediaPlaybackTargetContext&);
    static bool decode(Decoder&, WebCore::MediaPlaybackTargetContext&);
    static void encodePlatformData(Encoder&, const WebCore::MediaPlaybackTargetContext&);
    static bool decodePlatformData(Decoder&, WebCore::MediaPlaybackTargetContext&);
};
#endif

template<> struct ArgumentCoder<WebCore::RecentSearch> {
    static void encode(Encoder&, const WebCore::RecentSearch&);
    static bool decode(Decoder&, WebCore::RecentSearch&);
};

template<> struct ArgumentCoder<WebCore::ExceptionDetails> {
    static void encode(Encoder&, const WebCore::ExceptionDetails&);
    static bool decode(Decoder&, WebCore::ExceptionDetails&);
};

template<> struct ArgumentCoder<WebCore::ResourceLoadStatistics> {
    static void encode(Encoder&, const WebCore::ResourceLoadStatistics&);
    static bool decode(Decoder&, WebCore::ResourceLoadStatistics&);
};

#if ENABLE(APPLE_PAY)

template<> struct ArgumentCoder<WebCore::Payment> {
    static void encode(Encoder&, const WebCore::Payment&);
    static bool decode(Decoder&, WebCore::Payment&);
};

template<> struct ArgumentCoder<WebCore::PaymentAuthorizationResult> {
    static void encode(Encoder&, const WebCore::PaymentAuthorizationResult&);
    static bool decode(Decoder&, WebCore::PaymentAuthorizationResult&);
};

template<> struct ArgumentCoder<WebCore::PaymentContact> {
    static void encode(Encoder&, const WebCore::PaymentContact&);
    static bool decode(Decoder&, WebCore::PaymentContact&);
};

template<> struct ArgumentCoder<WebCore::PaymentError> {
    static void encode(Encoder&, const WebCore::PaymentError&);
    static bool decode(Decoder&, WebCore::PaymentError&);
};

template<> struct ArgumentCoder<WebCore::PaymentMerchantSession> {
    static void encode(Encoder&, const WebCore::PaymentMerchantSession&);
    static bool decode(Decoder&, WebCore::PaymentMerchantSession&);
};

template<> struct ArgumentCoder<WebCore::PaymentMethod> {
    static void encode(Encoder&, const WebCore::PaymentMethod&);
    static bool decode(Decoder&, WebCore::PaymentMethod&);
};

template<> struct ArgumentCoder<WebCore::PaymentMethodUpdate> {
    static void encode(Encoder&, const WebCore::PaymentMethodUpdate&);
    static bool decode(Decoder&, WebCore::PaymentMethodUpdate&);
};

template<> struct ArgumentCoder<WebCore::PaymentRequest> {
    static void encode(Encoder&, const WebCore::PaymentRequest&);
    static bool decode(Decoder&, WebCore::PaymentRequest&);
};

template<> struct ArgumentCoder<WebCore::PaymentRequest::ContactFields> {
    static void encode(Encoder&, const WebCore::PaymentRequest::ContactFields&);
    static bool decode(Decoder&, WebCore::PaymentRequest::ContactFields&);
};

template<> struct ArgumentCoder<WebCore::PaymentRequest::LineItem> {
    static void encode(Encoder&, const WebCore::PaymentRequest::LineItem&);
    static bool decode(Decoder&, WebCore::PaymentRequest::LineItem&);
};

template<> struct ArgumentCoder<WebCore::PaymentRequest::MerchantCapabilities> {
    static void encode(Encoder&, const WebCore::PaymentRequest::MerchantCapabilities&);
    static bool decode(Decoder&, WebCore::PaymentRequest::MerchantCapabilities&);
};

template<> struct ArgumentCoder<WebCore::PaymentRequest::ShippingMethod> {
    static void encode(Encoder&, const WebCore::PaymentRequest::ShippingMethod&);
    static bool decode(Decoder&, WebCore::PaymentRequest::ShippingMethod&);
};

template<> struct ArgumentCoder<WebCore::PaymentRequest::TotalAndLineItems> {
    static void encode(Encoder&, const WebCore::PaymentRequest::TotalAndLineItems&);
    static bool decode(Decoder&, WebCore::PaymentRequest::TotalAndLineItems&);
};

template<> struct ArgumentCoder<WebCore::ShippingContactUpdate> {
    static void encode(Encoder&, const WebCore::ShippingContactUpdate&);
    static bool decode(Decoder&, WebCore::ShippingContactUpdate&);
};

template<> struct ArgumentCoder<WebCore::ShippingMethodUpdate> {
    static void encode(Encoder&, const WebCore::ShippingMethodUpdate&);
    static bool decode(Decoder&, WebCore::ShippingMethodUpdate&);
};

#endif

#if ENABLE(MEDIA_STREAM)
template<> struct ArgumentCoder<WebCore::MediaConstraintsData> {
    static void encode(Encoder&, const WebCore::MediaConstraintsData&);
    static bool decode(Decoder&, WebCore::MediaConstraintsData&);
};

template<> struct ArgumentCoder<WebCore::CaptureDevice> {
    static void encode(Encoder&, const WebCore::CaptureDevice&);
    static bool decode(Decoder&, WebCore::CaptureDevice&);
};
#endif

#if ENABLE(INDEXED_DATABASE)

template<> struct ArgumentCoder<WebCore::IDBKeyPath> {
    static void encode(Encoder&, const WebCore::IDBKeyPath&);
    static bool decode(Decoder&, WebCore::IDBKeyPath&);
};

#endif

#if ENABLE(CSS_SCROLL_SNAP)

template<> struct ArgumentCoder<WebCore::ScrollOffsetRange<float>> {
    static void encode(Encoder&, const WebCore::ScrollOffsetRange<float>&);
    static bool decode(Decoder&, WebCore::ScrollOffsetRange<float>&);
};

#endif

} // namespace IPC

namespace WTF {

template<> struct EnumTraits<WebCore::ColorSpace> {
    using values = EnumValues<
    WebCore::ColorSpace,
    WebCore::ColorSpace::ColorSpaceDeviceRGB,
    WebCore::ColorSpace::ColorSpaceSRGB,
    WebCore::ColorSpace::ColorSpaceLinearRGB,
    WebCore::ColorSpace::ColorSpaceDisplayP3
    >;
};

template<> struct EnumTraits<WebCore::HasInsecureContent> {
    using values = EnumValues<
        WebCore::HasInsecureContent,
        WebCore::HasInsecureContent::No,
        WebCore::HasInsecureContent::Yes
    >;
};

template<> struct EnumTraits<WebCore::ShouldSample> {
    using values = EnumValues<
        WebCore::ShouldSample,
        WebCore::ShouldSample::No,
        WebCore::ShouldSample::Yes
    >;
};

template<> struct EnumTraits<WebCore::NetworkLoadPriority> {
    using values = EnumValues<
        WebCore::NetworkLoadPriority,
        WebCore::NetworkLoadPriority::Low,
        WebCore::NetworkLoadPriority::Medium,
        WebCore::NetworkLoadPriority::High
    >;
};

#if ENABLE(INDEXED_DATABASE)
template<> struct EnumTraits<WebCore::IndexedDB::GetAllType> {
    using values = EnumValues<
        WebCore::IndexedDB::GetAllType,
        WebCore::IndexedDB::GetAllType::Keys,
        WebCore::IndexedDB::GetAllType::Values
    >;
};
#endif

#if ENABLE(MEDIA_STREAM)
template<> struct EnumTraits<WebCore::CaptureDevice::DeviceType> {
    using values = EnumValues<
        WebCore::CaptureDevice::DeviceType,
        WebCore::CaptureDevice::DeviceType::Unknown,
        WebCore::CaptureDevice::DeviceType::Audio,
        WebCore::CaptureDevice::DeviceType::Video
    >;
};
#endif

} // namespace WTF
