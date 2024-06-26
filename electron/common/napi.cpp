
#include <windows.h>

#define GEN_NAPI_FUNC_0(name)                                  \
    __declspec(dllexport) int name()                           \
    {                                                          \
        OutputDebugStringA(#name "\n");                        \
        typedef int(__cdecl * FN_##name)();                    \
        static FN_##name p_##name = nullptr;                   \
                                                               \
        if (!p_##name) {                                       \
            HMODULE hMod = LoadLibraryW(L"node.dll");          \
            p_##name = (FN_##name)GetProcAddress(hMod, #name); \
        }                                                      \
        return p_##name();                                     \
    }

#define GEN_NAPI_FUNC_1(name)                                  \
    __declspec(dllexport) int name(int a1)                     \
    {                                                          \
        OutputDebugStringA(#name "\n");                        \
        typedef int(__cdecl * FN_##name)(int a1);              \
        static FN_##name p_##name = nullptr;                   \
                                                               \
        if (!p_##name) {                                       \
            HMODULE hMod = LoadLibraryW(L"node.dll");          \
            p_##name = (FN_##name)GetProcAddress(hMod, #name); \
        }                                                      \
        return p_##name(a1);                                   \
    }

#define GEN_NAPI_FUNC_2(name)                                  \
    __declspec(dllexport) int name(int a1, int a2)             \
    {                                                          \
        OutputDebugStringA(#name "\n");                        \
        typedef int(__cdecl * FN_##name)(int a1, int a2);      \
        static FN_##name p_##name = nullptr;                   \
                                                               \
        if (!p_##name) {                                       \
            HMODULE hMod = LoadLibraryW(L"node.dll");          \
            p_##name = (FN_##name)GetProcAddress(hMod, #name); \
        }                                                      \
        return p_##name(a1, a2);                               \
    }

#define GEN_NAPI_FUNC_3(name)                                     \
    __declspec(dllexport) int name(int a1, int a2, int a3)        \
    {                                                             \
        OutputDebugStringA(#name "\n");                           \
        typedef int(__cdecl * FN_##name)(int a1, int a2, int a3); \
        static FN_##name p_##name = nullptr;                      \
                                                                  \
        if (!p_##name) {                                          \
            HMODULE hMod = LoadLibraryW(L"node.dll");             \
            p_##name = (FN_##name)GetProcAddress(hMod, #name);    \
        }                                                         \
        return p_##name(a1, a2, a3);                              \
    }

#define GEN_NAPI_FUNC_4(name)                                             \
    __declspec(dllexport) int name(int a1, int a2, int a3, int a4)        \
    {                                                                     \
        OutputDebugStringA(#name "\n");                                   \
        typedef int(__cdecl * FN_##name)(int a1, int a2, int a3, int a4); \
        static FN_##name p_##name = nullptr;                              \
                                                                          \
        if (!p_##name) {                                                  \
            HMODULE hMod = LoadLibraryW(L"node.dll");                     \
            p_##name = (FN_##name)GetProcAddress(hMod, #name);            \
        }                                                                 \
        return p_##name(a1, a2, a3, a4);                                  \
    }

#define GEN_NAPI_FUNC_5(name)                                                     \
    __declspec(dllexport) int name(int a1, int a2, int a3, int a4, int a5)        \
    {                                                                             \
        OutputDebugStringA(#name "\n");                                           \
        typedef int(__cdecl * FN_##name)(int a1, int a2, int a3, int a4, int a5); \
        static FN_##name p_##name = nullptr;                                      \
                                                                                  \
        if (!p_##name) {                                                          \
            HMODULE hMod = LoadLibraryW(L"node.dll");                             \
            p_##name = (FN_##name)GetProcAddress(hMod, #name);                    \
        }                                                                         \
        return p_##name(a1, a2, a3, a4, a5);                                      \
    }

#define GEN_NAPI_FUNC_6(name)                                                             \
    __declspec(dllexport) int name(int a1, int a2, int a3, int a4, int a5, int a6)        \
    {                                                                                     \
        OutputDebugStringA(#name "\n");                                                   \
        typedef int(__cdecl * FN_##name)(int a1, int a2, int a3, int a4, int a5, int a6); \
        static FN_##name p_##name = nullptr;                                              \
                                                                                          \
        if (!p_##name) {                                                                  \
            HMODULE hMod = LoadLibraryW(L"node.dll");                                     \
            p_##name = (FN_##name)GetProcAddress(hMod, #name);                            \
        }                                                                                 \
        return p_##name(a1, a2, a3, a4, a5, a6);                                          \
    }

#define GEN_NAPI_FUNC_7(name)                                                                     \
    __declspec(dllexport) int name(int a1, int a2, int a3, int a4, int a5, int a6, int a7)        \
    {                                                                                             \
        OutputDebugStringA(#name "\n");                                                           \
        typedef int(__cdecl * FN_##name)(int a1, int a2, int a3, int a4, int a5, int a6, int a7); \
        static FN_##name p_##name = nullptr;                                                      \
                                                                                                  \
        if (!p_##name) {                                                                          \
            HMODULE hMod = LoadLibraryW(L"node.dll");                                             \
            p_##name = (FN_##name)GetProcAddress(hMod, #name);                                    \
        }                                                                                         \
        return p_##name(a1, a2, a3, a4, a5, a6, a7);                                              \
    }

#define GEN_NAPI_FUNC_8(name)                                                                             \
    __declspec(dllexport) int name(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)        \
    {                                                                                                     \
        OutputDebugStringA(#name "\n");                                                                   \
        typedef int(__cdecl * FN_##name)(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8); \
        static FN_##name p_##name = nullptr;                                                              \
                                                                                                          \
        if (!p_##name) {                                                                                  \
            HMODULE hMod = LoadLibraryW(L"node.dll");                                                     \
            p_##name = (FN_##name)GetProcAddress(hMod, #name);                                            \
        }                                                                                                 \
        return p_##name(a1, a2, a3, a4, a5, a6, a7, a8);                                                  \
    }

#define GEN_NAPI_FUNC_11(name)                                                                                                      \
    __declspec(dllexport) int name(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11)        \
    {                                                                                                                               \
        OutputDebugStringA(#name "\n");                                                                                             \
        typedef int(__cdecl * FN_##name)(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11); \
        static FN_##name p_##name = nullptr;                                                                                        \
                                                                                                                                    \
        if (!p_##name) {                                                                                                            \
            HMODULE hMod = LoadLibraryW(L"node.dll");                                                                               \
            p_##name = (FN_##name)GetProcAddress(hMod, #name);                                                                      \
        }                                                                                                                           \
        return p_##name(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);                                                              \
    }

extern "C" {

//     __declspec(dllexport) int napi_create_int64(int a1, int64_t a2, int a3)
//     {
//         typedef int(__cdecl *FN_napi_create_int64)(int a1, int a2, int a3);
//         static FN_napi_create_int64 p_napi_reference_ref = nullptr;
//
//         if (!p_napi_reference_ref) {
//             HMODULE hMod = LoadLibraryW("node.dll");
//             p_napi_reference_ref = (FN_napi_reference_ref)GetProcAddress(hMod, "napi_reference_ref");
//         }
//         return p_napi_reference_ref(a1, a2, a3);
//     }
GEN_NAPI_FUNC_1(napi_module_register);
GEN_NAPI_FUNC_2(napi_get_last_error_info);
GEN_NAPI_FUNC_4(napi_fatal_error);
GEN_NAPI_FUNC_2(napi_get_undefined);
GEN_NAPI_FUNC_2(napi_get_null);

GEN_NAPI_FUNC_2(napi_get_global);
GEN_NAPI_FUNC_3(napi_get_boolean);
GEN_NAPI_FUNC_2(napi_create_object);
GEN_NAPI_FUNC_2(napi_create_array);
GEN_NAPI_FUNC_3(napi_create_array_with_length);
GEN_NAPI_FUNC_3(napi_create_double);
GEN_NAPI_FUNC_3(napi_create_int32);
GEN_NAPI_FUNC_3(napi_create_uint32);
GEN_NAPI_FUNC_4(napi_create_int64); // TODO
GEN_NAPI_FUNC_4(napi_create_string_latin1);
GEN_NAPI_FUNC_4(napi_create_string_utf8);
GEN_NAPI_FUNC_4(napi_create_string_utf16);
GEN_NAPI_FUNC_3(napi_create_symbol);
GEN_NAPI_FUNC_6(napi_create_function);
GEN_NAPI_FUNC_4(napi_create_error);
GEN_NAPI_FUNC_4(napi_create_type_error);
GEN_NAPI_FUNC_4(napi_create_range_error);
GEN_NAPI_FUNC_3(napi_typeof);
GEN_NAPI_FUNC_3(napi_get_value_double);
GEN_NAPI_FUNC_3(napi_get_value_int32);
GEN_NAPI_FUNC_3(napi_get_value_uint32);
GEN_NAPI_FUNC_4(napi_get_value_int64); // TODO
GEN_NAPI_FUNC_3(napi_get_value_bool);
GEN_NAPI_FUNC_5(napi_get_value_string_latin1);
GEN_NAPI_FUNC_5(napi_get_value_string_utf8);
GEN_NAPI_FUNC_5(napi_get_value_string_utf16);
GEN_NAPI_FUNC_3(napi_coerce_to_bool);
GEN_NAPI_FUNC_3(napi_coerce_to_number);
GEN_NAPI_FUNC_3(napi_coerce_to_object);
GEN_NAPI_FUNC_3(napi_coerce_to_string);
GEN_NAPI_FUNC_3(napi_get_prototype);
GEN_NAPI_FUNC_3(napi_get_property_names);
GEN_NAPI_FUNC_4(napi_set_property);
GEN_NAPI_FUNC_4(napi_has_property);
GEN_NAPI_FUNC_4(napi_get_property);
GEN_NAPI_FUNC_4(napi_delete_property);
GEN_NAPI_FUNC_4(napi_has_own_property);
GEN_NAPI_FUNC_4(napi_set_named_property);
GEN_NAPI_FUNC_4(napi_has_named_property);
GEN_NAPI_FUNC_4(napi_get_named_property);
GEN_NAPI_FUNC_4(napi_set_element);
GEN_NAPI_FUNC_4(napi_has_element);
GEN_NAPI_FUNC_4(napi_get_element);
GEN_NAPI_FUNC_4(napi_delete_element);
GEN_NAPI_FUNC_4(napi_define_properties);
GEN_NAPI_FUNC_3(napi_is_array);
GEN_NAPI_FUNC_3(napi_get_array_length);
GEN_NAPI_FUNC_4(napi_strict_equals);
GEN_NAPI_FUNC_6(napi_call_function);
GEN_NAPI_FUNC_5(napi_new_instance);
GEN_NAPI_FUNC_4(napi_instanceof);
GEN_NAPI_FUNC_6(napi_get_cb_info);
GEN_NAPI_FUNC_3(napi_get_new_target);
GEN_NAPI_FUNC_8(napi_define_class);
GEN_NAPI_FUNC_6(napi_wrap);
GEN_NAPI_FUNC_3(napi_unwrap);
GEN_NAPI_FUNC_3(napi_remove_wrap);
GEN_NAPI_FUNC_5(napi_create_external);
GEN_NAPI_FUNC_3(napi_get_value_external);
GEN_NAPI_FUNC_4(napi_create_reference);
GEN_NAPI_FUNC_2(napi_delete_reference);
GEN_NAPI_FUNC_3(napi_reference_ref);
GEN_NAPI_FUNC_3(napi_reference_unref);
GEN_NAPI_FUNC_3(napi_get_reference_value);
GEN_NAPI_FUNC_2(napi_open_handle_scope);
GEN_NAPI_FUNC_2(napi_close_handle_scope);
GEN_NAPI_FUNC_2(napi_open_escapable_handle_scope);
GEN_NAPI_FUNC_2(napi_close_escapable_handle_scope);
GEN_NAPI_FUNC_4(napi_escape_handle);
GEN_NAPI_FUNC_2(napi_throw);
GEN_NAPI_FUNC_2(napi_throw_error);
GEN_NAPI_FUNC_3(napi_throw_type_error);
GEN_NAPI_FUNC_3(napi_throw_range_error);
GEN_NAPI_FUNC_3(napi_is_error);
GEN_NAPI_FUNC_2(napi_is_exception_pending);
GEN_NAPI_FUNC_2(napi_get_and_clear_last_exception);
GEN_NAPI_FUNC_4(napi_create_buffer);
GEN_NAPI_FUNC_6(napi_create_external_buffer);
GEN_NAPI_FUNC_3(napi_is_buffer);
GEN_NAPI_FUNC_4(napi_get_buffer_info);
GEN_NAPI_FUNC_3(napi_is_arraybuffer);
GEN_NAPI_FUNC_4(napi_create_arraybuffer);
GEN_NAPI_FUNC_6(napi_create_external_arraybuffer);
GEN_NAPI_FUNC_4(napi_get_arraybuffer_info);
GEN_NAPI_FUNC_3(napi_is_typedarray);
GEN_NAPI_FUNC_6(napi_create_typedarray);
GEN_NAPI_FUNC_7(napi_get_typedarray_info);
GEN_NAPI_FUNC_5(napi_create_dataview);
GEN_NAPI_FUNC_3(napi_is_dataview);
GEN_NAPI_FUNC_6(napi_get_dataview_info);
GEN_NAPI_FUNC_7(napi_create_async_work);
GEN_NAPI_FUNC_2(napi_delete_async_work);
GEN_NAPI_FUNC_2(napi_queue_async_work);
GEN_NAPI_FUNC_2(napi_cancel_async_work);
GEN_NAPI_FUNC_4(napi_async_init);
GEN_NAPI_FUNC_2(napi_async_destroy);
GEN_NAPI_FUNC_7(napi_make_callback);
GEN_NAPI_FUNC_2(napi_get_version);
GEN_NAPI_FUNC_2(napi_get_node_version);
GEN_NAPI_FUNC_3(napi_create_promise);
GEN_NAPI_FUNC_3(napi_resolve_deferred);
GEN_NAPI_FUNC_3(napi_reject_deferred);
GEN_NAPI_FUNC_3(napi_is_promise);
GEN_NAPI_FUNC_3(napi_adjust_external_memory);
GEN_NAPI_FUNC_3(napi_run_script);
GEN_NAPI_FUNC_3(napi_get_uv_event_loop);
GEN_NAPI_FUNC_4(napi_open_callback_scope);
GEN_NAPI_FUNC_2(napi_close_callback_scope);
GEN_NAPI_FUNC_2(napi_fatal_exception);
GEN_NAPI_FUNC_3(napi_add_env_cleanup_hook);
GEN_NAPI_FUNC_3(napi_remove_env_cleanup_hook);
GEN_NAPI_FUNC_11(napi_create_threadsafe_function);
GEN_NAPI_FUNC_2(napi_get_threadsafe_function_context);
GEN_NAPI_FUNC_3(napi_call_threadsafe_function);
GEN_NAPI_FUNC_1(napi_acquire_threadsafe_function);
GEN_NAPI_FUNC_2(napi_release_threadsafe_function);
GEN_NAPI_FUNC_2(napi_unref_threadsafe_function);
GEN_NAPI_FUNC_2(napi_ref_threadsafe_function);
GEN_NAPI_FUNC_4(napi_create_bigint_int64);
GEN_NAPI_FUNC_4(napi_create_bigint_uint64);
GEN_NAPI_FUNC_5(napi_create_bigint_words);
GEN_NAPI_FUNC_5(napi_get_value_bigint_int64);
GEN_NAPI_FUNC_5(napi_get_value_bigint_uint64);
GEN_NAPI_FUNC_5(napi_get_value_bigint_words);
GEN_NAPI_FUNC_6(napi_add_finalizer);
}