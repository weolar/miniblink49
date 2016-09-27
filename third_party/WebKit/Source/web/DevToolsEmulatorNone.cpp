// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "web/DevToolsEmulator.h"

#include "core/frame/FrameView.h"
#include "core/frame/Settings.h"
#include "core/page/Page.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "public/web/WebDeviceEmulationParams.h"
#include "web/InspectorEmulationAgent.h"
#include "web/WebInputEventConversion.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebSettingsImpl.h"
#include "web/WebViewImpl.h"

namespace {

    static float calculateDeviceScaleAdjustment(int width, int height, float deviceScaleFactor)
    {
        // Chromium on Android uses a device scale adjustment for fonts used in text autosizing for
        // improved legibility. This function computes this adjusted value for text autosizing.
        // For a description of the Android device scale adjustment algorithm, see:
        // chrome/browser/chrome_content_browser_client.cc, GetDeviceScaleAdjustment(...)
        if (!width || !height || !deviceScaleFactor)
            return 1;

        static const float kMinFSM = 1.05f;
        static const int kWidthForMinFSM = 320;
        static const float kMaxFSM = 1.3f;
        static const int kWidthForMaxFSM = 800;

        float minWidth = std::min(width, height) / deviceScaleFactor;
        if (minWidth <= kWidthForMinFSM)
            return kMinFSM;
        if (minWidth >= kWidthForMaxFSM)
            return kMaxFSM;

        // The font scale multiplier varies linearly between kMinFSM and kMaxFSM.
        float ratio = static_cast<float>(minWidth - kWidthForMinFSM) / (kWidthForMaxFSM - kWidthForMinFSM);
        return ratio * (kMaxFSM - kMinFSM) + kMinFSM;
    }

} // namespace

namespace blink {

    DevToolsEmulator::DevToolsEmulator(WebViewImpl* webViewImpl)
        : m_webViewImpl(webViewImpl)
        , m_emulationAgent(nullptr)
        , m_deviceMetricsEnabled(false)
        , m_emulateMobileEnabled(false)
        , m_isOverlayScrollbarsEnabled(false)
        , m_originalDefaultMinimumPageScaleFactor(0)
        , m_originalDefaultMaximumPageScaleFactor(0)
        , m_embedderTextAutosizingEnabled(webViewImpl->page()->settings().textAutosizingEnabled())
        , m_embedderDeviceScaleAdjustment(webViewImpl->page()->settings().deviceScaleAdjustment())
        , m_embedderPreferCompositingToLCDTextEnabled(webViewImpl->page()->settings().preferCompositingToLCDTextEnabled())
        , m_embedderUseMobileViewport(webViewImpl->page()->settings().useMobileViewportStyle())
        , m_embedderPluginsEnabled(webViewImpl->page()->settings().pluginsEnabled())
        , m_touchEventEmulationEnabled(false)
        , m_doubleTapToZoomEnabled(false)
        , m_originalTouchEnabled(false)
        , m_originalDeviceSupportsMouse(false)
        , m_originalDeviceSupportsTouch(false)
        , m_originalMaxTouchPoints(0)
        , m_embedderScriptEnabled(webViewImpl->page()->settings().scriptEnabled())
        , m_scriptExecutionDisabled(false)
    {
    }

    DevToolsEmulator::~DevToolsEmulator()
    {
    }

    void DevToolsEmulator::setEmulationAgent(InspectorEmulationAgent* agent)
    {
        m_emulationAgent = agent;
    }

    void DevToolsEmulator::viewportChanged()
    {
        if (m_emulationAgent)
            m_emulationAgent->viewportChanged();
    }

    void DevToolsEmulator::setTextAutosizingEnabled(bool enabled)
    {
        m_embedderTextAutosizingEnabled = enabled;
        bool emulateMobileEnabled = m_deviceMetricsEnabled && m_emulateMobileEnabled;
        if (!emulateMobileEnabled)
            m_webViewImpl->page()->settings().setTextAutosizingEnabled(enabled);
    }

    void DevToolsEmulator::setDeviceScaleAdjustment(float deviceScaleAdjustment)
    {
        m_embedderDeviceScaleAdjustment = deviceScaleAdjustment;
        bool emulateMobileEnabled = m_deviceMetricsEnabled && m_emulateMobileEnabled;
        if (!emulateMobileEnabled)
            m_webViewImpl->page()->settings().setDeviceScaleAdjustment(deviceScaleAdjustment);
    }

    void DevToolsEmulator::setPreferCompositingToLCDTextEnabled(bool enabled)
    {
        m_embedderPreferCompositingToLCDTextEnabled = enabled;
        bool emulateMobileEnabled = m_deviceMetricsEnabled && m_emulateMobileEnabled;
        if (!emulateMobileEnabled)
            m_webViewImpl->page()->settings().setPreferCompositingToLCDTextEnabled(enabled);
    }

    void DevToolsEmulator::setUseMobileViewportStyle(bool enabled)
    {
        m_embedderUseMobileViewport = enabled;
        bool emulateMobileEnabled = m_deviceMetricsEnabled && m_emulateMobileEnabled;
        if (!emulateMobileEnabled)
            m_webViewImpl->page()->settings().setUseMobileViewportStyle(enabled);
    }

    void DevToolsEmulator::setPluginsEnabled(bool enabled)
    {
        m_embedderPluginsEnabled = enabled;
        bool emulateMobileEnabled = m_deviceMetricsEnabled && m_emulateMobileEnabled;
        if (!emulateMobileEnabled)
            m_webViewImpl->page()->settings().setPluginsEnabled(enabled);
    }

    void DevToolsEmulator::setScriptEnabled(bool enabled)
    {
        m_embedderScriptEnabled = enabled;
        if (!m_scriptExecutionDisabled)
            m_webViewImpl->page()->settings().setScriptEnabled(enabled);
    }

    void DevToolsEmulator::setDoubleTapToZoomEnabled(bool enabled)
    {
        m_doubleTapToZoomEnabled = enabled;
    }

    bool DevToolsEmulator::doubleTapToZoomEnabled() const
    {
        return m_touchEventEmulationEnabled ? true : m_doubleTapToZoomEnabled;
    }

    void DevToolsEmulator::enableDeviceEmulation(const WebDeviceEmulationParams& params)
    {
        if (!m_deviceMetricsEnabled) {
            m_deviceMetricsEnabled = true;
            m_webViewImpl->setBackgroundColorOverride(Color::darkGray);
            m_webViewImpl->updateShowFPSCounterAndContinuousPainting();
        }

        m_webViewImpl->page()->settings().setDeviceScaleAdjustment(calculateDeviceScaleAdjustment(params.viewSize.width, params.viewSize.height, params.deviceScaleFactor));

        if (params.screenPosition == WebDeviceEmulationParams::Mobile)
            enableMobileEmulation();
        else
            disableMobileEmulation();

        m_webViewImpl->setCompositorDeviceScaleFactorOverride(params.deviceScaleFactor);
        m_webViewImpl->setRootLayerTransform(WebSize(params.offset.x, params.offset.y), params.scale);
        if (Document* document = m_webViewImpl->mainFrameImpl()->frame()->document()) {
            document->styleResolverChanged();
            document->mediaQueryAffectingValueChanged();
        }
    }

    void DevToolsEmulator::disableDeviceEmulation()
    {
        if (!m_deviceMetricsEnabled)
            return;

        m_deviceMetricsEnabled = false;
        m_webViewImpl->setBackgroundColorOverride(Color::transparent);
        m_webViewImpl->updateShowFPSCounterAndContinuousPainting();
        m_webViewImpl->page()->settings().setDeviceScaleAdjustment(m_embedderDeviceScaleAdjustment);
        disableMobileEmulation();
        m_webViewImpl->setCompositorDeviceScaleFactorOverride(0.f);
        m_webViewImpl->setRootLayerTransform(WebSize(0.f, 0.f), 1.f);
        m_webViewImpl->setPageScaleFactor(1.f);
        if (Document* document = m_webViewImpl->mainFrameImpl()->frame()->document()) {
            document->styleResolverChanged();
            document->mediaQueryAffectingValueChanged();
        }
    }

    void DevToolsEmulator::enableMobileEmulation()
    {
        if (m_emulateMobileEnabled)
            return;
        m_emulateMobileEnabled = true;
        m_isOverlayScrollbarsEnabled = RuntimeEnabledFeatures::overlayScrollbarsEnabled();
        RuntimeEnabledFeatures::setOverlayScrollbarsEnabled(true);
        m_webViewImpl->enableViewport();
        m_webViewImpl->settings()->setViewportMetaEnabled(true);
        m_webViewImpl->settings()->setShrinksViewportContentToFit(true);
        m_webViewImpl->page()->settings().setTextAutosizingEnabled(true);
        m_webViewImpl->page()->settings().setPreferCompositingToLCDTextEnabled(true);
        m_webViewImpl->page()->settings().setUseMobileViewportStyle(true);
        m_webViewImpl->page()->settings().setPluginsEnabled(false);
        m_webViewImpl->setZoomFactorOverride(1);

        m_originalDefaultMinimumPageScaleFactor = m_webViewImpl->defaultMinimumPageScaleFactor();
        m_originalDefaultMaximumPageScaleFactor = m_webViewImpl->defaultMaximumPageScaleFactor();
        m_webViewImpl->setDefaultPageScaleLimits(0.25f, 5);
    }

    void DevToolsEmulator::disableMobileEmulation()
    {
        if (!m_emulateMobileEnabled)
            return;
        RuntimeEnabledFeatures::setOverlayScrollbarsEnabled(m_isOverlayScrollbarsEnabled);
        m_webViewImpl->disableViewport();
        m_webViewImpl->settings()->setViewportMetaEnabled(false);
        m_webViewImpl->settings()->setShrinksViewportContentToFit(false);
        m_webViewImpl->page()->settings().setTextAutosizingEnabled(m_embedderTextAutosizingEnabled);
        m_webViewImpl->page()->settings().setPreferCompositingToLCDTextEnabled(m_embedderPreferCompositingToLCDTextEnabled);
        m_webViewImpl->page()->settings().setUseMobileViewportStyle(m_embedderUseMobileViewport);
        m_webViewImpl->page()->settings().setPluginsEnabled(m_embedderPluginsEnabled);
        m_webViewImpl->setZoomFactorOverride(0);
        m_emulateMobileEnabled = false;
        m_webViewImpl->setDefaultPageScaleLimits(
            m_originalDefaultMinimumPageScaleFactor,
            m_originalDefaultMaximumPageScaleFactor);
    }

    void DevToolsEmulator::setTouchEventEmulationEnabled(bool enabled)
    {
        if (m_touchEventEmulationEnabled == enabled)
            return;
        if (!m_touchEventEmulationEnabled) {
            m_originalTouchEnabled = RuntimeEnabledFeatures::touchEnabled();
            m_originalDeviceSupportsMouse = m_webViewImpl->page()->settings().deviceSupportsMouse();
            m_originalDeviceSupportsTouch = m_webViewImpl->page()->settings().deviceSupportsTouch();
            m_originalMaxTouchPoints = m_webViewImpl->page()->settings().maxTouchPoints();
        }
        RuntimeEnabledFeatures::setTouchEnabled(enabled ? true : m_originalTouchEnabled);
        if (!m_originalDeviceSupportsTouch) {
            m_webViewImpl->page()->settings().setDeviceSupportsMouse(enabled ? false : m_originalDeviceSupportsMouse);
            m_webViewImpl->page()->settings().setDeviceSupportsTouch(enabled ? true : m_originalDeviceSupportsTouch);
            // Currently emulation does not provide multiple touch points.
            m_webViewImpl->page()->settings().setMaxTouchPoints(enabled ? 1 : m_originalMaxTouchPoints);
        }
        m_touchEventEmulationEnabled = enabled;
        m_webViewImpl->mainFrameImpl()->frameView()->layout();
    }

    void DevToolsEmulator::setScriptExecutionDisabled(bool scriptExecutionDisabled)
    {
        m_scriptExecutionDisabled = scriptExecutionDisabled;
        m_webViewImpl->page()->settings().setScriptEnabled(m_scriptExecutionDisabled ? false : m_embedderScriptEnabled);
    }

    bool DevToolsEmulator::handleInputEvent(const WebInputEvent& inputEvent)
    {
        Page* page = m_webViewImpl->page();
        if (!page)
            return false;

        // FIXME: This workaround is required for touch emulation on Mac, where
        // compositor-side pinch handling is not enabled. See http://crbug.com/138003.
        bool isPinch = inputEvent.type == WebInputEvent::GesturePinchBegin || inputEvent.type == WebInputEvent::GesturePinchUpdate || inputEvent.type == WebInputEvent::GesturePinchEnd;
        if (isPinch && m_touchEventEmulationEnabled) {
            FrameView* frameView = page->deprecatedLocalMainFrame()->view();
            PlatformGestureEventBuilder gestureEvent(frameView, static_cast<const WebGestureEvent&>(inputEvent));
            float pageScaleFactor = page->pageScaleFactor();
            if (gestureEvent.type() == PlatformEvent::GesturePinchBegin) {
                m_lastPinchAnchorCss = adoptPtr(new IntPoint(frameView->scrollPosition() + gestureEvent.position()));
                m_lastPinchAnchorDip = adoptPtr(new IntPoint(gestureEvent.position()));
                m_lastPinchAnchorDip->scale(pageScaleFactor, pageScaleFactor);
            }
            if (gestureEvent.type() == PlatformEvent::GesturePinchUpdate && m_lastPinchAnchorCss) {
                float newPageScaleFactor = pageScaleFactor * gestureEvent.scale();
                IntPoint anchorCss(*m_lastPinchAnchorDip.get());
                anchorCss.scale(1.f / newPageScaleFactor, 1.f / newPageScaleFactor);
                m_webViewImpl->setPageScaleFactor(newPageScaleFactor);
                m_webViewImpl->mainFrame()->setScrollOffset(toIntSize(*m_lastPinchAnchorCss.get() - toIntSize(anchorCss)));
            }
            if (gestureEvent.type() == PlatformEvent::GesturePinchEnd) {
                m_lastPinchAnchorCss.clear();
                m_lastPinchAnchorDip.clear();
            }
            return true;
        }

        return false;
    }

} // namespace blink
