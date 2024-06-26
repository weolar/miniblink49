
#ifndef gpu_command_buffer_common_command_buffer_h
#define gpu_command_buffer_common_command_buffer_h

#include "gpu/command_buffer/common/command_buffer.h"

#include <string>

namespace content {

class GpuChannelMgr;

class CommandBufferServiceStub {
    CommandBufferServiceStub(GpuChannelMgr* gpuChannelMgr);
    ~CommandBufferServiceStub();

    void onInitialize(const char* sharedStateMem, size_t sharedStateMemSize);

    void onSetGetBuffer(int32 shmId, int* replyMessage);

    int getId() const
    {
        return m_id;
    }

private:
    int m_id;
    GpuChannelMgr* m_gpuChannelMgr;

    scoped_refptr<gpu::SyncPointOrderData> sync_point_order_data_;

    // Outlives the stub.
    gpu::SyncPointManager* sync_point_manager_;

    // Task runner for main thread.
    scoped_refptr<base::SingleThreadTaskRunner> task_runner_;

    // The group of contexts that share namespaces with this context.
    scoped_refptr<gpu::gles2::ContextGroup> context_group_;

    bool initialized_;
    gfx::GLSurfaceHandle handle_;
    gfx::Size initial_size_;
    gpu::gles2::DisallowedFeatures disallowed_features_;
    std::vector<int32> requested_attribs_;
    gfx::GpuPreference gpu_preference_;
    bool use_virtualized_gl_context_;
    const uint64_t command_buffer_id_;
    const int32 stream_id_;
    const int32 route_id_;
    const bool offscreen_;
    uint32 last_flush_count_;

    scoped_ptr<gpu::CommandBufferService> command_buffer_;
    scoped_ptr<gpu::gles2::GLES2Decoder> decoder_;
    scoped_ptr<gpu::GpuScheduler> scheduler_;
    scoped_ptr<gpu::SyncPointClient> sync_point_client_;
    scoped_refptr<gfx::GLSurface> surface_;

    // A queue of sync points associated with this stub.
    std::deque<uint32> sync_points_;
    bool waiting_for_sync_point_;

    base::TimeTicks process_delayed_work_time_;
    uint32_t previous_processed_num_;
    base::TimeTicks last_idle_time_;

    scoped_refptr<gpu::PreemptionFlag> preemption_flag_;

    std::string active_url_;
    size_t active_url_hash_;

    size_t total_gpu_memory_;
    scoped_ptr<WaitForCommandState> wait_for_token_;
    scoped_ptr<WaitForCommandState> wait_for_get_offset_;
};

}

#endif // gpu_command_buffer_common_command_buffer_h