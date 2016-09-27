/*
 * Copyright (c) 2008, 2009, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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

#ifndef npruntime_impl_h
#define npruntime_impl_h

#include "core/CoreExport.h"
#include <bindings/npruntime.h>

// This file exists to support WebCore, which expects to be able to call upon
// portions of the NPRuntime implementation.

#ifdef __cplusplus
extern "C" {
#endif

CORE_EXPORT NPIdentifier _NPN_GetStringIdentifier(const NPUTF8* name);
CORE_EXPORT void _NPN_GetStringIdentifiers(const NPUTF8** names, int32_t nameCount, NPIdentifier*);
CORE_EXPORT NPIdentifier _NPN_GetIntIdentifier(int32_t intId);
CORE_EXPORT bool _NPN_IdentifierIsString(NPIdentifier);
CORE_EXPORT NPUTF8 *_NPN_UTF8FromIdentifier(NPIdentifier);
CORE_EXPORT int32_t _NPN_IntFromIdentifier(NPIdentifier);
CORE_EXPORT void _NPN_ReleaseVariantValue(NPVariant*);
CORE_EXPORT NPObject *_NPN_CreateObject(NPP, NPClass*);
CORE_EXPORT NPObject* _NPN_RetainObject(NPObject*);
CORE_EXPORT void _NPN_ReleaseObject(NPObject*);
CORE_EXPORT bool _NPN_Invoke(NPP, NPObject*, NPIdentifier methodName, const NPVariant* arguments, uint32_t argumentCount, NPVariant* result);
CORE_EXPORT bool _NPN_InvokeDefault(NPP, NPObject*, const NPVariant* arguments, uint32_t argumentCount, NPVariant* result);
CORE_EXPORT bool _NPN_Evaluate(NPP, NPObject*, NPString* npScript, NPVariant* result);
CORE_EXPORT bool _NPN_EvaluateHelper(NPP, bool popupsAllowed, NPObject*, NPString* npScript, NPVariant* result);
CORE_EXPORT bool _NPN_GetProperty(NPP, NPObject*, NPIdentifier propertyName, NPVariant* result);
CORE_EXPORT bool _NPN_SetProperty(NPP, NPObject*, NPIdentifier propertyName, const NPVariant* value);
CORE_EXPORT bool _NPN_RemoveProperty(NPP, NPObject*, NPIdentifier propertyName);
CORE_EXPORT bool _NPN_HasProperty(NPP, NPObject*, NPIdentifier propertyName);
CORE_EXPORT bool _NPN_HasMethod(NPP, NPObject*, NPIdentifier methodName);
CORE_EXPORT void _NPN_SetException(NPObject*, const NPUTF8 *message);
CORE_EXPORT bool _NPN_Enumerate(NPP, NPObject*, NPIdentifier**, uint32_t* count);
CORE_EXPORT bool _NPN_Construct(NPP, NPObject*, const NPVariant* arguments, uint32_t argumentCount, NPVariant* result);

#ifdef __cplusplus
}  /* end extern "C" */
#endif

#endif
