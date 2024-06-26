// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/surfaces/surface_manager.h"

#include "base/logging.h"
#include "cc/surfaces/surface.h"
#include "cc/surfaces/surface_id_allocator.h"

namespace cc {

SurfaceManager::SurfaceManager()
{
    thread_checker_.DetachFromThread();
}

SurfaceManager::~SurfaceManager()
{
    DCHECK(thread_checker_.CalledOnValidThread());
    for (SurfaceDestroyList::iterator it = surfaces_to_destroy_.begin();
         it != surfaces_to_destroy_.end();
         ++it) {
        DeregisterSurface((*it)->surface_id());
        delete *it;
    }
}

void SurfaceManager::RegisterSurface(Surface* surface)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK(surface);
    DCHECK(!surface_map_.count(surface->surface_id()));
    surface_map_[surface->surface_id()] = surface;
}

void SurfaceManager::DeregisterSurface(SurfaceId surface_id)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    SurfaceMap::iterator it = surface_map_.find(surface_id);
    DCHECK(it != surface_map_.end());
    surface_map_.erase(it);
}

void SurfaceManager::Destroy(scoped_ptr<Surface> surface)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    surface->set_destroyed(true);
    surfaces_to_destroy_.push_back(surface.release());
    GarbageCollectSurfaces();
}

void SurfaceManager::DidSatisfySequences(uint32_t id_namespace,
    std::vector<uint32_t>* sequence)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    for (std::vector<uint32_t>::iterator it = sequence->begin();
         it != sequence->end();
         ++it) {
        satisfied_sequences_.insert(SurfaceSequence(id_namespace, *it));
    }
    sequence->clear();
    GarbageCollectSurfaces();
}

void SurfaceManager::RegisterSurfaceIdNamespace(uint32_t id_namespace)
{
    bool inserted = valid_surface_id_namespaces_.insert(id_namespace).second;
    DCHECK(inserted);
}

void SurfaceManager::InvalidateSurfaceIdNamespace(uint32_t id_namespace)
{
    valid_surface_id_namespaces_.erase(id_namespace);
    GarbageCollectSurfaces();
}

void SurfaceManager::GarbageCollectSurfaces()
{
    // Simple mark and sweep GC.
    // TODO(jbauman): Reduce the amount of work when nothing needs to be
    // destroyed.
    std::vector<SurfaceId> live_surfaces;
    std::set<SurfaceId> live_surfaces_set;

    // GC roots are surfaces that have not been destroyed, or have not had all
    // their destruction dependencies satisfied.
    for (auto& map_entry : surface_map_) {
        map_entry.second->SatisfyDestructionDependencies(
            &satisfied_sequences_, &valid_surface_id_namespaces_);
        if (!map_entry.second->destroyed() || map_entry.second->GetDestructionDependencyCount()) {
            live_surfaces_set.insert(map_entry.first);
            live_surfaces.push_back(map_entry.first);
        }
    }

    // Mark all surfaces reachable from live surfaces by adding them to
    // live_surfaces and live_surfaces_set.
    for (size_t i = 0; i < live_surfaces.size(); i++) {
        Surface* surf = surface_map_[live_surfaces[i]];
        DCHECK(surf);

        for (SurfaceId id : surf->referenced_surfaces()) {
            if (live_surfaces_set.count(id))
                continue;

            Surface* surf2 = GetSurfaceForId(id);
            if (surf2) {
                live_surfaces.push_back(id);
                live_surfaces_set.insert(id);
            }
        }
    }

    // Destroy all remaining unreachable surfaces.
    for (SurfaceDestroyList::iterator dest_it = surfaces_to_destroy_.begin();
         dest_it != surfaces_to_destroy_.end();) {
        if (!live_surfaces_set.count((*dest_it)->surface_id())) {
            scoped_ptr<Surface> surf(*dest_it);
            DeregisterSurface(surf->surface_id());
            dest_it = surfaces_to_destroy_.erase(dest_it);
        } else {
            ++dest_it;
        }
    }
}

Surface* SurfaceManager::GetSurfaceForId(SurfaceId surface_id)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    SurfaceMap::iterator it = surface_map_.find(surface_id);
    if (it == surface_map_.end())
        return NULL;
    return it->second;
}

bool SurfaceManager::SurfaceModified(SurfaceId surface_id)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    bool changed = false;
    FOR_EACH_OBSERVER(SurfaceDamageObserver, observer_list_,
        OnSurfaceDamaged(surface_id, &changed));
    return changed;
}

} // namespace cc
