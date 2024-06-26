// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_FILTER_OPERATIONS_H_
#define CC_OUTPUT_FILTER_OPERATIONS_H_

#include <vector>

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "cc/output/filter_operation.h"

namespace base {
namespace trace_event {
    class TracedValue;
}
class Value;
}

namespace cc {

// An ordered list of filter operations.
class CC_EXPORT FilterOperations {
public:
    FilterOperations();

    FilterOperations(const FilterOperations& other);

    ~FilterOperations();

    FilterOperations& operator=(const FilterOperations& other);

    bool operator==(const FilterOperations& other) const;

    bool operator!=(const FilterOperations& other) const
    {
        return !(*this == other);
    }

    void Append(const FilterOperation& filter);

    // Removes all filter operations.
    void Clear();

    bool IsEmpty() const;

    void GetOutsets(int* top, int* right, int* bottom, int* left) const;
    bool HasFilterThatMovesPixels() const;
    bool HasFilterThatAffectsOpacity() const;
    bool HasReferenceFilter() const;

    size_t size() const
    {
        return operations_.size();
    }

    const FilterOperation& at(size_t index) const
    {
        DCHECK_LT(index, size());
        return operations_[index];
    }

    // If |from| is of the same size as this, where in each position, the filter
    // in |from| is of the same type as the filter in this, and if this doesn't
    // contain any reference filters, returns a FilterOperations formed by
    // linearly interpolating at each position a |progress| fraction of the way
    // from the filter in |from| to the filter in this. If |from| and this are of
    // different lengths, they are treated as having the same length by padding
    // the shorter sequence with no-op filters of the same type as the filters in
    // the corresponding positions in the longer sequence. If either sequence has
    // a reference filter or if there is a type mismatch at some position, returns
    // a copy of this.
    FilterOperations Blend(const FilterOperations& from, double progress) const;

    void AsValueInto(base::trace_event::TracedValue* value) const;

private:
    std::vector<FilterOperation> operations_;
};

} // namespace cc

#endif // CC_OUTPUT_FILTER_OPERATIONS_H_
