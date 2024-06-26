// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/RenderWidgetCompositor.h"

#include "base/message_loop/message_loop.h"
#include "cc/animation/animation_host.h"
#include "cc/animation/animation_timeline.h"
#include "cc/blink/web_compositor_animation_timeline_impl.h"
#include "cc/blink/web_layer_impl.h"
#include "cc/layers/layer.h"
#include "cc/output/managed_memory_policy.h"
#include "cc/raster/task_graph_runner.h"
#include "cc/surfaces/onscreen_display_client.h"
#include "cc/surfaces/surface_display_output_surface.h"
#include "cc/surfaces/surface_id_allocator.h"
#include "cc/surfaces/surface_manager.h"
#include "content/OrigChromeMgr.h"
#include "content/WebPageOcBridge.h"
#include "content/WebSharedBitmapManager.h"
#include "content/compositor/BrowserContextProvider.h"
#include "content/compositor/EmptyOutputSurface.h"
#include "content/compositor/GpuOutputSurface.h"
#include "content/compositor/SoftwareOutputDevice.h"
#include "content/compositor/SoftwareOutputSurface.h"
#include "content/gpu/ChildGpuMemoryBufferManager.h"
#include "gpu/command_buffer/client/gles2_interface.h"
#include "gpu/command_buffer/common/gpu_memory_allocation.h"
#include "third_party/WebKit/public/web/WebSelection.h"

namespace wke {
extern bool g_headlessEnable;
}

namespace content {

cc::SurfaceManager* g_surfaceManager = nullptr;
static uint32_t s_nextSurfaceIdNamespace = 1u;

RenderWidgetCompositor::RenderWidgetCompositor(WebPageOcBridge* webPageOcBridge, bool isUiThreadIsolate)
{
    m_webPageOcBridge = webPageOcBridge;
    m_softwareOutputDevice = nullptr;
    m_hWnd = nullptr;

    init(isUiThreadIsolate);
}

void RenderWidgetCompositor::init(bool isUiThreadIsolate)
{
    cc::LayerTreeSettings settings;
    settings.layer_transforms_should_scale_layer_contents = true;
    settings.gpu_rasterization_forced = false;
    settings.gpu_rasterization_enabled = false;
    settings.create_low_res_tiling = true;
    settings.can_use_lcd_text = true;
    settings.use_distance_field_text = false;
    settings.use_zero_copy = false;
    settings.accelerated_animation_enabled = true;
    settings.use_compositor_animation_timelines = true;

    cc::LayerTreeHost::InitParams params;

    OrigChromeMgr* mgr = OrigChromeMgr::getInst();

    params.client = this;
    params.shared_bitmap_manager = mgr->getSharedBitmapManager();
    params.gpu_memory_buffer_manager = mgr->getChildGpuMemoryBufferManager();
    params.task_graph_runner = mgr->getTaskGraphRunner();
    params.settings = &settings;
    params.main_task_runner = base::MessageLoop::current()->task_runner();

    //scoped_refptr<base::SingleThreadTaskRunner> implTaskRunner = mgr->getOrCreateCompositorThread()->task_runner();
    scoped_refptr<base::SingleThreadTaskRunner> implTaskRunner = isUiThreadIsolate ? mgr->getUiLoop()->task_runner() : params.main_task_runner;

    m_layerTreeHost = cc::LayerTreeHost::CreateThreaded(implTaskRunner, &params).release();
}

static void destroyOnUiThread(
    cc::SurfaceIdAllocator* idAllocator,
    cc::OutputSurface* outputSurface,
    cc::OnscreenDisplayClient* displayClient)
{
    if (idAllocator)
        delete idAllocator;
    if (outputSurface)
        delete outputSurface;
    if (displayClient)
        delete displayClient;
}

RenderWidgetCompositor::~RenderWidgetCompositor()
{
    m_layerTreeHost->SetVisible(false);

    scoped_ptr<cc::OutputSurface> outputSurface;
    if (!m_layerTreeHost->output_surface_lost())
        outputSurface = m_layerTreeHost->ReleaseOutputSurface();
    delete m_layerTreeHost;

    OrigChromeMgr::getInst()->getUiLoop()->task_runner()->PostTask(FROM_HERE,
        base::Bind(&destroyOnUiThread, m_idAllocator.release(), outputSurface.release(), m_displayClient.release()));
}

void RenderWidgetCompositor::onHostResized(int width, int height)
{
    gfx::Size size(width, height);
    if (m_layerTreeHost)
        m_layerTreeHost->SetViewportSize(size);

    if (m_displayClient)
        m_displayClient->display()->Resize(size);
}

void RenderWidgetCompositor::setHWND(HWND hWnd)
{
    m_hWnd = hWnd;

    if (m_softwareOutputDevice)
        m_softwareOutputDevice->setHWND(hWnd);
}

HDC RenderWidgetCompositor::getHdcLocked()
{
    if (m_softwareOutputDevice)
        return m_softwareOutputDevice->getHdcLocked();
    return nullptr;
}

void RenderWidgetCompositor::releaseHdc()
{
    if (m_softwareOutputDevice)
        m_softwareOutputDevice->releaseHdc();
}

// WebLayerTreeView implementation.
void RenderWidgetCompositor::setRootLayer(const blink::WebLayer& layer)
{
    if (wke::g_headlessEnable)
        return;
    m_layerTreeHost->SetRootLayer(static_cast<const cc_blink::WebLayerImpl*>(&layer)->layer());
}

void RenderWidgetCompositor::clearRootLayer()
{
    m_layerTreeHost->SetRootLayer(scoped_refptr<cc::Layer>());
}

blink::WebSize RenderWidgetCompositor::deviceViewportSize() const
{
    return blink::WebSize(m_layerTreeHost->device_viewport_size());
}

float RenderWidgetCompositor::deviceScaleFactor() const
{
    return m_layerTreeHost->device_scale_factor();
}

void RenderWidgetCompositor::finishAllRendering()
{
    m_layerTreeHost->FinishAllRendering();
}

void RenderWidgetCompositor::attachCompositorAnimationTimeline(blink::WebCompositorAnimationTimeline* compositorTimeline)
{
    DCHECK(compositorTimeline);
    DCHECK(m_layerTreeHost->animation_host());
    m_layerTreeHost->animation_host()->AddAnimationTimeline(static_cast<const cc_blink::WebCompositorAnimationTimelineImpl*>(compositorTimeline)->animation_timeline());
}

void RenderWidgetCompositor::detachCompositorAnimationTimeline(blink::WebCompositorAnimationTimeline* compositorTimeline)
{
    DCHECK(compositorTimeline);
    DCHECK(m_layerTreeHost->animation_host());
    m_layerTreeHost->animation_host()->RemoveAnimationTimeline(static_cast<const cc_blink::WebCompositorAnimationTimelineImpl*>(compositorTimeline)->animation_timeline());
}

void RenderWidgetCompositor::setViewportSize(const blink::WebSize& deviceViewportSize)
{
    m_layerTreeHost->SetViewportSize(deviceViewportSize);
}

blink::WebFloatPoint RenderWidgetCompositor::adjustEventPointForPinchZoom(const blink::WebFloatPoint& point) const
{
    return point;
}

void RenderWidgetCompositor::setDeviceScaleFactor(float deviceScale)
{
    m_layerTreeHost->SetDeviceScaleFactor(deviceScale);
}

void RenderWidgetCompositor::setBackgroundColor(blink::WebColor color)
{
    m_layerTreeHost->set_background_color(color);
}

void RenderWidgetCompositor::setHasTransparentBackground(bool transparent)
{
    m_layerTreeHost->set_has_transparent_background(transparent);
}

void RenderWidgetCompositor::setVisible(bool visible)
{
    m_layerTreeHost->SetVisible(visible);

    if (visible)
        m_layerTreeHost->SetLayerTreeHostClientReady();
}

void RenderWidgetCompositor::setPageScaleFactorAndLimits(float page_scale_factor, float minimum, float maximum)
{
    m_layerTreeHost->SetPageScaleFactorAndLimits(page_scale_factor, minimum, maximum);
}

void RenderWidgetCompositor::startPageScaleAnimation(const blink::WebPoint& destination, bool useAnchor, float newPageScale, double durationSec)
{
    base::TimeDelta duration = base::TimeDelta::FromMicroseconds((int64)(durationSec)*base::Time::kMicrosecondsPerSecond);
    m_layerTreeHost->StartPageScaleAnimation(
        gfx::Vector2d(destination.x, destination.y),
        useAnchor,
        newPageScale,
        duration);
}

void RenderWidgetCompositor::heuristicsForGpuRasterizationUpdated(bool matches_heuristics)
{
    m_layerTreeHost->SetHasGpuRasterizationTrigger(matches_heuristics);
}

void RenderWidgetCompositor::setNeedsAnimate()
{
    m_layerTreeHost->SetNeedsAnimate();
    m_layerTreeHost->SetNeedsUpdateLayers();
}

void RenderWidgetCompositor::setNeedsBeginFrame()
{
    m_layerTreeHost->SetNeedsAnimate();
}

void RenderWidgetCompositor::setNeedsCompositorUpdate()
{
    m_layerTreeHost->SetNeedsUpdateLayers();
}

void RenderWidgetCompositor::didStopFlinging()
{
    m_layerTreeHost->DidStopFlinging();
}

extern bool g_popupMenuIniting;

void RenderWidgetCompositor::registerForAnimations(blink::WebLayer* layer)
{
    if (g_popupMenuIniting)
        return;

    cc::Layer* cc_layer = static_cast<cc_blink::WebLayerImpl*>(layer)->layer();
    cc_layer->RegisterForAnimations(m_layerTreeHost->animation_registrar());
}

void RenderWidgetCompositor::registerViewportLayers(
    const blink::WebLayer* overscrollElasticityLayer,
    const blink::WebLayer* pageScaleLayer,
    const blink::WebLayer* innerViewportScrollLayer,
    const blink::WebLayer* outerViewportScrollLayer)
{
    m_layerTreeHost->RegisterViewportLayers(
        // TODO(bokan): This check can probably be removed now, but it looks
        // like overscroll elasticity may still be NULL until PinchViewport
        // registers its layers.
        // The scroll elasticity layer will only exist when using pinch virtual
        // viewports.
        overscrollElasticityLayer ? static_cast<const cc_blink::WebLayerImpl*>(overscrollElasticityLayer)->layer() : NULL,
        static_cast<const cc_blink::WebLayerImpl*>(pageScaleLayer)->layer(), static_cast<const cc_blink::WebLayerImpl*>(innerViewportScrollLayer)->layer(),
        // TODO(bokan): This check can probably be removed now, but it looks
        // like overscroll elasticity may still be NULL until PinchViewport
        // registers its layers.
        // The outer viewport layer will only exist when using pinch virtual
        // viewports.
        outerViewportScrollLayer ? static_cast<const cc_blink::WebLayerImpl*>(outerViewportScrollLayer)->layer() : NULL);
}

void RenderWidgetCompositor::clearViewportLayers()
{
    m_layerTreeHost->RegisterViewportLayers(
        scoped_refptr<cc::Layer>(), scoped_refptr<cc::Layer>(),
        scoped_refptr<cc::Layer>(), scoped_refptr<cc::Layer>());
}

cc::LayerSelectionBound ConvertWebSelectionBound(const blink::WebSelection& web_selection, bool is_start)
{
    cc::LayerSelectionBound cc_bound;
    if (web_selection.isNone())
        return cc_bound;

    const blink::WebSelectionBound& web_bound = is_start ? web_selection.start() : web_selection.end();
    DCHECK(web_bound.layerId);
    cc_bound.type = cc::SELECTION_BOUND_CENTER;
    if (web_selection.isRange()) {
        if (is_start) {
            cc_bound.type = web_bound.isTextDirectionRTL ? cc::SELECTION_BOUND_RIGHT : cc::SELECTION_BOUND_LEFT;
        } else {
            cc_bound.type = web_bound.isTextDirectionRTL ? cc::SELECTION_BOUND_LEFT : cc::SELECTION_BOUND_RIGHT;
        }
    }
    cc_bound.layer_id = web_bound.layerId;
    cc_bound.edge_top = gfx::PointF(web_bound.edgeTopInLayer);
    cc_bound.edge_bottom = gfx::PointF(web_bound.edgeBottomInLayer);
    return cc_bound;
}

cc::LayerSelection ConvertWebSelection(const blink::WebSelection& web_selection)
{
    cc::LayerSelection cc_selection;
    cc_selection.start = ConvertWebSelectionBound(web_selection, true);
    cc_selection.end = ConvertWebSelectionBound(web_selection, false);
    cc_selection.is_editable = web_selection.isEditable();
    cc_selection.is_empty_text_form_control = web_selection.isEmptyTextFormControl();
    return cc_selection;
}

void RenderWidgetCompositor::registerSelection(const blink::WebSelection& selection)
{
    m_layerTreeHost->RegisterSelection(ConvertWebSelection(selection));
}

void RenderWidgetCompositor::clearSelection()
{
    cc::LayerSelection empty_selection;
    m_layerTreeHost->RegisterSelection(empty_selection);
}

int RenderWidgetCompositor::layerTreeId() const
{
    return m_layerTreeHost->id();
}

void RenderWidgetCompositor::layoutAndPaintAsync(blink::WebLayoutAndPaintAsyncCallback* callback)
{
    DebugBreak();
}

void RenderWidgetCompositor::compositeAndReadbackAsync(blink::WebCompositeAndReadbackAsyncCallback* callback)
{
    DebugBreak();
}

void RenderWidgetCompositor::setDeferCommits(bool defer_commits)
{
    m_layerTreeHost->SetDeferCommits(defer_commits);
}
//////////////////////////////////////////////////////////////////////////

static scoped_ptr<gpu_blink::WebGraphicsContext3DInProcessCommandBufferImpl> createContextCommon(HWND window)
{
    blink::WebGraphicsContext3D::Attributes attrs;
    attrs.shareResources = true;
    attrs.depth = false;
    attrs.stencil = false;
    attrs.antialias = false;
    attrs.noAutomaticFlushes = true;
    bool lose_context_when_out_of_memory = true;
    std::string url("chrome://gpu/GpuProcessTransportFactory::CreateContextCommon");

    scoped_ptr<gpu_blink::WebGraphicsContext3DInProcessCommandBufferImpl> context;
    if (window)
        context = gpu_blink::WebGraphicsContext3DInProcessCommandBufferImpl::CreateViewContext(attrs, true, window);
    else
        context = gpu_blink::WebGraphicsContext3DInProcessCommandBufferImpl::CreateOffscreenContext(attrs, true);
    return context.Pass();
}

static scoped_ptr<cc::SurfaceIdAllocator> genSurfaceIdAllocator()
{
    scoped_ptr<cc::SurfaceIdAllocator> allocator = make_scoped_ptr(new cc::SurfaceIdAllocator(s_nextSurfaceIdNamespace++));
    if (g_surfaceManager)
        allocator->RegisterSurfaceIdNamespace(g_surfaceManager);
    return allocator;
}

void RenderWidgetCompositor::WillBeginMainFrame()
{
}

void RenderWidgetCompositor::BeginMainFrame(const cc::BeginFrameArgs& args)
{
    m_webPageOcBridge->onBeginMainFrame();
}

void RenderWidgetCompositor::BeginMainFrameNotExpectedSoon()
{
}

void RenderWidgetCompositor::DidBeginMainFrame()
{
}

void RenderWidgetCompositor::Layout()
{
    m_webPageOcBridge->onLayout();
}

void RenderWidgetCompositor::DidInitializeOutputSurface()
{
}

void RenderWidgetCompositor::DidFailToInitializeOutputSurface()
{
}

void RenderWidgetCompositor::WillCommit()
{
}

void RenderWidgetCompositor::DidCommit()
{
}

void RenderWidgetCompositor::DidCommitAndDrawFrame()
{
}

void RenderWidgetCompositor::DidCompleteSwapBuffers()
{
}

void RenderWidgetCompositor::RecordFrameTimingEvents(
    scoped_ptr<cc::FrameTimingTracker::CompositeTimingSet> compositeEvents,
    scoped_ptr<cc::FrameTimingTracker::MainFrameTimingSet> mainFrameEvents)
{
    //     const base::hash_map<int64_t, std::vector<CompositeTimingEvent>>::iterator& compositeEvent = compositeEvents.begin();
    //     for (; compositeEvent != compositeEvents.end();++compositeEventcompositeEvent) {
    //         int64_t frameId = composite_event.first;
    //         const std::vector<cc::FrameTimingTracker::CompositeTimingEvent>& events =
    //             compositeEvent.second;
    //         std::vector<blink::WebFrameTimingEvent> webEvents;
    //         for (size_t i = 0; i < events.size(); ++i) {
    //             webEvents.push_back(blink::WebFrameTimingEvent(
    //                 events[i].frame_id,
    //                 (events[i].timestamp - base::TimeTicks()).InSecondsF()));
    //         }
    //         widget_->webwidget()->recordFrameTimingEvent(
    //             blink::WebWidget::CompositeEvent, frameId, webEvents);
    //     }
    //
    //     const base::hash_map<int64_t, std::vector<MainFrameTimingEvent>>& mainFrameEvent = mainFrameEvents.begin();
    //     for (; mainFrameEvent != mainFrameEvents.end(); ++mainFrameEvent) {
    //         int64_t frameId = mainFrameEvent.first;
    //         const std::vector<cc::FrameTimingTracker::MainFrameTimingEvent>& events =
    //             mainFrameEvent.second;
    //         std::vector<blink::WebFrameTimingEvent> webEvents;
    //         for (size_t i = 0; i < events.size(); ++i) {
    //             webEvents.push_back(blink::WebFrameTimingEvent(
    //                 events[i].frame_id,
    //                 (events[i].timestamp - base::TimeTicks()).InSecondsF(),
    //                 (events[i].end_time - base::TimeTicks()).InSecondsF()));
    //         }
    //         widget_->webwidget()->recordFrameTimingEvent(
    //             blink::WebWidget::RenderEvent, frameId, webEvents);
    //     }
}

//////////////////////////////////////////////////////////////////////////

void RenderWidgetCompositor::initializeLayerTreeView()
{
    //     SIZE size = m_renderViewHost->windowSize();
    //     m_layerTreeHost->SetViewportSize(gfx::Size(size.cx, size.cy));
}

void RenderWidgetCompositor::firePaintEvent(HDC hdc, const RECT& paintRect)
{
    if (m_softwareOutputDevice)
        m_softwareOutputDevice->firePaintEvent(hdc, paintRect);
    else {
        //         gfx::Rect damageRect(paintRect);
        //         m_layerTreeHost->SetNeedsRedrawRect(damageRect);
        //         m_layerTreeHost->SetNeedsCommit();
    }
}

void RenderWidgetCompositor::ReturnResources(const cc::ReturnedResourceArray& resources)
{
    ;
}

// cc::LayerTreeHostClient
void RenderWidgetCompositor::RequestNewOutputSurface()
{
    if (kGLImplTypeNone == OrigChromeMgr::getInst()->getGlImplType()) {
        scoped_ptr<SoftwareOutputSurface> outputSurface = SoftwareOutputSurface::Create(m_webPageOcBridge);
        m_softwareOutputDevice = (SoftwareOutputDevice*)outputSurface->software_device();
        m_layerTreeHost->SetOutputSurface(std::move(outputSurface));

        if (m_hWnd)
            m_softwareOutputDevice->setHWND(m_hWnd);
        return;
    }

    scoped_refptr<ContextProviderCommandBuffer> contextProvider;

    bool sharedWorkerContextProviderLost = false; // Try to reuse existing worker context provider.
    if (m_shareWorkerContextProvider) {
        base::AutoLock lock(*m_shareWorkerContextProvider->GetLock()); // Note: If context is lost, we delete reference after releasing the lock.
        if (m_shareWorkerContextProvider->ContextGL()->GetGraphicsResetStatusKHR() != GL_NO_ERROR)
            sharedWorkerContextProviderLost = true;
    }

    contextProvider = ContextProviderCommandBuffer::Create(createContextCommon(m_hWnd), BROWSER_COMPOSITOR_ONSCREEN_CONTEXT);
    if (!m_shareWorkerContextProvider || sharedWorkerContextProviderLost) {
        m_shareWorkerContextProvider = ContextProviderCommandBuffer::Create(createContextCommon(nullptr), BROWSER_WORKER_CONTEXT);

        if (m_shareWorkerContextProvider && !m_shareWorkerContextProvider->BindToCurrentThread())
            m_shareWorkerContextProvider = nullptr;

        if (m_shareWorkerContextProvider)
            m_shareWorkerContextProvider->SetupLock();
    }

    bool createdGpuBrowserCompositor = !!contextProvider && !!m_shareWorkerContextProvider;

    if (!createdGpuBrowserCompositor) {
        // Try again. TODO
        OutputDebugStringA("createdGpuBrowserCompositor is fail\n");
        return;
    }

    OrigChromeMgr* mgr = OrigChromeMgr::getInst();
    WebSharedBitmapManager* sharedBitmapManager = mgr->getSharedBitmapManager();
    ;
    ChildGpuMemoryBufferManager* childGpuMemoryBufferManager = mgr->getChildGpuMemoryBufferManager();

    cc::RendererSettings rendererSettings = m_layerTreeHost->settings().renderer_settings;

    if (!g_surfaceManager)
        g_surfaceManager = new cc::SurfaceManager();

    m_idAllocator = genSurfaceIdAllocator();

    scoped_ptr<EmptyOutputSurface> outputSurfaceStub;
    outputSurfaceStub = make_scoped_ptr(new EmptyOutputSurface(contextProvider, m_shareWorkerContextProvider, sharedBitmapManager, childGpuMemoryBufferManager, rendererSettings));

    scoped_refptr<base::SingleThreadTaskRunner> task_runner = OrigChromeMgr::getInst()->getUiLoop()->task_runner();
    scoped_ptr<cc::SurfaceDisplayOutputSurface> outputSurface(new cc::SurfaceDisplayOutputSurface(g_surfaceManager, m_idAllocator.get(), contextProvider, m_shareWorkerContextProvider));

    m_displayClient = make_scoped_ptr(new cc::OnscreenDisplayClient(
        outputSurfaceStub.Pass(), g_surfaceManager,
        sharedBitmapManager, childGpuMemoryBufferManager,
        rendererSettings, task_runner));

    m_displayClient->set_surface_output_surface(outputSurface.get());
    outputSurface->set_display_client(m_displayClient.get());

    m_layerTreeHost->SetOutputSurface(outputSurface.Pass());

    gfx::Size size = m_layerTreeHost->device_viewport_size();
    m_displayClient->display()->Resize(size);
}

} // content