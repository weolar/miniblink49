/*
 * Copyright (C) 2006, 2007, 2009, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2008, 2010 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2012, Samsung Electronics. All rights reserved.
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
 */

#include "config.h"
#include "core/page/ChromeClient.h"

#include "core/dom/Document.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLInputElement.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/layout/HitTestResult.h"
#include "core/page/FrameTree.h"
#include "core/page/ScopedPageLoadDeferrer.h"
#include "core/page/WindowFeatures.h"
#include "platform/geometry/IntRect.h"
#include "platform/network/NetworkHints.h"
#include "public/platform/WebScreenInfo.h"
#include <algorithm>

namespace blink {

void ChromeClient::setWindowRectWithAdjustment(const IntRect& pendingRect)
{
    IntRect screen = screenInfo().availableRect;
    IntRect window = pendingRect;

    IntSize minimumSize = minimumWindowSize();
    // Let size 0 pass through, since that indicates default size, not minimum
    // size.
    if (window.width())
        window.setWidth(std::min(std::max(minimumSize.width(), window.width()), screen.width()));
    if (window.height())
        window.setHeight(std::min(std::max(minimumSize.height(), window.height()), screen.height()));

    // Constrain the window position within the valid screen area.
    window.setX(std::max(screen.x(), std::min(window.x(), screen.maxX() - window.width())));
    window.setY(std::max(screen.y(), std::min(window.y(), screen.maxY() - window.height())));
    setWindowRect(window);
}

bool ChromeClient::canOpenModalIfDuringPageDismissal(Frame* mainFrame, ChromeClient::DialogType dialog, const String& message)
{
    for (Frame* frame = mainFrame; frame; frame = frame->tree().traverseNext()) {
        if (!frame->isLocalFrame())
            continue;
        Document::PageDismissalType dismissal = toLocalFrame(frame)->document()->pageDismissalEventBeingDispatched();
        if (dismissal != Document::NoDismissal)
            return shouldOpenModalDialogDuringPageDismissal(dialog, message, dismissal);
    }
    return true;
}

void ChromeClient::setWindowFeatures(const WindowFeatures& features)
{
    setToolbarsVisible(features.toolBarVisible || features.locationBarVisible);
    setStatusbarVisible(features.statusBarVisible);
    setScrollbarsVisible(features.scrollbarsVisible);
    setMenubarVisible(features.menuBarVisible);
    setResizable(features.resizable);
}

template<typename... Params>
bool openJavaScriptDialog(
    ChromeClient* chromeClient,
    bool(ChromeClient::*function)(LocalFrame*, const String& message, Params&...),
    LocalFrame& frame,
    const String& message,
    ChromeClient::DialogType dialogType,
    Params&... parameters)
{
    // Defer loads in case the client method runs a new event loop that would
    // otherwise cause the load to continue while we're in the middle of
    // executing JavaScript.
    ScopedPageLoadDeferrer deferrer;

    InspectorInstrumentationCookie cookie = InspectorInstrumentation::willRunJavaScriptDialog(&frame, message, dialogType);
    bool result = (chromeClient->*function)(&frame, message, parameters...);
    InspectorInstrumentation::didRunJavaScriptDialog(cookie, result);
    return result;
}

bool ChromeClient::openBeforeUnloadConfirmPanel(const String& message, LocalFrame* frame)
{
    ASSERT(frame);
    return openJavaScriptDialog(this, &ChromeClient::openBeforeUnloadConfirmPanelDelegate, *frame, message, ChromeClient::HTMLDialog);
}

bool ChromeClient::openJavaScriptAlert(LocalFrame* frame, const String& message)
{
    ASSERT(frame);
    if (!canOpenModalIfDuringPageDismissal(frame->tree().top(), ChromeClient::AlertDialog, message))
        return false;
    return openJavaScriptDialog(this, &ChromeClient::openJavaScriptAlertDelegate, *frame, message, ChromeClient::AlertDialog);
}

bool ChromeClient::openJavaScriptConfirm(LocalFrame* frame, const String& message)
{
    ASSERT(frame);
    if (!canOpenModalIfDuringPageDismissal(frame->tree().top(), ChromeClient::ConfirmDialog, message))
        return false;
    return openJavaScriptDialog(this, &ChromeClient::openJavaScriptConfirmDelegate, *frame, message, ChromeClient::ConfirmDialog);
}

bool ChromeClient::openJavaScriptPrompt(LocalFrame* frame, const String& prompt, const String& defaultValue, String& result)
{
    ASSERT(frame);
    if (!canOpenModalIfDuringPageDismissal(frame->tree().top(), ChromeClient::PromptDialog, prompt))
        return false;
    return openJavaScriptDialog(this, &ChromeClient::openJavaScriptPromptDelegate, *frame, prompt, ChromeClient::PromptDialog, defaultValue, result);
}

void ChromeClient::mouseDidMoveOverElement(const HitTestResult& result)
{
    if (result.innerNode() && result.innerNode()->document().isDNSPrefetchEnabled())
        prefetchDNS(result.absoluteLinkURL().host());

    showMouseOverURL(result);

    setToolTip(result);
}

void ChromeClient::setToolTip(const HitTestResult& result)
{
    // First priority is a potential toolTip representing a spelling or grammar
    // error.
    TextDirection toolTipDirection;
    String toolTip = result.spellingToolTip(toolTipDirection);

    // Next we'll consider a tooltip for element with "title" attribute.
    if (toolTip.isEmpty())
        toolTip = result.title(toolTipDirection);

    // Lastly, for <input type="file"> that allow multiple files, we'll consider
    // a tooltip for the selected filenames.
    if (toolTip.isEmpty()) {
        if (Node* node = result.innerNode()) {
            if (isHTMLInputElement(*node)) {
                HTMLInputElement* input = toHTMLInputElement(node);
                toolTip = input->defaultToolTip();

                // FIXME: We should obtain text direction of tooltip from
                // ChromeClient or platform. As of October 2011, all client
                // implementations don't use text direction information for
                // ChromeClient::setToolTip. We'll work on tooltip text
                // direction during bidi cleanup in form inputs.
                toolTipDirection = LTR;
            }
        }
    }

    setToolTip(toolTip, toolTipDirection);
}

void ChromeClient::print(LocalFrame* frame)
{
    // Defer loads in case the client method runs a new event loop that would
    // otherwise cause the load to continue while we're in the middle of
    // executing JavaScript.
    ScopedPageLoadDeferrer deferrer;

    printDelegate(frame);
}

} // namespace blink
