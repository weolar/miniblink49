/*
* Copyright (C) 2008 Nikolas Zimmermann <zimmermann@kde.org>
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

#ifndef ModuleScriptLoader_h
#define ModuleScriptLoader_h

#include "core/CoreExport.h"
#include "core/fetch/ResourceOwner.h"
#include "core/fetch/ScriptResource.h"
#include "core/fetch/FetchRequest.h"
#include "platform/heap/Handle.h"

namespace blink {

class Document;
class ScriptLoaderClient;
class ScriptSourceCode;
class ScriptPromiseResolver;
class Modulator;

class ModuleScriptLoader 
    : public NoBaseWillBeGarbageCollectedFinalized<ModuleScriptLoader>
    , private ScriptResourceClient {
public:
    static ModuleScriptLoader* create(Document* document, const KURL& url, FetchRequest::DeferOption defer);

    ResourcePtr<ScriptResource> resource() { return m_resource; }

    void setModulator(Modulator* modulator)
    {
        m_modulator = modulator;
    }

protected:
    DECLARE_TRACE();

    ModuleScriptLoader();
    ~ModuleScriptLoader();

    // ResourceClient
    void notifyFinished(Resource*) override;

    //Member<ScriptPromiseResolver> m_promiseResolver;
    Member<Modulator> m_modulator;
    ResourcePtr<ScriptResource> m_resource;
};

}

#endif // ModuleScriptLoader_h
