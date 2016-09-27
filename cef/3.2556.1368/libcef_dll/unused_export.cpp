#include <windows.h>

#define CEF_EXPORT extern "C" __declspec(dllexport) 
/*
// CEF_EXPORT void cef_time_to_timet() {DebugBreak();}
// CEF_EXPORT void cef_time_now() {DebugBreak();}
// CEF_EXPORT void cef_time_delta() {DebugBreak();}

// CEF_EXPORT void cef_get_min_log_level() {DebugBreak();}
// CEF_EXPORT void cef_get_current_platform_thread_handle() { DebugBreak(); }

// CEF_EXPORT void cef_log(const char* file, int line, int severity, const char* message) { DebugBreak(); }

// CEF_EXPORT void cef_string_utf8_clear() {DebugBreak();}

// CEF_EXPORT void cef_cookie_manager_get_global_manager() {DebugBreak();}
// CEF_EXPORT void cef_now_from_system_trace_time() {DebugBreak();}

// CEF_EXPORT void cef_string_wide_to_utf8() {DebugBreak();}
CEF_EXPORT void cef_get_current_platform_thread_id() { DebugBreak(); }
// CEF_EXPORT void cef_string_utf16_clear() {DebugBreak();}
// CEF_EXPORT void cef_string_utf16_set() {DebugBreak();}
// CEF_EXPORT void cef_string_list_free() {DebugBreak();}
// CEF_EXPORT void cef_string_list_copy() {DebugBreak();}
// CEF_EXPORT void cef_binary_value_create() {DebugBreak();}
// CEF_EXPORT void cef_string_list_alloc() {DebugBreak();}
// CEF_EXPORT void cef_browser_host_create_browser() {DebugBreak();}
// CEF_EXPORT void cef_browser_host_create_browser_sync() {DebugBreak();}
// CEF_EXPORT void cef_api_hash() { DebugBreak(); }
// CEF_EXPORT void cef_string_userfree_utf16_free() {DebugBreak();}

CEF_EXPORT void cef_string_map_alloc() {DebugBreak();}
CEF_EXPORT void cef_string_map_free() {DebugBreak();}
// CEF_EXPORT void cef_command_line_create() {DebugBreak();}
// CEF_EXPORT void cef_command_line_get_global() {DebugBreak();}
// CEF_EXPORT void cef_cookie_manager_get_global_manage() { DebugBreak(); }
// CEF_EXPORT void cef_cookie_manager_create_manager() {DebugBreak();}
// CEF_EXPORT void cef_dictionary_value_create() {DebugBreak();}
CEF_EXPORT void cef_drag_data_create() {DebugBreak();}
// CEF_EXPORT void cef_list_value_create() {DebugBreak();}
CEF_EXPORT void cef_post_data_create() {DebugBreak();}
CEF_EXPORT void cef_post_data_element_create() { DebugBreak(); }
CEF_EXPORT void cef_print_settings_create() { DebugBreak(); }
// CEF_EXPORT void cef_process_message_create() { DebugBreak(); }
// CEF_EXPORT void cef_request_context_get_global_context() { DebugBreak(); }
// CEF_EXPORT void cef_request_context_create_context() { DebugBreak(); }
// CEF_EXPORT void create_context_shared() { DebugBreak(); }
CEF_EXPORT void cef_string_multimap_alloc() { DebugBreak(); }
CEF_EXPORT void cef_string_multimap_free() { DebugBreak(); }
CEF_EXPORT void cef_request_create() { DebugBreak(); }
CEF_EXPORT void cef_resource_bundle_get_global() { DebugBreak(); }
CEF_EXPORT void cef_response_create() { DebugBreak(); }
CEF_EXPORT void cef_stream_reader_create_for_file() { DebugBreak(); }
CEF_EXPORT void cef_stream_reader_create_for_data() { DebugBreak(); }
CEF_EXPORT void cef_stream_reader_create_for_handler() { DebugBreak(); }
CEF_EXPORT void cef_stream_writer_create_for_file() { DebugBreak(); }
CEF_EXPORT void cef_stream_writer_create_for_handler() { DebugBreak(); }
CEF_EXPORT void cef_task_runner_get_for_current_thread() { DebugBreak(); }
CEF_EXPORT void cef_task_runner_get_for_thread() { DebugBreak(); }
CEF_EXPORT void cef_translator_test_create() { DebugBreak(); }
CEF_EXPORT void cef_translator_test_object_child_child_create() { DebugBreak(); }
CEF_EXPORT void cef_translator_test_object_child_create() { DebugBreak(); }
CEF_EXPORT void cef_translator_test_object_create() { DebugBreak(); }
CEF_EXPORT void cef_urlrequest_create() { DebugBreak(); }
CEF_EXPORT void cef_v8context_get_current_context() { DebugBreak(); }
CEF_EXPORT void cef_v8context_get_entered_context() { DebugBreak(); }
CEF_EXPORT void cef_v8context_in_context() { DebugBreak(); }
CEF_EXPORT void cef_v8stack_trace_get_current() { DebugBreak(); }
CEF_EXPORT void cef_v8value_create_undefined() { DebugBreak(); }
CEF_EXPORT void cef_v8value_create_null() { DebugBreak(); }
CEF_EXPORT void cef_v8value_create_bool() { DebugBreak(); }
CEF_EXPORT void cef_v8value_create_int() { DebugBreak(); }
CEF_EXPORT void cef_v8value_create_uint() { DebugBreak(); }
CEF_EXPORT void cef_v8value_create_double() { DebugBreak(); }
CEF_EXPORT void cef_v8value_create_date() { DebugBreak(); }
CEF_EXPORT void cef_v8value_create_string() { DebugBreak(); }
CEF_EXPORT void cef_v8value_create_object() { DebugBreak(); }
CEF_EXPORT void cef_v8value_create_array() { DebugBreak(); }
CEF_EXPORT void cef_v8value_create_function() { DebugBreak(); }
CEF_EXPORT void cef_value_create() { DebugBreak(); }
CEF_EXPORT void cef_xml_reader_create() { DebugBreak(); }
CEF_EXPORT void cef_zip_reader_create() { DebugBreak(); }
// CEF_EXPORT void cef_string_utf16_cmp() { DebugBreak(); }
// CEF_EXPORT void cef_string_list_size() { DebugBreak(); }
// CEF_EXPORT void cef_string_list_value() { DebugBreak(); }
// CEF_EXPORT void cef_string_list_append() { DebugBreak(); }
CEF_EXPORT void cef_string_map_size() { DebugBreak(); }
CEF_EXPORT void cef_string_map_key() { DebugBreak(); }
CEF_EXPORT void cef_string_map_value() { DebugBreak(); }
CEF_EXPORT void cef_string_map_append() { DebugBreak(); }
CEF_EXPORT void cef_string_map_clear() { DebugBreak(); }

CEF_EXPORT void cef_string_multimap_size() { DebugBreak(); }
CEF_EXPORT void cef_string_multimap_key() { DebugBreak(); }
CEF_EXPORT void cef_string_multimap_value() { DebugBreak(); }
CEF_EXPORT void cef_string_multimap_append() { DebugBreak(); }
// CEF_EXPORT void cef_string_utf8_to_utf16() { DebugBreak(); }
// CEF_EXPORT void cef_string_utf16_to_utf8() { DebugBreak(); }
// CEF_EXPORT void cef_visit_web_plugin_info() { DebugBreak(); }
// CEF_EXPORT void cef_refresh_web_plugins() { DebugBreak(); }
// CEF_EXPORT void cef_add_web_plugin_path() { DebugBreak(); }
// CEF_EXPORT void cef_add_web_plugin_directory() { DebugBreak(); }
// CEF_EXPORT void cef_remove_web_plugin_path() { DebugBreak(); }
// CEF_EXPORT void cef_unregister_internal_web_plugin() { DebugBreak(); }
// CEF_EXPORT void cef_force_web_plugin_shutdown() { DebugBreak(); }
// CEF_EXPORT void cef_register_web_plugin_crash() {DebugBreak();}
// CEF_EXPORT void cef_is_web_plugin_unstable() { DebugBreak(); }
// CEF_EXPORT void cef_currently_on() { DebugBreak(); }
// CEF_EXPORT void cef_post_task() { DebugBreak(); }
// CEF_EXPORT void cef_post_delayed_task() { DebugBreak(); }
// CEF_EXPORT void cef_register_extension() { DebugBreak(); }
// CEF_EXPORT void cef_register_scheme_handler_factory() { DebugBreak(); }
// CEF_EXPORT void cef_clear_scheme_handler_factories() { DebugBreak(); }
// CEF_EXPORT void cef_execute_process() {DebugBreak();}
// CEF_EXPORT void cef_initialize() {DebugBreak();}
// CEF_EXPORT void cef_shutdown() {DebugBreak();}
// CEF_EXPORT void cef_do_message_loop_work() {DebugBreak();}
// CEF_EXPORT void cef_run_message_loop() {DebugBreak();}
// CEF_EXPORT void cef_quit_message_loop() {DebugBreak();}
// CEF_EXPORT void cef_set_osmodal_loop() {DebugBreak();}
// CEF_EXPORT void cef_enable_highdpi_support() {DebugBreak();}
// CEF_EXPORT void cef_get_geolocation() {DebugBreak();}
// CEF_EXPORT void cef_add_cross_origin_whitelist_entry() { DebugBreak(); }
// CEF_EXPORT void cef_remove_cross_origin_whitelist_entry() { DebugBreak(); }
// CEF_EXPORT void cef_clear_cross_origin_whitelist() { DebugBreak(); }
// CEF_EXPORT void cef_parse_url() { DebugBreak(); }
// CEF_EXPORT void cef_create_url() { DebugBreak(); }
// CEF_EXPORT void cef_format_url_for_security_display() { DebugBreak(); }
// CEF_EXPORT void cef_get_mime_type() { DebugBreak(); }
// CEF_EXPORT void cef_get_extensions_for_mime_type() { DebugBreak(); }
// CEF_EXPORT void cef_base64encode() { DebugBreak(); }
// CEF_EXPORT void cef_base64decode() { DebugBreak(); }
// CEF_EXPORT void cef_uriencode() { DebugBreak(); }
// CEF_EXPORT void cef_uridecode() { DebugBreak(); }
// CEF_EXPORT void cef_parse_csscolor() { DebugBreak(); }
// CEF_EXPORT void cef_parse_json() { DebugBreak(); }
// CEF_EXPORT void cef_parse_jsonand_return_error() { DebugBreak(); }
// CEF_EXPORT void cef_write_json() { DebugBreak(); }
// CEF_EXPORT void cef_get_path() { DebugBreak(); }
// CEF_EXPORT void cef_launch_process() { DebugBreak(); }
// CEF_EXPORT void cef_begin_tracing() { DebugBreak(); }
// CEF_EXPORT void cef_end_tracing() { DebugBreak(); }
*/

CEF_EXPORT void cef_string_map_size() { DebugBreak(); }
CEF_EXPORT void cef_string_map_key() { DebugBreak(); }
CEF_EXPORT void cef_string_map_value() { DebugBreak(); }
CEF_EXPORT void cef_string_map_append() { DebugBreak(); }
CEF_EXPORT void cef_string_map_clear() { DebugBreak(); }
CEF_EXPORT void cef_string_map_alloc() { DebugBreak(); }
CEF_EXPORT void cef_string_map_free() { DebugBreak(); }
// CEF_EXPORT void cef_string_multimap_size() { DebugBreak(); }
// CEF_EXPORT void cef_string_multimap_key() { DebugBreak(); }
// CEF_EXPORT void cef_string_multimap_value() { DebugBreak(); }
// CEF_EXPORT void cef_string_multimap_append() { DebugBreak(); }
// CEF_EXPORT void cef_string_multimap_alloc() { DebugBreak(); }
// CEF_EXPORT void cef_string_multimap_clear() { DebugBreak(); }
// CEF_EXPORT void cef_string_multimap_free() { DebugBreak(); }

typedef DWORD cef_platform_thread_id_t;
CEF_EXPORT cef_platform_thread_id_t cef_get_current_platform_thread_id() { return ::GetCurrentThreadId(); }