// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/renderer/plugins/cef_plugin_placeholder.h"

#include "libcef/common/cef_messages.h"
#include "libcef/renderer/content_renderer_client.h"
#include "libcef/renderer/plugins/plugin_preroller.h"

#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "chrome/grit/generated_resources.h"
#include "chrome/renderer/custom_menu_commands.h"
#include "components/content_settings/content/common/content_settings_messages.h"
#include "content/public/common/context_menu_params.h"
#include "content/public/renderer/render_frame.h"
#include "content/public/renderer/render_thread.h"
#include "gin/object_template_builder.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebInputEvent.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebScriptSource.h"
#include "third_party/WebKit/public/web/WebView.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/webui/jstemplate_builder.h"
#include "ui/gfx/geometry/size.h"
#include "url/url_util.h"

using base::UserMetricsAction;
using content::RenderThread;
using content::RenderView;

namespace {
const CefPluginPlaceholder* g_last_active_menu = NULL;
}  // namespace

gin::WrapperInfo CefPluginPlaceholder::kWrapperInfo = {
    gin::kEmbedderNativeGin};

CefPluginPlaceholder::CefPluginPlaceholder(
    content::RenderFrame* render_frame,
    blink::WebLocalFrame* frame,
    const blink::WebPluginParams& params,
    const std::string& html_data,
    const base::string16& title)
    : plugins::LoadablePluginPlaceholder(render_frame,
                                         frame,
                                         params,
                                         html_data),
      status_(CefViewHostMsg_GetPluginInfo_Status::kAllowed),
      title_(title),
      has_host_(false),
      context_menu_request_id_(0) {
  RenderThread::Get()->AddObserver(this);
}

CefPluginPlaceholder::~CefPluginPlaceholder() {
  RenderThread::Get()->RemoveObserver(this);
  if (context_menu_request_id_ && render_frame())
    render_frame()->CancelContextMenu(context_menu_request_id_);
}

// static
CefPluginPlaceholder* CefPluginPlaceholder::CreateLoadableMissingPlugin(
    content::RenderFrame* render_frame,
    blink::WebLocalFrame* frame,
    const blink::WebPluginParams& params) {
  const base::StringPiece template_html(
      ResourceBundle::GetSharedInstance().GetRawDataResource(
          IDR_BLOCKED_PLUGIN_HTML));

  base::DictionaryValue values;
  values.SetString("message",
                   l10n_util::GetStringUTF8(IDS_PLUGIN_NOT_SUPPORTED));

  std::string html_data = webui::GetI18nTemplateHtml(template_html, &values);

  // Will destroy itself when its WebViewPlugin is going away.
  return new CefPluginPlaceholder(render_frame, frame, params, html_data,
                                  params.mimeType);
}

// static
CefPluginPlaceholder* CefPluginPlaceholder::CreateBlockedPlugin(
    content::RenderFrame* render_frame,
    blink::WebLocalFrame* frame,
    const blink::WebPluginParams& params,
    const content::WebPluginInfo& info,
    const std::string& identifier,
    const base::string16& name,
    int template_id,
    const base::string16& message,
    const PlaceholderPosterInfo& poster_info) {
  base::DictionaryValue values;
  values.SetString("message", message);
  values.SetString("name", name);
  values.SetString("hide", l10n_util::GetStringUTF8(IDS_PLUGIN_HIDE));
  values.SetString("pluginType",
                   frame->view()->mainFrame()->isWebLocalFrame() &&
                   frame->view()->mainFrame()->document().isPluginDocument()
                       ? "document"
                       : "embedded");

  if (!poster_info.poster_attribute.empty()) {
    values.SetString("poster", poster_info.poster_attribute);
    values.SetString("baseurl", poster_info.base_url.spec());

    if (!poster_info.custom_poster_size.IsEmpty()) {
      float zoom_factor =
          blink::WebView::zoomLevelToZoomFactor(frame->view()->zoomLevel());
      int width = roundf(poster_info.custom_poster_size.width() / zoom_factor);
      int height =
          roundf(poster_info.custom_poster_size.height() / zoom_factor);
      values.SetString("visibleWidth", base::IntToString(width) + "px");
      values.SetString("visibleHeight", base::IntToString(height) + "px");
    }
  }

  const base::StringPiece template_html(
      ResourceBundle::GetSharedInstance().GetRawDataResource(template_id));

  DCHECK(!template_html.empty()) << "unable to load template. ID: "
                                 << template_id;
  std::string html_data = webui::GetI18nTemplateHtml(template_html, &values);

  // |blocked_plugin| will destroy itself when its WebViewPlugin is going away.
  CefPluginPlaceholder* blocked_plugin = new CefPluginPlaceholder(
      render_frame, frame, params, html_data, name);

  if (!poster_info.poster_attribute.empty())
    blocked_plugin->BlockForPowerSaverPoster();
  blocked_plugin->SetPluginInfo(info);
  blocked_plugin->SetIdentifier(identifier);
  return blocked_plugin;
}

void CefPluginPlaceholder::SetStatus(
    CefViewHostMsg_GetPluginInfo_Status status) {
  status_ = status;
}

bool CefPluginPlaceholder::OnMessageReceived(const IPC::Message& message) {
  // We don't swallow these messages because multiple blocked plugins and other
  // objects have an interest in them.
  IPC_BEGIN_MESSAGE_MAP(CefPluginPlaceholder, message)
    IPC_MESSAGE_HANDLER(CefViewMsg_LoadBlockedPlugins, OnLoadBlockedPlugins)
  IPC_END_MESSAGE_MAP()

  return false;
}

void CefPluginPlaceholder::OpenAboutPluginsCallback() {
  // CEF does not use IDR_DISABLED_PLUGIN_HTML which would originate this
  // callback for the chrome://plugins link.
  NOTREACHED();
}

void CefPluginPlaceholder::PluginListChanged() {
  if (!GetFrame() || !plugin())
    return;
  blink::WebDocument document = GetFrame()->top()->document();
  if (document.isNull())
    return;

  CefViewHostMsg_GetPluginInfo_Output output;
  std::string mime_type(GetPluginParams().mimeType.utf8());
  blink::WebString top_origin = GetFrame()->top()->securityOrigin().toString();
  render_frame()->Send(
      new CefViewHostMsg_GetPluginInfo(routing_id(),
                                          GURL(GetPluginParams().url),
                                          GURL(top_origin),
                                          mime_type,
                                          &output));
  if (output.status == status_)
    return;
  blink::WebPlugin* new_plugin = CefContentRendererClient::CreatePlugin(
      render_frame(), GetFrame(), GetPluginParams(), output);
  ReplacePlugin(new_plugin);
}

void CefPluginPlaceholder::OnMenuAction(int request_id, unsigned action) {
  DCHECK_EQ(context_menu_request_id_, request_id);
  if (g_last_active_menu != this)
    return;
  switch (action) {
    case chrome::MENU_COMMAND_PLUGIN_RUN: {
      RenderThread::Get()->RecordAction(UserMetricsAction("Plugin_Load_Menu"));
      MarkPluginEssential(
          content::PluginInstanceThrottler::UNTHROTTLE_METHOD_BY_CLICK);
      LoadPlugin();
      break;
    }
    case chrome::MENU_COMMAND_PLUGIN_HIDE: {
      RenderThread::Get()->RecordAction(UserMetricsAction("Plugin_Hide_Menu"));
      HidePlugin();
      break;
    }
    default:
      NOTREACHED();
  }
}

void CefPluginPlaceholder::OnMenuClosed(int request_id) {
  DCHECK_EQ(context_menu_request_id_, request_id);
  context_menu_request_id_ = 0;
}

v8::Local<v8::Value> CefPluginPlaceholder::GetV8Handle(
    v8::Isolate* isolate) {
  return gin::CreateHandle(isolate, this).ToV8();
}

void CefPluginPlaceholder::ShowContextMenu(
    const blink::WebMouseEvent& event) {
  if (context_menu_request_id_)
    return;  // Don't allow nested context menu requests.

  content::ContextMenuParams params;

  if (!title_.empty()) {
    content::MenuItem name_item;
    name_item.label = title_;
    params.custom_items.push_back(name_item);

    content::MenuItem separator_item;
    separator_item.type = content::MenuItem::SEPARATOR;
    params.custom_items.push_back(separator_item);
  }

  if (!GetPluginInfo().path.value().empty()) {
    content::MenuItem run_item;
    run_item.action = chrome::MENU_COMMAND_PLUGIN_RUN;
    // Disable this menu item if the plugin is blocked by policy.
    run_item.enabled = LoadingAllowed();
    run_item.label = l10n_util::GetStringUTF16(IDS_CONTENT_CONTEXT_PLUGIN_RUN);
    params.custom_items.push_back(run_item);
  }

  content::MenuItem hide_item;
  hide_item.action = chrome::MENU_COMMAND_PLUGIN_HIDE;
  bool is_main_frame_plugin_document =
      GetFrame()->view()->mainFrame()->isWebLocalFrame() &&
      GetFrame()->view()->mainFrame()->document().isPluginDocument();
  hide_item.enabled = !is_main_frame_plugin_document;
  hide_item.label = l10n_util::GetStringUTF16(IDS_CONTENT_CONTEXT_PLUGIN_HIDE);
  params.custom_items.push_back(hide_item);

  params.x = event.windowX;
  params.y = event.windowY;

  context_menu_request_id_ = render_frame()->ShowContextMenu(this, params);
  g_last_active_menu = this;
}

blink::WebPlugin* CefPluginPlaceholder::CreatePlugin() {
  scoped_ptr<content::PluginInstanceThrottler> throttler;
  // If the plugin has already been marked essential in its placeholder form,
  // we shouldn't create a new throttler and start the process all over again.
  if (power_saver_enabled()) {
    throttler = content::PluginInstanceThrottler::Create();
    // PluginPreroller manages its own lifetime.
    new CefPluginPreroller(
        render_frame(), GetFrame(), GetPluginParams(),
        GetPluginInfo(), GetIdentifier(), title_,
        l10n_util::GetStringFUTF16(IDS_PLUGIN_BLOCKED, title_),
        throttler.get());
  }
  return render_frame()->CreatePlugin(GetFrame(), GetPluginInfo(),
                                      GetPluginParams(), throttler.Pass());
}

gin::ObjectTemplateBuilder CefPluginPlaceholder::GetObjectTemplateBuilder(
    v8::Isolate* isolate) {
  return gin::Wrappable<CefPluginPlaceholder>::GetObjectTemplateBuilder(
             isolate)
      .SetMethod<void (CefPluginPlaceholder::*)()>(
           "hide", &CefPluginPlaceholder::HideCallback)
      .SetMethod<void (CefPluginPlaceholder::*)()>(
           "load", &CefPluginPlaceholder::LoadCallback)
      .SetMethod<void (CefPluginPlaceholder::*)()>(
           "didFinishLoading",
           &CefPluginPlaceholder::DidFinishLoadingCallback)
      .SetMethod("openAboutPlugins",
                 &CefPluginPlaceholder::OpenAboutPluginsCallback);
}
