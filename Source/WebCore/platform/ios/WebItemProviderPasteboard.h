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

#import <WebCore/AbstractPasteboard.h>

#if TARGET_OS_IPHONE && __IPHONE_OS_VERSION_MIN_REQUIRED >= 110000

@class UIItemProvider;

NS_ASSUME_NONNULL_BEGIN

typedef void (^WebItemProviderFileLoadBlock)(NSArray<NSURL *> *);

WEBCORE_EXPORT @interface WebItemProviderPasteboard : NSObject<AbstractPasteboard>

+ (instancetype)sharedInstance;

- (UIItemProvider *)itemProviderAtIndex:(NSInteger)index;

@property (copy, nonatomic, nullable) NSArray<UIItemProvider *> *itemProviders;
@property (readonly, nonatomic) NSInteger numberOfItems;
@property (readonly, nonatomic) NSInteger changeCount;

// This will only be non-empty when an operation is being performed.
@property (readonly, nonatomic) NSArray<NSURL *> *filenamesForDataInteraction;

@property (readonly, nonatomic) BOOL hasPendingOperation;
- (void)incrementPendingOperationCount;
- (void)decrementPendingOperationCount;

- (void)enumerateItemProvidersWithBlock:(void (^)(UIItemProvider *itemProvider, NSUInteger index, BOOL *stop))block;

// The given completion block is always dispatched on the main thread.
- (void)doAfterLoadingProvidedContentIntoFileURLs:(WebItemProviderFileLoadBlock)action;

@end

NS_ASSUME_NONNULL_END

#endif // TARGET_OS_IPHONE && __IPHONE_OS_VERSION_MIN_REQUIRED >= 110000
