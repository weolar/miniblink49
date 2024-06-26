
#include "mc/animation/FilterOperationsWrap.h"

namespace mc {

FilterOperationsWrap::FilterOperationsWrap()
{

}

FilterOperationsWrap::FilterOperationsWrap(const FilterOperationsWrap& other)
{
    ops = other.ops;
}
    
FilterOperationsWrap doFilterOperationsBlend(const FilterOperationsWrap& me, const FilterOperationsWrap& from, double progress)
{
    if (me.ops.hasReferenceFilter() || from.ops.hasReferenceFilter())
        return me;

    bool fromIsLonger = from.ops.size() > me.ops.size();

    size_t shorterSize = 0;
    size_t longerSize = 0;
    if (me.ops.size() == from.ops.size()) {
        shorterSize = longerSize = me.ops.size();
    } else if (fromIsLonger) {
        longerSize = from.ops.size();
        shorterSize = me.ops.size();
    } else {
        longerSize = me.ops.size();
        shorterSize = from.ops.size();
    }

    for (size_t i = 0; i < shorterSize; i++) {
        if (from.ops.at(i)->type() != me.ops.at(i)->type())
            return me;
    }

    FilterOperationsWrap blendedFilters;
    for (size_t i = 0; i < shorterSize; i++) {
        blendedFilters.ops.operations().append(blink::FilterOperation::blend(from.ops.at(i), me.ops.at(i), progress));
    }

    if (fromIsLonger) {
        for (size_t i = shorterSize; i < longerSize; i++)
            blendedFilters.ops.operations().append(blink::FilterOperation::blend(from.ops.at(i), NULL, progress));
    } else {
        for (size_t i = shorterSize; i < longerSize; i++)
            blendedFilters.ops.operations().append(blink::FilterOperation::blend(NULL, me.ops.at(i), progress));
    }

    return blendedFilters;
}

}

using namespace blink;

namespace mc {

DEFINE_TRACE(FilterOperationsWrap)
{
    visitor->trace(ops);
}

}