// Copyright (c) 2013 The Chromium Embedded Framework Authors.
// Portions copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/renderer/content_renderer_client.h"

#include "libcef/browser/context.h"
#include "libcef/common/cef_messages.h"
#include "libcef/common/cef_switches.h"
#include "libcef/common/content_client.h"
#include "libcef/common/extensions/extensions_client.h"
#include "libcef/common/extensions/extensions_util.h"
#include "libcef/common/request_impl.h"
#include "libcef/common/values_impl.h"
#include "libcef/renderer/browser_impl.h"
#include "libcef/renderer/extensions/extensions_dispatcher_delegate.h"
#include "libcef/renderer/extensions/extensions_renderer_client.h"
#include "libcef/renderer/extensions/print_web_view_helper_delegate.h"
#include "libcef/renderer/media/cef_key_systems.h"
#include "libcef/renderer/pepper/pepper_helper.h"
#include "libcef/renderer/plugins/cef_plugin_placeholder.h"
#include "libcef/renderer/plugins/plugin_preroller.h"
#include "libcef/renderer/render_frame_observer.h"
#include "libcef/renderer/render_message_filter.h"
#include "libcef/renderer/render_process_observer.h"
#include "libcef/renderer/thread_util.h"
#include "libcef/renderer/v8_impl.h"
#include "libcef/renderer/webkit_glue.h"

#include "base/command_line.h"
#include "base/metrics/user_metrics_action.h"
#include "base/path_service.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/extensions/extension_process_policy.h"
#include "chrome/common/pepper_permission_util.h"
#include "chrome/common/url_constants.h"
#include "chrome/grit/generated_resources.h"
#include "chrome/renderer/content_settings_observer.h"
#include "chrome/renderer/extensions/resource_request_policy.h"
#include "chrome/renderer/loadtimes_extension_bindings.h"
#include "chrome/renderer/pepper/chrome_pdf_print_client.h"
#include "chrome/renderer/spellchecker/spellcheck.h"
#include "chrome/renderer/spellchecker/spellcheck_provider.h"
#include "components/content_settings/core/common/content_settings_types.h"
#include "components/nacl/common/nacl_constants.h"
#include "components/printing/renderer/print_web_view_helper.h"
#include "components/web_cache/renderer/web_cache_render_process_observer.h"
#include "content/child/worker_task_runner.h"
#include "content/common/frame_messages.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/child/child_thread.h"
#include "content/public/common/content_constants.h"
#include "content/public/common/content_paths.h"
#include "content/public/renderer/plugin_instance_throttler.h"
#include "content/public/renderer/render_thread.h"
#include "content/public/renderer/render_view.h"
#include "content/public/renderer/render_view_visitor.h"
#include "content/renderer/render_frame_impl.h"
#include "extensions/common/constants.h"
#include "extensions/common/switches.h"
#include "extensions/renderer/dispatcher.h"
#include "extensions/renderer/dispatcher_delegate.h"
#include "extensions/renderer/extension_frame_helper.h"
#include "extensions/renderer/extension_helper.h"
#include "extensions/renderer/guest_view/extensions_guest_view_container.h"
#include "extensions/renderer/guest_view/extensions_guest_view_container_dispatcher.h"
#include "extensions/renderer/guest_view/mime_handler_view/mime_handler_view_container.h"
#include "ipc/ipc_sync_channel.h"
#include "media/base/media.h"
#include "third_party/WebKit/public/platform/WebPrerenderingSupport.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/public/platform/WebURL.h"
#include "third_party/WebKit/public/web/WebConsoleMessage.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebElement.h"
#include "third_party/WebKit/public/web/WebFrame.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebPluginParams.h"
#include "third_party/WebKit/public/web/WebPrerendererClient.h"
#include "third_party/WebKit/public/web/WebRuntimeFeatures.h"
#include "third_party/WebKit/public/web/WebSecurityPolicy.h"
#include "third_party/WebKit/public/web/WebView.h"
#include "ui/base/l10n/l10n_util.h"

#if defined(OS_MACOSX)
#include "base/mac/mac_util.h"
#include "base/strings/sys_string_conversions.h"
#endif

namespace {

// Stub implementation of blink::WebPrerenderingSupport.
class CefPrerenderingSupport : public blink::WebPrerenderingSupport {
 private:
  void add(const blink::WebPrerender& prerender) override {}
  void cancel(const blink::WebPrerender& prerender) override {}
  void abandon(const blink::WebPrerender& prerender) override {}
};

// Stub implementation of blink::WebPrerendererClient.
class CefPrerendererClient : public content::RenderViewObserver,
                             public blink::WebPrerendererClient {
 public:
  explicit CefPrerendererClient(content::RenderView* render_view)
      : content::RenderViewObserver(render_view) {
    DCHECK(render_view);
    render_view->GetWebView()->setPrerendererClient(this);
  }

 private:
  ~CefPrerendererClient() override {}

  void willAddPrerender(blink::WebPrerender* prerender) override {}
};

void IsGuestViewApiAvailableToScriptContext(
    bool* api_is_available,
    extensions::ScriptContext* context) {
  if (context->GetAvailability("guestViewInternal").is_available()) {
    *api_is_available = true;
  }
}

void AppendParams(const std::vector<base::string16>& additional_names,
                  const std::vector<base::string16>& additional_values,
                  blink::WebVector<blink::WebString>* existing_names,
                  blink::WebVector<blink::WebString>* existing_values) {
  DCHECK(additional_names.size() == additional_values.size());
  DCHECK(existing_names->size() == existing_values->size());

  size_t existing_size = existing_names->size();
  size_t total_size = existing_size + additional_names.size();

  blink::WebVector<blink::WebString> names(total_size);
  blink::WebVector<blink::WebString> values(total_size);

  for (size_t i = 0; i < existing_size; ++i) {
    names[i] = (*existing_names)[i];
    values[i] = (*existing_values)[i];
  }

  for (size_t i = 0; i < additional_names.size(); ++i) {
    names[existing_size + i] = additional_names[i];
    values[existing_size + i] = additional_values[i];
  }

  existing_names->swap(names);
  existing_values->swap(values);
}

std::string GetPluginInstancePosterAttribute(
    const blink::WebPluginParams& params) {
  DCHECK_EQ(params.attributeNames.size(), params.attributeValues.size());

  for (size_t i = 0; i < params.attributeNames.size(); ++i) {
    if (params.attributeNames[i].utf8() == "poster" &&
        !params.attributeValues[i].isEmpty()) {
      return params.attributeValues[i].utf8();
    }
  }
  return std::string();
}

bool IsStandaloneExtensionProcess() {
  return base::CommandLine::ForCurrentProcess()->HasSwitch(
      extensions::switches::kExtensionProcess);
}

bool CrossesExtensionExtents(
    blink::WebLocalFrame* frame,
    const GURL& new_url,
    bool is_extension_url,
    bool is_initial_navigation) {
  DCHECK(!frame->parent());
  GURL old_url(frame->document().url());

  extensions::RendererExtensionRegistry* extension_registry =
      extensions::RendererExtensionRegistry::Get();

  // If old_url is still empty and this is an initial navigation, then this is
  // a window.open operation.  We should look at the opener URL.  Note that the
  // opener is a local frame in this case.
  if (is_initial_navigation && old_url.is_empty() && frame->opener()) {
    blink::WebLocalFrame* opener_frame = frame->opener()->toWebLocalFrame();

    // If we're about to open a normal web page from a same-origin opener stuck
    // in an extension process, we want to keep it in process to allow the
    // opener to script it.
    blink::WebDocument opener_document = opener_frame->document();
    blink::WebSecurityOrigin opener_origin = opener_document.securityOrigin();
    bool opener_is_extension_url = !opener_origin.isUnique() &&
                                   extension_registry->GetExtensionOrAppByURL(
                                       opener_document.url()) != NULL;
    if (!is_extension_url &&
        !opener_is_extension_url &&
        IsStandaloneExtensionProcess() &&
        opener_origin.canRequest(blink::WebURL(new_url)))
      return false;

    // In all other cases, we want to compare against the URL that determines
    // the type of process.  In default Chrome, that's the URL of the opener's
    // top frame and not the opener frame itself.  In --site-per-process, we
    // can use the opener frame itself.
    // TODO(nick): Either wire this up to SiteIsolationPolicy, or to state on
    // |opener_frame|/its ancestors.
    if (base::CommandLine::ForCurrentProcess()->HasSwitch(
            switches::kSitePerProcess) ||
        base::CommandLine::ForCurrentProcess()->HasSwitch(
            switches::kIsolateExtensions))
      old_url = opener_frame->document().url();
    else
      old_url = opener_frame->top()->document().url();
  }

  // Only consider keeping non-app URLs in an app process if this window
  // has an opener (in which case it might be an OAuth popup that tries to
  // script an iframe within the app).
  bool should_consider_workaround = !!frame->opener();

  return extensions::CrossesExtensionProcessBoundary(
      *extension_registry->GetMainThreadExtensionSet(), old_url, new_url,
      should_consider_workaround);
}

}  // namespace

CefContentRendererClient::CefContentRendererClient()
    : devtools_agent_count_(0),
      uncaught_exception_stack_size_(0),
      single_process_cleanup_complete_(false) {
}

CefContentRendererClient::~CefContentRendererClient() {
}

// static
CefContentRendererClient* CefContentRendererClient::Get() {
  return static_cast<CefContentRendererClient*>(
      CefContentClient::Get()->renderer());
}

CefRefPtr<CefBrowserImpl> CefContentRendererClient::GetBrowserForView(
    content::RenderView* view) {
  CEF_REQUIRE_RT_RETURN(NULL);

  BrowserMap::const_iterator it = browsers_.find(view);
  if (it != browsers_.end())
    return it->second;
  return NULL;
}

CefRefPtr<CefBrowserImpl> CefContentRendererClient::GetBrowserForMainFrame(
    blink::WebFrame* frame) {
  CEF_REQUIRE_RT_RETURN(NULL);

  BrowserMap::const_iterator it = browsers_.begin();
  for (; it != browsers_.end(); ++it) {
    content::RenderView* render_view = it->second->render_view();
    if (render_view && render_view->GetWebView() &&
        render_view->GetWebView()->mainFrame() == frame) {
      return it->second;
    }
  }

  return NULL;
}

void CefContentRendererClient::OnBrowserDestroyed(CefBrowserImpl* browser) {
  BrowserMap::iterator it = browsers_.begin();
  for (; it != browsers_.end(); ++it) {
    if (it->second.get() == browser) {
      browsers_.erase(it);
      return;
    }
  }

  // No browser was found in the map.
  NOTREACHED();
}

void CefContentRendererClient::WebKitInitialized() {
  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();

  // Create global objects associated with the default Isolate.
  CefV8IsolateCreated();

  // TODO(cef): Enable these once the implementation supports it.
  blink::WebRuntimeFeatures::enableNotifications(false);

  const CefContentClient::SchemeInfoList* schemes =
      CefContentClient::Get()->GetCustomSchemes();
  if (!schemes->empty()) {
    // Register the custom schemes.
    CefContentClient::SchemeInfoList::const_iterator it = schemes->begin();
    for (; it != schemes->end(); ++it) {
      const CefContentClient::SchemeInfo& info = *it;
      const blink::WebString& scheme =
          blink::WebString::fromUTF8(info.scheme_name);
      if (info.is_standard) {
        // Standard schemes must also be registered as CORS enabled to support
        // CORS-restricted requests (for example, XMLHttpRequest redirects).
        blink::WebSecurityPolicy::registerURLSchemeAsCORSEnabled(scheme);
      }
      if (info.is_local)
        blink::WebSecurityPolicy::registerURLSchemeAsLocal(scheme);
      if (info.is_display_isolated)
        blink::WebSecurityPolicy::registerURLSchemeAsDisplayIsolated(scheme);
    }
  }

  if (!cross_origin_whitelist_entries_.empty()) {
    // Add the cross-origin white list entries.
    for (size_t i = 0; i < cross_origin_whitelist_entries_.size(); ++i) {
      const Cef_CrossOriginWhiteListEntry_Params& entry =
          cross_origin_whitelist_entries_[i];
      GURL gurl = GURL(entry.source_origin);
      blink::WebSecurityPolicy::addOriginAccessWhitelistEntry(
          gurl,
          blink::WebString::fromUTF8(entry.target_protocol),
          blink::WebString::fromUTF8(entry.target_domain),
          entry.allow_target_subdomains);
    }
    cross_origin_whitelist_entries_.clear();
  }

  // The number of stack trace frames to capture for uncaught exceptions.
  if (command_line->HasSwitch(switches::kUncaughtExceptionStackSize)) {
    int uncaught_exception_stack_size = 0;
    base::StringToInt(
        command_line->GetSwitchValueASCII(
            switches::kUncaughtExceptionStackSize),
        &uncaught_exception_stack_size);

    if (uncaught_exception_stack_size > 0) {
      uncaught_exception_stack_size_ = uncaught_exception_stack_size;
      CefV8SetUncaughtExceptionStackSize(uncaught_exception_stack_size_);
    }
  }

  // Notify the render process handler.
  CefRefPtr<CefApp> application = CefContentClient::Get()->application();
  if (application.get()) {
    CefRefPtr<CefRenderProcessHandler> handler =
        application->GetRenderProcessHandler();
    if (handler.get())
      handler->OnWebKitInitialized();
  }
}

void CefContentRendererClient::OnRenderProcessShutdown() {
  // Destroy global objects associated with the default Isolate.
  CefV8IsolateDestroyed();
}

void CefContentRendererClient::DevToolsAgentAttached() {
  CEF_REQUIRE_RT();
  ++devtools_agent_count_;
}

void CefContentRendererClient::DevToolsAgentDetached() {
  CEF_REQUIRE_RT();
  --devtools_agent_count_;
  if (devtools_agent_count_ == 0 && uncaught_exception_stack_size_ > 0) {
    // When the last DevToolsAgent is detached the stack size is set to 0.
    // Restore the user-specified stack size here.
    CefV8SetUncaughtExceptionStackSize(uncaught_exception_stack_size_);
  }
}

scoped_refptr<base::SequencedTaskRunner>
    CefContentRendererClient::GetCurrentTaskRunner() {
  // Check if currently on the render thread.
  if (CEF_CURRENTLY_ON_RT())
    return render_task_runner_;
  return NULL;
}

void CefContentRendererClient::RunSingleProcessCleanup() {
  DCHECK(content::RenderProcessHost::run_renderer_in_process());

  // Make sure the render thread was actually started.
  if (!render_task_runner_.get())
    return;

  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    RunSingleProcessCleanupOnUIThread();
  } else {
    content::BrowserThread::PostTask(content::BrowserThread::UI, FROM_HERE,
        base::Bind(&CefContentRendererClient::RunSingleProcessCleanupOnUIThread,
                   base::Unretained(this)));
  }

  // Wait for the render thread cleanup to complete. Spin instead of using
  // base::WaitableEvent because calling Wait() is not allowed on the UI
  // thread.
  bool complete = false;
  do {
    {
      base::AutoLock lock_scope(single_process_cleanup_lock_);
      complete = single_process_cleanup_complete_;
    }
    if (!complete)
      base::PlatformThread::YieldCurrentThread();
  } while (!complete);
}

void CefContentRendererClient::RenderThreadStarted() {
  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();

  render_task_runner_ = base::ThreadTaskRunnerHandle::Get();
  observer_.reset(new CefRenderProcessObserver());
  web_cache_observer_.reset(new web_cache::WebCacheRenderProcessObserver());

  content::RenderThread* thread = content::RenderThread::Get();
  thread->AddObserver(observer_.get());
  thread->AddObserver(web_cache_observer_.get());
  thread->GetChannel()->AddFilter(new CefRenderMessageFilter);

  if (!command_line->HasSwitch(switches::kDisableSpellChecking)) {
    spellcheck_.reset(new SpellCheck());
    thread->AddObserver(spellcheck_.get());
  }

  if (content::RenderProcessHost::run_renderer_in_process()) {
    // When running in single-process mode register as a destruction observer
    // on the render thread's MessageLoop.
    base::MessageLoop::current()->AddDestructionObserver(this);
  }

  blink::WebPrerenderingSupport::initialize(new CefPrerenderingSupport());

  // Retrieve the new render thread information synchronously.
  CefProcessHostMsg_GetNewRenderThreadInfo_Params params;
  thread->Send(new CefProcessHostMsg_GetNewRenderThreadInfo(&params));

  // Cross-origin entries need to be added after WebKit is initialized.
  cross_origin_whitelist_entries_ = params.cross_origin_whitelist_entries;

#if defined(OS_MACOSX)
  if (base::mac::IsOSLionOrLater()) {
    base::ScopedCFTypeRef<CFStringRef> key(
        base::SysUTF8ToCFStringRef("NSScrollViewRubberbanding"));
    base::ScopedCFTypeRef<CFStringRef> value;

    // If the command-line switch is specified then set the value that will be
    // checked in RenderThreadImpl::Init(). Otherwise, remove the application-
    // level value.
    if (command_line->HasSwitch(switches::kDisableScrollBounce))
      value.reset(base::SysUTF8ToCFStringRef("false"));

    CFPreferencesSetAppValue(key, value, kCFPreferencesCurrentApplication);
    CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);
  }
#endif  // defined(OS_MACOSX)

  if (extensions::PdfExtensionEnabled()) {
    pdf_print_client_.reset(new ChromePDFPrintClient());
    pdf::PepperPDFHost::SetPrintClient(pdf_print_client_.get());
  }

  if (extensions::ExtensionsEnabled()) {
    extensions_client_.reset(new extensions::CefExtensionsClient);
    extensions::ExtensionsClient::Set(extensions_client_.get());

    extensions_renderer_client_.reset(
        new extensions::CefExtensionsRendererClient);
    extensions::ExtensionsRendererClient::Set(
        extensions_renderer_client_.get());

    extension_dispatcher_delegate_.reset(
        new extensions::CefExtensionsDispatcherDelegate());

    // Must be initialized after ExtensionsRendererClient.
    extension_dispatcher_.reset(
        new extensions::Dispatcher(extension_dispatcher_delegate_.get()));
    thread->AddObserver(extension_dispatcher_.get());

    guest_view_container_dispatcher_.reset(
        new extensions::ExtensionsGuestViewContainerDispatcher());
    thread->AddObserver(guest_view_container_dispatcher_.get());

    resource_request_policy_.reset(
      new extensions::ResourceRequestPolicy(extension_dispatcher_.get()));
  }

  // Notify the render process handler.
  CefRefPtr<CefApp> application = CefContentClient::Get()->application();
  if (application.get()) {
    CefRefPtr<CefRenderProcessHandler> handler =
        application->GetRenderProcessHandler();
    if (handler.get()) {
      CefRefPtr<CefListValueImpl> listValuePtr(
        new CefListValueImpl(&params.extra_info, false, true));
      handler->OnRenderThreadCreated(listValuePtr.get());
      listValuePtr->Detach(NULL);
    }
  }

  // Register extensions last because it will trigger WebKit initialization.
  thread->RegisterExtension(extensions_v8::LoadTimesExtension::Get());
}

void CefContentRendererClient::RenderFrameCreated(
    content::RenderFrame* render_frame) {
  new CefRenderFrameObserver(render_frame);
  new CefPepperHelper(render_frame);

  if (extensions::ExtensionsEnabled()) {
    new extensions::ExtensionFrameHelper(render_frame,
                                         extension_dispatcher_.get());
    extension_dispatcher_->OnRenderFrameCreated(render_frame);
  }

  BrowserCreated(render_frame->GetRenderView(), render_frame);
}

void CefContentRendererClient::RenderViewCreated(
    content::RenderView* render_view) {
  new CefPrerendererClient(render_view);
  new printing::PrintWebViewHelper(
      render_view,
      make_scoped_ptr<printing::PrintWebViewHelper::Delegate>(
          new extensions::CefPrintWebViewHelperDelegate()));

  if (extensions::ExtensionsEnabled()) {
    new extensions::ExtensionHelper(render_view, extension_dispatcher_.get());
  }

  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();
  if (!command_line->HasSwitch(switches::kDisableSpellChecking))
    new SpellCheckProvider(render_view, spellcheck_.get());

  BrowserCreated(render_view, render_view->GetMainRenderFrame());
}

bool CefContentRendererClient::OverrideCreatePlugin(
    content::RenderFrame* render_frame,
    blink::WebLocalFrame* frame,
    const blink::WebPluginParams& params,
    blink::WebPlugin** plugin) {
  if (!extensions::ExtensionsEnabled())
    return false;

  // Based on ChromeContentRendererClient::OverrideCreatePlugin.
  std::string orig_mime_type = params.mimeType.utf8();
  if (orig_mime_type == content::kBrowserPluginMimeType) {
    bool guest_view_api_available = false;
    extension_dispatcher_->script_context_set().ForEach(
        render_frame,
        base::Bind(&IsGuestViewApiAvailableToScriptContext,
                   &guest_view_api_available));
    if (guest_view_api_available)
      return false;
  }

  GURL url(params.url);
  CefViewHostMsg_GetPluginInfo_Output output;
  render_frame->Send(new CefViewHostMsg_GetPluginInfo(
      render_frame->GetRoutingID(), url, frame->top()->document().url(),
      orig_mime_type, &output));

  *plugin = CreatePlugin(render_frame, frame, params, output);
  return true;
}

bool CefContentRendererClient::HandleNavigation(
    content::RenderFrame* render_frame,
    content::DocumentState* document_state,
    int opener_id,
    blink::WebFrame* frame,
    const blink::WebURLRequest& request,
    blink::WebNavigationType type,
    blink::WebNavigationPolicy default_policy,
    bool is_redirect) {
  CefRefPtr<CefApp> application = CefContentClient::Get()->application();
  if (application.get()) {
    CefRefPtr<CefRenderProcessHandler> handler =
        application->GetRenderProcessHandler();
    if (handler.get()) {
      CefRefPtr<CefBrowserImpl> browserPtr =
          CefBrowserImpl::GetBrowserForMainFrame(frame->top());
      if (browserPtr.get()) {
        CefRefPtr<CefFrameImpl> framePtr = browserPtr->GetWebFrameImpl(frame);
        CefRefPtr<CefRequest> requestPtr(CefRequest::Create());
        CefRequestImpl* requestImpl =
            static_cast<CefRequestImpl*>(requestPtr.get());
        requestImpl->Set(request);
        requestImpl->SetReadOnly(true);

        cef_navigation_type_t navigation_type = NAVIGATION_OTHER;
        switch (type) {
          case blink::WebNavigationTypeLinkClicked:
            navigation_type = NAVIGATION_LINK_CLICKED;
            break;
          case blink::WebNavigationTypeFormSubmitted:
            navigation_type = NAVIGATION_FORM_SUBMITTED;
            break;
          case blink::WebNavigationTypeBackForward:
            navigation_type = NAVIGATION_BACK_FORWARD;
            break;
          case blink::WebNavigationTypeReload:
            navigation_type = NAVIGATION_RELOAD;
            break;
          case blink::WebNavigationTypeFormResubmitted:
            navigation_type = NAVIGATION_FORM_RESUBMITTED;
            break;
          case blink::WebNavigationTypeOther:
            navigation_type = NAVIGATION_OTHER;
            break;
        }

        if (handler->OnBeforeNavigation(browserPtr.get(), framePtr.get(),
                                        requestPtr.get(), navigation_type,
                                        is_redirect)) {
          return true;
        }
      }
    }
  }

  return false;
}

bool CefContentRendererClient::ShouldFork(blink::WebLocalFrame* frame,
                                          const GURL& url,
                                          const std::string& http_method,
                                          bool is_initial_navigation,
                                          bool is_server_redirect,
                                          bool* send_referrer) {
  DCHECK(!frame->parent());

  // For now, we skip the rest for POST submissions.  This is because
  // http://crbug.com/101395 is more likely to cause compatibility issues
  // with hosted apps and extensions than WebUI pages.  We will remove this
  // check when cross-process POST submissions are supported.
  if (http_method != "GET")
    return false;

  if (extensions::ExtensionsEnabled()) {
    const extensions::RendererExtensionRegistry* extension_registry =
        extensions::RendererExtensionRegistry::Get();

    // Determine if the new URL is an extension (excluding bookmark apps).
    const extensions::Extension* new_url_extension =
        extensions::GetNonBookmarkAppExtension(
            *extension_registry->GetMainThreadExtensionSet(), url);
    bool is_extension_url = !!new_url_extension;

    // If the navigation would cross an app extent boundary, we also need
    // to defer to the browser to ensure process isolation.  This is not
    // necessary for server redirects, which will be transferred to a new
    // process by the browser process when they are ready to commit.  It is
    // necessary for client redirects, which won't be transferred in the same
    // way.
    if (!is_server_redirect &&
        CrossesExtensionExtents(frame, url, is_extension_url,
                                is_initial_navigation)) {
      // Include the referrer in this case since we're going from a hosted web
      // page. (the packaged case is handled previously by the extension
      // navigation test)
      *send_referrer = true;
      return true;
    }

    // If this is a reload, check whether it has the wrong process type.  We
    // should send it to the browser if it's an extension URL (e.g., hosted app)
    // in a normal process, or if it's a process for an extension that has been
    // uninstalled.  Without --site-per-process mode, we never fork processes
    // for subframes, so this check only makes sense for top-level frames.
    // TODO(alexmos,nasko): Figure out how this check should work when reloading
    // subframes in --site-per-process mode.
    if (!frame->parent() && frame->document().url() == url) {
      if (is_extension_url != IsStandaloneExtensionProcess())
        return true;
    }
  }

  return false;
}

bool CefContentRendererClient::WillSendRequest(
    blink::WebFrame* frame,
    ui::PageTransition transition_type,
    const GURL& url,
    const GURL& first_party_for_cookies,
    GURL* new_url) {
  if (extensions::ExtensionsEnabled()) {
    // Check whether the request should be allowed. If not allowed, we reset the
    // URL to something invalid to prevent the request and cause an error.
    if (url.SchemeIs(extensions::kExtensionScheme) &&
        !resource_request_policy_->CanRequestResource(url, frame,
                                                      transition_type)) {
      *new_url = GURL(chrome::kExtensionInvalidRequestURL);
      return true;
    }

    if (url.SchemeIs(extensions::kExtensionResourceScheme) &&
        !resource_request_policy_->CanRequestExtensionResourceScheme(url,
                                                                     frame)) {
      *new_url = GURL(chrome::kExtensionResourceInvalidRequestURL);
      return true;
    }
  }

  return false;
}

content::BrowserPluginDelegate*
CefContentRendererClient::CreateBrowserPluginDelegate(
    content::RenderFrame* render_frame,
    const std::string& mime_type,
    const GURL& original_url) {
  DCHECK(extensions::ExtensionsEnabled());
  if (mime_type == content::kBrowserPluginMimeType) {
    return new extensions::ExtensionsGuestViewContainer(render_frame);
  } else {
    return new extensions::MimeHandlerViewContainer(
        render_frame, mime_type, original_url);
  }
}

void CefContentRendererClient::AddKeySystems(
    std::vector<media::KeySystemInfo>* key_systems) {
  AddCefKeySystems(key_systems);
}

void CefContentRendererClient::WillDestroyCurrentMessageLoop() {
  base::AutoLock lock_scope(single_process_cleanup_lock_);
  single_process_cleanup_complete_ = true;
}

// static
bool CefContentRendererClient::IsExtensionOrSharedModuleWhitelisted(
    const GURL& url, const std::set<std::string>& whitelist) {
  DCHECK(extensions::ExtensionsEnabled());
  const extensions::ExtensionSet* extension_set =
      extensions::RendererExtensionRegistry::Get()->GetMainThreadExtensionSet();
  return chrome::IsExtensionOrSharedModuleWhitelisted(url, extension_set,
      whitelist);
}

// static
blink::WebPlugin* CefContentRendererClient::CreatePlugin(
    content::RenderFrame* render_frame,
    blink::WebLocalFrame* frame,
    const blink::WebPluginParams& original_params,
    const CefViewHostMsg_GetPluginInfo_Output& output) {
  const content::WebPluginInfo& info = output.plugin;
  const std::string& actual_mime_type = output.actual_mime_type;
  const base::string16& group_name = output.group_name;
  const std::string& identifier = output.group_identifier;
  CefViewHostMsg_GetPluginInfo_Status status = output.status;
  GURL url(original_params.url);
  std::string orig_mime_type = original_params.mimeType.utf8();
  CefPluginPlaceholder* placeholder = NULL;

  // If the browser plugin is to be enabled, this should be handled by the
  // renderer, so the code won't reach here due to the early exit in
  // OverrideCreatePlugin.
  if (status == CefViewHostMsg_GetPluginInfo_Status::kNotFound ||
      orig_mime_type == content::kBrowserPluginMimeType) {
    placeholder = CefPluginPlaceholder::CreateLoadableMissingPlugin(
        render_frame, frame, original_params);
  } else {
    // TODO(bauerb): This should be in content/.
    blink::WebPluginParams params(original_params);
    for (size_t i = 0; i < info.mime_types.size(); ++i) {
      if (info.mime_types[i].mime_type == actual_mime_type) {
        AppendParams(info.mime_types[i].additional_param_names,
                     info.mime_types[i].additional_param_values,
                     &params.attributeNames, &params.attributeValues);
        break;
      }
    }
    if (params.mimeType.isNull() && (actual_mime_type.size() > 0)) {
      // Webkit might say that mime type is null while we already know the
      // actual mime type via CefViewHostMsg_GetPluginInfo. In that case
      // we should use what we know since WebpluginDelegateProxy does some
      // specific initializations based on this information.
      params.mimeType = blink::WebString::fromUTF8(actual_mime_type.c_str());
    }

    auto create_blocked_plugin =
        [&render_frame, &frame, &params, &info, &identifier, &group_name](
            int template_id, const base::string16& message) {
          return CefPluginPlaceholder::CreateBlockedPlugin(
              render_frame, frame, params, info, identifier, group_name,
              template_id, message, PlaceholderPosterInfo());
        };
    switch (status) {
      case CefViewHostMsg_GetPluginInfo_Status::kNotFound: {
        NOTREACHED();
        break;
      }
      case CefViewHostMsg_GetPluginInfo_Status::kAllowed:
      case CefViewHostMsg_GetPluginInfo_Status::kPlayImportantContent: {
        // Delay loading plugins if prerendering.
        // TODO(mmenke):  In the case of prerendering, feed into
        //                CefContentRendererClient::CreatePlugin instead, to
        //                reduce the chance of future regressions.
        bool is_prerendering = false;
        bool power_saver_enabled =
            status ==
                CefViewHostMsg_GetPluginInfo_Status::kPlayImportantContent;
        bool blocked_for_background_tab =
            render_frame->IsHidden() && power_saver_enabled;

        PlaceholderPosterInfo poster_info;
        if (power_saver_enabled) {
          poster_info.poster_attribute =
              GetPluginInstancePosterAttribute(params);
          poster_info.base_url = frame->document().url();
        }

        if (blocked_for_background_tab || is_prerendering ||
            !poster_info.poster_attribute.empty()) {
          placeholder = CefPluginPlaceholder::CreateBlockedPlugin(
              render_frame, frame, params, info, identifier, group_name,
              poster_info.poster_attribute.empty() ? IDR_BLOCKED_PLUGIN_HTML
                                                   : IDR_PLUGIN_POSTER_HTML,
              l10n_util::GetStringFUTF16(IDS_PLUGIN_BLOCKED, group_name),
              poster_info);
          placeholder->set_blocked_for_background_tab(
              blocked_for_background_tab);
          placeholder->set_blocked_for_prerendering(is_prerendering);
          placeholder->set_power_saver_enabled(power_saver_enabled);
          placeholder->AllowLoading();
          break;
        }

        scoped_ptr<content::PluginInstanceThrottler> throttler;
        if (power_saver_enabled) {
          throttler = content::PluginInstanceThrottler::Create();
          // PluginPreroller manages its own lifetime.
          new CefPluginPreroller(
              render_frame, frame, params, info, identifier, group_name,
              l10n_util::GetStringFUTF16(IDS_PLUGIN_BLOCKED, group_name),
              throttler.get());
        }

        return render_frame->CreatePlugin(frame, info, params,
                                          throttler.Pass());
      }
      case CefViewHostMsg_GetPluginInfo_Status::kNPAPINotSupported: {
        content::RenderThread::Get()->RecordAction(
            base::UserMetricsAction("Plugin_NPAPINotSupported"));
        placeholder = create_blocked_plugin(
            IDR_BLOCKED_PLUGIN_HTML,
            l10n_util::GetStringUTF16(IDS_PLUGIN_NOT_SUPPORTED_METRO));
        break;
      }
      case CefViewHostMsg_GetPluginInfo_Status::kDisabled: {
        // Intentionally using the blocked plugin resources instead of the
        // disabled plugin resources. This provides better messaging (no link to
        // chrome://plugins) and adds testing support.
        placeholder = create_blocked_plugin(
            IDR_BLOCKED_PLUGIN_HTML,
            l10n_util::GetStringFUTF16(IDS_PLUGIN_BLOCKED_BY_POLICY,
                                       group_name));
        break;
      }
      case CefViewHostMsg_GetPluginInfo_Status::kOutdatedBlocked: {
        NOTREACHED() << "Plugin installation is not supported.";
        break;
      }
      case CefViewHostMsg_GetPluginInfo_Status::kOutdatedDisallowed: {
        placeholder = create_blocked_plugin(
            IDR_BLOCKED_PLUGIN_HTML,
            l10n_util::GetStringFUTF16(IDS_PLUGIN_OUTDATED, group_name));
        break;
      }
      case CefViewHostMsg_GetPluginInfo_Status::kUnauthorized: {
        placeholder = create_blocked_plugin(
            IDR_BLOCKED_PLUGIN_HTML,
            l10n_util::GetStringFUTF16(IDS_PLUGIN_NOT_AUTHORIZED, group_name));
        placeholder->AllowLoading();
        break;
      }
      case CefViewHostMsg_GetPluginInfo_Status::kBlocked: {
        placeholder = create_blocked_plugin(
            IDR_BLOCKED_PLUGIN_HTML,
            l10n_util::GetStringFUTF16(IDS_PLUGIN_BLOCKED, group_name));
        placeholder->AllowLoading();
        content::RenderThread::Get()->RecordAction(
            base::UserMetricsAction("Plugin_Blocked"));
        break;
      }
      case CefViewHostMsg_GetPluginInfo_Status::kBlockedByPolicy: {
        placeholder = create_blocked_plugin(
            IDR_BLOCKED_PLUGIN_HTML,
            l10n_util::GetStringFUTF16(IDS_PLUGIN_BLOCKED_BY_POLICY,
                                       group_name));
        content::RenderThread::Get()->RecordAction(
            base::UserMetricsAction("Plugin_BlockedByPolicy"));
        break;
      }
    }
  }
  placeholder->SetStatus(status);
  return placeholder->plugin();
}

void CefContentRendererClient::BrowserCreated(
    content::RenderView* render_view,
    content::RenderFrame* render_frame) {
  // Retrieve the browser information synchronously. This will also register
  // the routing ids with the browser info object in the browser process.
  CefProcessHostMsg_GetNewBrowserInfo_Params params;
  content::RenderThread::Get()->Send(
      new CefProcessHostMsg_GetNewBrowserInfo(
          render_view->GetRoutingID(),
          render_frame->GetRoutingID(),
          &params));
  DCHECK_GT(params.browser_id, 0);

  if (params.is_guest_view) {
    // Don't create a CefBrowser for guest views.
    return;
  }

  // Don't create another browser object if one already exists for the view.
  if (GetBrowserForView(render_view).get())
    return;

#if defined(OS_MACOSX)
  // FIXME: It would be better if this API would be a callback from the
  // WebKit layer, or if it would be exposed as an WebView instance method; the
  // current implementation uses a static variable, and WebKit needs to be
  // patched in order to make it work for each WebView instance
  render_view->GetWebView()->setUseExternalPopupMenusThisInstance(
      !params.is_windowless);
#endif

  CefRefPtr<CefBrowserImpl> browser =
      new CefBrowserImpl(render_view, params.browser_id, params.is_popup,
                         params.is_windowless);
  browsers_.insert(std::make_pair(render_view, browser));

  // Notify the render process handler.
  CefRefPtr<CefApp> application = CefContentClient::Get()->application();
  if (application.get()) {
    CefRefPtr<CefRenderProcessHandler> handler =
        application->GetRenderProcessHandler();
    if (handler.get())
      handler->OnBrowserCreated(browser.get());
  }
}

void CefContentRendererClient::RunSingleProcessCleanupOnUIThread() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));

  // Clean up the single existing RenderProcessHost.
  content::RenderProcessHost* host = NULL;
  content::RenderProcessHost::iterator iterator(
      content::RenderProcessHost::AllHostsIterator());
  if (!iterator.IsAtEnd()) {
    host = iterator.GetCurrentValue();
    host->Cleanup();
    iterator.Advance();
    DCHECK(iterator.IsAtEnd());
  }
  DCHECK(host);

  // Clear the run_renderer_in_process() flag to avoid a DCHECK in the
  // RenderProcessHost destructor.
  content::RenderProcessHost::SetRunRendererInProcess(false);

  // Deletion of the RenderProcessHost object will stop the render thread and
  // result in a call to WillDestroyCurrentMessageLoop.
  // Cleanup() will cause deletion to be posted as a task on the UI thread but
  // this task will only execute when running in multi-threaded message loop
  // mode (because otherwise the UI message loop has already stopped). Therefore
  // we need to explicitly delete the object when not running in this mode.
  if (!CefContext::Get()->settings().multi_threaded_message_loop)
    delete host;
}
