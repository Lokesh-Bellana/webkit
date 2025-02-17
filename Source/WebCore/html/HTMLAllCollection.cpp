/*
 * Copyright (C) 2009, 2011, 2012 Apple Inc. All rights reserved.
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

#include "config.h"
#include "HTMLAllCollection.h"

#include "Element.h"

namespace WebCore {

Ref<HTMLAllCollection> HTMLAllCollection::create(Document& document, CollectionType type)
{
    return adoptRef(*new HTMLAllCollection(document, type));
}

inline HTMLAllCollection::HTMLAllCollection(Document& document, CollectionType type)
    : AllDescendantsCollection(document, type)
{
}

Element* HTMLAllCollection::namedItemWithIndex(const AtomicString& name, unsigned index) const
{
    updateNamedElementCache();
    const CollectionNamedElementCache& cache = namedItemCaches();

    if (const Vector<Element*>* elements = cache.findElementsWithId(name)) {
        if (index < elements->size())
            return elements->at(index);
        index -= elements->size();
    }

    if (const Vector<Element*>* elements = cache.findElementsWithName(name)) {
        if (index < elements->size())
            return elements->at(index);
    }

    return nullptr;
}

} // namespace WebCore
