// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_SERIAL_RUNNER_H_
#define MEDIA_BASE_SERIAL_RUNNER_H_

#include <queue>

#include "base/callback.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "media/base/media_export.h"
#include "media/base/pipeline_status.h"

namespace base {
class SingleThreadTaskRunner;
}

namespace media {

// Runs a series of bound functions accepting Closures or PipelineStatusCB.
// SerialRunner doesn't use regular Closure/PipelineStatusCBs as it late binds
// the completion callback as the series progresses.
class MEDIA_EXPORT SerialRunner {
public:
    typedef base::Callback<void(const base::Closure&)> BoundClosure;
    typedef base::Callback<void(const PipelineStatusCB&)> BoundPipelineStatusCB;

    // Serial queue of bound functions to run.
    class MEDIA_EXPORT Queue {
    public:
        Queue();
        ~Queue();

        void Push(const base::Closure& closure);
        void Push(const BoundClosure& bound_fn);
        void Push(const BoundPipelineStatusCB& bound_fn);

    private:
        friend class SerialRunner;

        BoundPipelineStatusCB Pop();
        bool empty();

        std::queue<BoundPipelineStatusCB> bound_fns_;
    };

    // Executes the bound functions in series, executing |done_cb| when finished.
    //
    // All bound functions are executed on the thread that Run() is called on,
    // including |done_cb|.
    //
    // To eliminate an unnecessary posted task, the first function is executed
    // immediately on the caller's stack. It is *strongly advised* to ensure
    // the calling code does no more work after the call to Run().
    //
    // In all cases, |done_cb| is guaranteed to execute on a separate calling
    // stack.
    //
    // Deleting the object will prevent execution of any unstarted bound
    // functions, including |done_cb|.
    static scoped_ptr<SerialRunner> Run(
        const Queue& bound_fns, const PipelineStatusCB& done_cb);

private:
    friend struct base::DefaultDeleter<SerialRunner>;

    SerialRunner(const Queue& bound_fns, const PipelineStatusCB& done_cb);
    ~SerialRunner();

    void RunNextInSeries(PipelineStatus last_status);

    scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
    Queue bound_fns_;
    PipelineStatusCB done_cb_;

    // NOTE: Weak pointers must be invalidated before all other member variables.
    base::WeakPtrFactory<SerialRunner> weak_factory_;

    DISALLOW_COPY_AND_ASSIGN(SerialRunner);
};

} // namespace media

#endif // MEDIA_BASE_SERIAL_RUNNER_H_
