#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "include/cef_drag_data.h"
#include "include/cef_request.h"
#include "include/cef_resource_bundle.h"
#include "include/test/cef_translator_test.h"
#include "include/cef_urlrequest.h"
#include "include/cef_xml_reader.h"
#include "include/cef_zip_reader.h"
#include "include/cef_web_plugin.h"
#include "include/cef_geolocation.h"
#include "include/cef_path_util.h"
#include "include/cef_command_line.h"
#include "include/cef_trace.h"

#include "include/internal/cef_string.h"
#include "include/internal/cef_types_wrappers.h"


CefRefPtr<CefDragData> CefDragData::Create() {
    DebugBreak();
    return nullptr;
}

CefRefPtr<CefPostData> CefPostData::Create() {
    DebugBreak();
    return nullptr;
}

CefRefPtr<CefResourceBundle> CefResourceBundle::GetGlobal(void) {
    DebugBreak();
    return nullptr;
}

CefRefPtr<CefTranslatorTest> CefTranslatorTest::Create(void) {
    DebugBreak();
    return nullptr;
}

CefRefPtr<CefURLRequest> CefURLRequest::Create(
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefURLRequestClient> client,
    CefRefPtr<CefRequestContext> request_context) {
    DebugBreak();
    return nullptr;
}

CefRefPtr<CefXmlReader> CefXmlReader::Create(CefRefPtr<CefStreamReader> stream,
    EncodingType encodingType,
    const CefString& URI) {
    DebugBreak();
    return nullptr;
}

CefRefPtr<CefZipReader> CefZipReader::Create(CefRefPtr<CefStreamReader> stream) {
    DebugBreak();
    return nullptr;
}

void CefIsWebPluginUnstable(const CefString& path, CefRefPtr<CefWebPluginUnstableCallback> callback) {
    DebugBreak();
}

bool CefGetGeolocation(CefRefPtr<CefGetGeolocationCallback> callback) {
    DebugBreak();
    return false;
}

bool CefAddCrossOriginWhitelistEntry(const CefString& source_origin, const CefString& target_protocol, const CefString& target_domain, bool allow_target_subdomains) {
    DebugBreak();
    return false;
}

bool CefRemoveCrossOriginWhitelistEntry(const CefString& source_origin, const CefString& target_protocol, const CefString& target_domain, bool allow_target_subdomains) {
    DebugBreak();
    return false;
}

bool CefClearCrossOriginWhitelist() {
    DebugBreak();
    return false;
}

bool CefParseURL(const CefString& url, CefURLParts& parts) {
    DebugBreak();
    return false;
}

bool CefCreateURL(const CefURLParts& parts, CefString& url) {
    DebugBreak();
    return false;
}

CefString CefFormatUrlForSecurityDisplay(const CefString& origin_url, const CefString& languages) {
    DebugBreak();
    return "";
}

CefString CefGetMimeType(const CefString& extension) {
    DebugBreak();
    return "";
}

void CefGetExtensionsForMimeType(const CefString& mime_type, std::vector<CefString>& extensions) {
    DebugBreak();
}

CefString CefBase64Encode(const void* data, size_t data_size) {
    DebugBreak();
    return "";
}

CefString CefURIEncode(const CefString& text, bool use_plus) {
    DebugBreak();
    return "";
}

CefString CefURIDecode(const CefString& text, bool convert_to_utf8, cef_uri_unescape_rule_t unescape_rule) {
    DebugBreak();
    return "";
}

bool CefParseCSSColor(const CefString& string, bool strict, cef_color_t& color) {
    DebugBreak();
    return false;
}

CefRefPtr<CefValue> CefParseJSON(const CefString& json_string, cef_json_parser_options_t options) {
    DebugBreak();
    return nullptr;
}

CefRefPtr<CefValue> CefParseJSONAndReturnError(const CefString& json_string, cef_json_parser_options_t options, cef_json_parser_error_t& error_code_out, CefString& error_msg_out) {
    DebugBreak();
    return nullptr;
}

CefString CefWriteJSON(CefRefPtr<CefValue> node, cef_json_writer_options_t options) {
    DebugBreak();
    return "";
}

bool CefGetPath(PathKey key, CefString& path) {
    DebugBreak();
    return false;
}

bool CefLaunchProcess(CefRefPtr<CefCommandLine> command_line) {
    DebugBreak();
    return false;
}

bool CefBeginTracing(const CefString& categories, CefRefPtr<CefCompletionCallback> callback) {
    DebugBreak();
    return true;
}

bool CefEndTracing(const CefString& tracing_file, CefRefPtr<CefEndTracingCallback> callback) {
    DebugBreak();
    return false;
}

int64 CefNowFromSystemTraceTime() {
    DebugBreak();
    return 0;
}

CefRefPtr<CefBinaryValue> CefBase64Decode(const CefString& data) {
    DebugBreak();
    return nullptr;
}
#endif
