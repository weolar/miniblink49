/*
 * Copyright (C) 2006, 2009 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"
#include "wtf/text/StringImpl.h"

#import <CoreFoundation/CFBase.h>
#import <Foundation/NSObject.h>
#include "wtf/RetainPtr.h"

namespace WTF {

// Use HardAutorelease to return an object made by a CoreFoundation
// "create" or "copy" function as an autoreleased and garbage collected
// object. CF objects need to be "made collectable" for autorelease to work
// properly under GC.

static inline id HardAutorelease(CFTypeRef object)
{
    if (object)
        CFMakeCollectable(object);
    [(id)object autorelease];
    return (id)object;
}

StringImpl::operator NSString *()
{
    return HardAutorelease(createCFString().leakRef());
}

}
