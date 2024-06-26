
#ifndef mc_animation_FilterOperationsWrap_h
#define mc_animation_FilterOperationsWrap_h

#include "third_party/WebKit/Source/platform/graphics/filters/FilterOperations.h"
#include "third_party/WebKit/Source/platform/heap/Heap.h"
#include "third_party/WebKit/Source/platform/heap/GarbageCollected.h"

namespace mc {

class FilterOperationsWrap : public blink::GarbageCollectedFinalized<FilterOperationsWrap> {
public:
    FilterOperationsWrap();
    FilterOperationsWrap(const FilterOperationsWrap& other);
    DECLARE_TRACE();
    blink::FilterOperations ops;
};

FilterOperationsWrap doFilterOperationsBlend(const FilterOperationsWrap& me, const FilterOperationsWrap& from, double progress);

}

#endif // mc_animation_FilterOperations_h