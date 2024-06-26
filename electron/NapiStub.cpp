
#include <windows.h>

extern HMODULE g_hMiniblinkMod;

void initNapiStub()
{
    //g_hMiniblinkMod = LoadLibraryW(L"node.dll");
    // ¼ûG:\mycode\mb\electron\electron.cpp
}

// extern "C" __declspec(dllexport) DWORD __cdecl napi_module_register(DWORD p0)
// {
//     typedef DWORD(__cdecl* napi_module_registerFn)(DWORD p0);
//     static napi_module_registerFn napi_module_registerPtr = nullptr;
//     if (!napi_module_registerPtr)
//         napi_module_registerPtr = (napi_module_registerFn)GetProcAddress(g_hMiniblinkMod, "napi_module_register");
//     return napi_module_registerPtr(p0);
// }

#define DEFINE_FUN3_EX(name, p1, p2, p3)                                     \
    extern "C" __declspec(dllexport) DWORD __cdecl name(p1 v1, p2 v2, p3 v3) \
    {                                                                        \
        typedef DWORD(__cdecl* name##FN)(p1, p2, p3);                        \
        static name##FN funcPtr = nullptr;                                   \
        if (!funcPtr)                                                        \
            funcPtr = (name##FN)GetProcAddress(g_hMiniblinkMod, #name);               \
        return funcPtr(v1, v2, v3);                                          \
    }

#define DEFINE_FUN0(name)                                      \
    extern "C" __declspec(dllexport) DWORD __cdecl name()      \
    {                                                          \
        typedef DWORD(__cdecl* name##FN)();                    \
        static name##FN funcPtr = nullptr;                     \
        if (!funcPtr)                                          \
            funcPtr = (name##FN)GetProcAddress(g_hMiniblinkMod, #name); \
        return funcPtr();                                      \
    }

#define DEFINE_FUN1(name)                                         \
    extern "C" __declspec(dllexport) DWORD __cdecl name(DWORD p1) \
    {                                                             \
        typedef DWORD(__cdecl* name##FN)(DWORD p1);               \
        static name##FN funcPtr = nullptr;                        \
        if (!funcPtr)                                             \
            funcPtr = (name##FN)GetProcAddress(g_hMiniblinkMod, #name);    \
        return funcPtr(p1);                                       \
    }

#define DEFINE_FUN2(name)                                                   \
    extern "C" __declspec(dllexport) DWORD __cdecl name(DWORD p1, DWORD p2) \
    {                                                                       \
        typedef DWORD(__cdecl* name##FN)(DWORD p1, DWORD p2);               \
        static name##FN funcPtr = nullptr;                                  \
        if (!funcPtr)                                                       \
            funcPtr = (name##FN)GetProcAddress(g_hMiniblinkMod, #name);              \
        return funcPtr(p1, p2);                                             \
    }

#define DEFINE_FUN3(name)                                                             \
    extern "C" __declspec(dllexport) DWORD __cdecl name(DWORD p1, DWORD p2, DWORD p3) \
    {                                                                                 \
        typedef DWORD(__cdecl* name##FN)(DWORD p1, DWORD p2, DWORD p3);               \
        static name##FN funcPtr = nullptr;                                            \
        if (!funcPtr)                                                                 \
            funcPtr = (name##FN)GetProcAddress(g_hMiniblinkMod, #name);                        \
        return funcPtr(p1, p2, p3);                                                   \
    }

#define DEFINE_FUN4(name)                                                                       \
    extern "C" __declspec(dllexport) DWORD __cdecl name(DWORD p1, DWORD p2, DWORD p3, DWORD p4) \
    {                                                                                           \
        typedef DWORD(__cdecl* name##FN)(DWORD p1, DWORD p2, DWORD p3, DWORD p4);               \
        static name##FN funcPtr = nullptr;                                                      \
        if (!funcPtr)                                                                           \
            funcPtr = (name##FN)GetProcAddress(g_hMiniblinkMod, #name);                                  \
        return funcPtr(p1, p2, p3, p4);                                                         \
    }

#define DEFINE_FUN5(name)                                                                                 \
    extern "C" __declspec(dllexport) DWORD __cdecl name(DWORD p1, DWORD p2, DWORD p3, DWORD p4, DWORD p5) \
    {                                                                                                     \
        typedef DWORD(__cdecl* name##FN)(DWORD p1, DWORD p2, DWORD p3, DWORD p4, DWORD p5);               \
        static name##FN funcPtr = nullptr;                                                                \
        if (!funcPtr)                                                                                     \
            funcPtr = (name##FN)GetProcAddress(g_hMiniblinkMod, #name);                                            \
        return funcPtr(p1, p2, p3, p4, p5);                                                               \
    }

#define DEFINE_FUN6(name)                                                                                           \
    extern "C" __declspec(dllexport) DWORD __cdecl name(DWORD p1, DWORD p2, DWORD p3, DWORD p4, DWORD p5, DWORD p6) \
    {                                                                                                               \
        typedef DWORD(__cdecl* name##FN)(DWORD p1, DWORD p2, DWORD p3, DWORD p4, DWORD p5, DWORD p6);               \
        static name##FN funcPtr = nullptr;                                                                          \
        if (!funcPtr)                                                                                               \
            funcPtr = (name##FN)GetProcAddress(g_hMiniblinkMod, #name);                                                      \
        return funcPtr(p1, p2, p3, p4, p5, p6);                                                                     \
    }

#define DEFINE_FUN7(name)                                                                                                     \
    extern "C" __declspec(dllexport) DWORD __cdecl name(DWORD p1, DWORD p2, DWORD p3, DWORD p4, DWORD p5, DWORD p6, DWORD p7) \
    {                                                                                                                         \
        typedef DWORD(__cdecl* name##FN)(DWORD p1, DWORD p2, DWORD p3, DWORD p4, DWORD p5, DWORD p6, DWORD p7);               \
        static name##FN funcPtr = nullptr;                                                                                    \
        if (!funcPtr)                                                                                                         \
            funcPtr = (name##FN)GetProcAddress(g_hMiniblinkMod, #name);                                                                \
        return funcPtr(p1, p2, p3, p4, p5, p6, p7);                                                                           \
    }

#define DEFINE_FUN8(name)                                                                                                               \
    extern "C" __declspec(dllexport) DWORD __cdecl name(DWORD p1, DWORD p2, DWORD p3, DWORD p4, DWORD p5, DWORD p6, DWORD p7, DWORD p8) \
    {                                                                                                                                   \
        typedef DWORD(__cdecl* name##FN)(DWORD p1, DWORD p2, DWORD p3, DWORD p4, DWORD p5, DWORD p6, DWORD p7, DWORD p8);               \
        static name##FN funcPtr = nullptr;                                                                                              \
        if (!funcPtr)                                                                                                                   \
            funcPtr = (name##FN)GetProcAddress(g_hMiniblinkMod, #name);                                                                          \
        return funcPtr(p1, p2, p3, p4, p5, p6, p7, p8);                                                                                 \
    }

#define DEFINE_FUN_EX(ret, name, dp, p)                        \
    extern "C" __declspec(dllexport) ret __cdecl name dp       \
    {                                                          \
        typedef ret(__cdecl* name##FN) dp;                     \
        static name##FN funcPtr = nullptr;                     \
        if (!funcPtr)                                          \
            funcPtr = (name##FN)GetProcAddress(g_hMiniblinkMod, #name); \
        return funcPtr p;                                      \
    }

DEFINE_FUN1(napi_module_register)
DEFINE_FUN2(napi_get_last_error_info)
DEFINE_FUN4(napi_fatal_error)
DEFINE_FUN2(napi_get_undefined)
DEFINE_FUN2(napi_get_null)
DEFINE_FUN2(napi_get_global)
DEFINE_FUN3(napi_get_boolean)
DEFINE_FUN2(napi_create_object)
DEFINE_FUN2(napi_create_array)
DEFINE_FUN3(napi_create_array_with_length)
DEFINE_FUN3_EX(napi_create_double, void*, double, void*)
DEFINE_FUN3(napi_create_int32)
DEFINE_FUN3(napi_create_uint32)
DEFINE_FUN3_EX(napi_create_int64, void*, __int64, void*)
DEFINE_FUN4(napi_create_string_latin1)
DEFINE_FUN4(napi_create_string_utf8)
DEFINE_FUN4(napi_create_string_utf16)
DEFINE_FUN3(napi_create_symbol)
DEFINE_FUN6(napi_create_function)
DEFINE_FUN4(napi_create_error)
DEFINE_FUN4(napi_create_type_error)
DEFINE_FUN4(napi_create_range_error)

DEFINE_FUN3(napi_typeof)
DEFINE_FUN3(napi_get_value_double)
DEFINE_FUN3(napi_get_value_int32)
DEFINE_FUN3(napi_get_value_uint32)
DEFINE_FUN3(napi_get_value_int64)
DEFINE_FUN3(napi_get_value_bool)
DEFINE_FUN5(napi_get_value_string_latin1)
DEFINE_FUN5(napi_get_value_string_utf8)
DEFINE_FUN1(napi_get_value_string_utf16)
DEFINE_FUN3(napi_coerce_to_bool)
DEFINE_FUN3(napi_coerce_to_number)
DEFINE_FUN3(napi_coerce_to_object)
DEFINE_FUN3(napi_coerce_to_string)

DEFINE_FUN3(napi_get_prototype)
DEFINE_FUN3(napi_get_property_names)
DEFINE_FUN4(napi_set_property)
DEFINE_FUN4(napi_has_property)
DEFINE_FUN4(napi_get_property)
DEFINE_FUN4(napi_delete_property)
DEFINE_FUN4(napi_has_own_property)
DEFINE_FUN4(napi_set_named_property)
DEFINE_FUN4(napi_has_named_property)
DEFINE_FUN4(napi_get_named_property)
DEFINE_FUN4(napi_set_element)
DEFINE_FUN4(napi_has_element)
DEFINE_FUN4(napi_get_element)
DEFINE_FUN4(napi_delete_element)

DEFINE_FUN4(napi_define_properties)
DEFINE_FUN3(napi_is_array)
DEFINE_FUN3(napi_get_array_length)
DEFINE_FUN4(napi_strict_equals)
DEFINE_FUN6(napi_call_function)
DEFINE_FUN5(napi_new_instance)
DEFINE_FUN4(napi_instanceof)

DEFINE_FUN6(napi_get_cb_info)
DEFINE_FUN3(napi_get_new_target)
DEFINE_FUN8(napi_define_class)
DEFINE_FUN6(napi_wrap)
DEFINE_FUN3(napi_unwrap)
DEFINE_FUN3(napi_remove_wrap)
DEFINE_FUN5(napi_create_external)
DEFINE_FUN3(napi_get_value_external)
DEFINE_FUN4(napi_create_reference)
DEFINE_FUN2(napi_delete_reference)
DEFINE_FUN3(napi_reference_ref)
DEFINE_FUN3(napi_reference_unref)
DEFINE_FUN3(napi_get_reference_value)

DEFINE_FUN2(napi_open_handle_scope)
DEFINE_FUN2(napi_close_handle_scope)
DEFINE_FUN2(napi_open_escapable_handle_scope)
DEFINE_FUN2(napi_close_escapable_handle_scope)
DEFINE_FUN4(napi_escape_handle)
DEFINE_FUN2(napi_throw)
DEFINE_FUN3(napi_throw_error)
DEFINE_FUN3(napi_throw_type_error)
DEFINE_FUN3(napi_throw_range_error)
DEFINE_FUN3(napi_is_error)

DEFINE_FUN6(napi_add_finalizer)

DEFINE_FUN2(napi_is_exception_pending)
DEFINE_FUN2(napi_get_and_clear_last_exception)
DEFINE_FUN4(napi_create_buffer)
DEFINE_FUN6(napi_create_external_buffer)
DEFINE_FUN5(napi_create_buffer_copy)
DEFINE_FUN3(napi_is_buffer)
DEFINE_FUN4(napi_get_buffer_info)

DEFINE_FUN3(napi_is_arraybuffer)
DEFINE_FUN4(napi_create_arraybuffer)

DEFINE_FUN6(napi_create_external_arraybuffer)
DEFINE_FUN4(napi_get_arraybuffer_info)
DEFINE_FUN3(napi_is_typedarray)
DEFINE_FUN6(napi_create_typedarray)
DEFINE_FUN7(napi_get_typedarray_info)

DEFINE_FUN5(napi_create_dataview)
DEFINE_FUN3(napi_is_dataview)
DEFINE_FUN6(napi_get_dataview_info)

DEFINE_FUN7(napi_create_async_work)
DEFINE_FUN2(napi_delete_async_work)
DEFINE_FUN2(napi_queue_async_work)
DEFINE_FUN2(napi_cancel_async_work)

DEFINE_FUN4(napi_async_init)
DEFINE_FUN2(napi_async_destroy)
DEFINE_FUN7(napi_make_callback)

DEFINE_FUN2(napi_get_version)

DEFINE_FUN3(napi_get_node_version)

DEFINE_FUN1(napi_create_promise)
DEFINE_FUN3(napi_resolve_deferred)
DEFINE_FUN3(napi_reject_deferred)
DEFINE_FUN3(napi_is_promise)

DEFINE_FUN3_EX(napi_adjust_external_memory, void*, __int64, __int64*)
DEFINE_FUN3(napi_run_script)

DEFINE_FUN2(napi_get_uv_event_loop)

DEFINE_FUN4(napi_open_callback_scope)

DEFINE_FUN2(napi_close_callback_scope)

DEFINE_FUN2(napi_fatal_exception)

DEFINE_FUN3(napi_add_env_cleanup_hook)
DEFINE_FUN3(napi_remove_env_cleanup_hook)

DEFINE_FUN4(napi_set_instance_data)
DEFINE_FUN2(napi_get_instance_data)

//////////////////////////////////////////////////////////////////////////
DEFINE_FUN0(uv_version)
DEFINE_FUN0(uv_version_string)
DEFINE_FUN4(uv_replace_allocator)

DEFINE_FUN0(uv_default_loop)
DEFINE_FUN1(uv_loop_init)
DEFINE_FUN1(uv_loop_close)

DEFINE_FUN0(uv_loop_new)

DEFINE_FUN1(uv_loop_delete)
DEFINE_FUN0(uv_loop_size)
DEFINE_FUN1(uv_loop_alive)

DEFINE_FUN2(uv_run)
DEFINE_FUN1(uv_stop)

DEFINE_FUN1(uv_ref)
DEFINE_FUN1(uv_unref)
DEFINE_FUN1(uv_has_ref)

DEFINE_FUN1(uv_update_time)
DEFINE_FUN_EX(__int64, uv_now, (void* p1), (p1))
DEFINE_FUN1(uv_backend_fd)
DEFINE_FUN1(uv_backend_timeout)

DEFINE_FUN1(uv_strerror)
DEFINE_FUN1(uv_err_name)
DEFINE_FUN3(uv_shutdown)

DEFINE_FUN1(uv_handle_size)
DEFINE_FUN1(uv_req_size)

DEFINE_FUN1(uv_is_active)

DEFINE_FUN3(uv_walk)

DEFINE_FUN2(uv_print_all_handles)
DEFINE_FUN2(uv_print_active_handles)

DEFINE_FUN2(uv_close)

DEFINE_FUN2(uv_send_buffer_size)
DEFINE_FUN2(uv_recv_buffer_size)
DEFINE_FUN2(uv_fileno)
DEFINE_FUN2(uv_buf_init)

DEFINE_FUN3(uv_listen)
DEFINE_FUN2(uv_accept)

DEFINE_FUN3(uv_read_start)
DEFINE_FUN3(uv_read_stop)
DEFINE_FUN5(uv_write)
DEFINE_FUN6(uv_write2)
DEFINE_FUN3(uv_try_write)

DEFINE_FUN1(uv_is_readable)
DEFINE_FUN1(uv_is_writable)

DEFINE_FUN2(uv_stream_set_blocking)

DEFINE_FUN1(uv_is_closing)

DEFINE_FUN2(uv_tcp_init)
DEFINE_FUN3(uv_tcp_init_ex)
DEFINE_FUN2(uv_tcp_open)
DEFINE_FUN2(uv_tcp_nodelay)
DEFINE_FUN3(uv_tcp_keepalive)
DEFINE_FUN2(uv_tcp_simultaneous_accepts)

DEFINE_FUN3(uv_tcp_bind)
DEFINE_FUN3(uv_tcp_getsockname)
DEFINE_FUN3(uv_tcp_getpeername)
DEFINE_FUN4(uv_tcp_connect)

DEFINE_FUN2(uv_udp_init)
DEFINE_FUN3(uv_udp_init_ex)
DEFINE_FUN2(uv_udp_open)
DEFINE_FUN3(uv_udp_bind)

DEFINE_FUN3(uv_udp_getsockname)
DEFINE_FUN4(uv_udp_set_membership)
DEFINE_FUN2(uv_udp_set_multicast_loop)
DEFINE_FUN2(uv_udp_set_multicast_ttl)
DEFINE_FUN2(uv_udp_set_multicast_interface)
DEFINE_FUN2(uv_udp_set_broadcast)
DEFINE_FUN2(uv_udp_set_ttl)
DEFINE_FUN6(uv_udp_send)
DEFINE_FUN4(uv_udp_try_send)
DEFINE_FUN3(uv_udp_recv_start)
DEFINE_FUN1(uv_udp_recv_stop)

DEFINE_FUN4(uv_tty_init)
DEFINE_FUN2(uv_tty_set_mode)
DEFINE_FUN0(uv_tty_reset_mode)
DEFINE_FUN3(uv_tty_get_winsize)

DEFINE_FUN1(uv_guess_handle)

DEFINE_FUN3(uv_pipe_init)
DEFINE_FUN2(uv_pipe_open)
DEFINE_FUN2(uv_pipe_bind)
DEFINE_FUN4(uv_pipe_connect)
DEFINE_FUN3(uv_pipe_getsockname)
DEFINE_FUN3(uv_pipe_getpeername)
DEFINE_FUN2(uv_pipe_pending_instances)
DEFINE_FUN1(uv_pipe_pending_count)
DEFINE_FUN1(uv_pipe_pending_type)
DEFINE_FUN3(uv_poll_init)
DEFINE_FUN3(uv_poll_init_socket)
DEFINE_FUN3(uv_poll_start)
DEFINE_FUN1(uv_poll_stop)

DEFINE_FUN2(uv_prepare_init)
DEFINE_FUN2(uv_prepare_start)
DEFINE_FUN1(uv_prepare_stop)

DEFINE_FUN2(uv_check_init)
DEFINE_FUN2(uv_check_start)
DEFINE_FUN1(uv_check_stop)

DEFINE_FUN2(uv_idle_init)
DEFINE_FUN2(uv_idle_start)
DEFINE_FUN1(uv_idle_stop)

DEFINE_FUN3(uv_async_init)
DEFINE_FUN1(uv_async_send)

DEFINE_FUN2(uv_timer_init)
DEFINE_FUN_EX(int, uv_timer_start, (void* p1, void* p2, __int64 p3, __int64 p4), (p1, p2, p3, p4))
DEFINE_FUN1(uv_timer_stop)
DEFINE_FUN1(uv_timer_again)
DEFINE_FUN_EX(void, uv_timer_set_repeat, (void* p1, __int64 p2), (p1, p2))
DEFINE_FUN_EX(__int64, uv_timer_get_repeat, (void* p1), (p1))

DEFINE_FUN6(uv_getaddrinfo)
DEFINE_FUN1(uv_freeaddrinfo)

DEFINE_FUN5(uv_getnameinfo)

DEFINE_FUN3(uv_spawn)
DEFINE_FUN2(uv_process_kill)
DEFINE_FUN2(uv_kill)

DEFINE_FUN4(uv_queue_work)
DEFINE_FUN1(uv_cancel)

DEFINE_FUN2(uv_setup_args)
DEFINE_FUN2(uv_get_process_title)
DEFINE_FUN1(uv_set_process_title)
DEFINE_FUN1(uv_resident_set_memory)
DEFINE_FUN1(uv_uptime)

DEFINE_FUN1(uv_getrusage)

DEFINE_FUN2(uv_os_homedir)
DEFINE_FUN2(uv_os_tmpdir)
DEFINE_FUN1(uv_os_get_passwd)
DEFINE_FUN1(uv_os_free_passwd)

DEFINE_FUN2(uv_cpu_info)
DEFINE_FUN2(uv_free_cpu_info)

DEFINE_FUN2(uv_interface_addresses)
DEFINE_FUN2(uv_free_interface_addresses)

DEFINE_FUN1(uv_fs_req_cleanup)
DEFINE_FUN4(uv_fs_close)
DEFINE_FUN6(uv_fs_open)
DEFINE_FUN_EX(int, uv_fs_read,
    (void* p1, void* p2, int p3, void* p4, int p5, __int64 p6, void* p7),
    (p1, p2, p3, p4, p5, p6, p7))
DEFINE_FUN4(uv_fs_unlink)
DEFINE_FUN_EX(int, uv_fs_write,
    (void* p1, void* p2, int p3, void* p4, int p5, __int64 p6, void* p7),
    (p1, p2, p3, p4, p5, p6, p7))
DEFINE_FUN5(uv_fs_mkdir)
DEFINE_FUN4(uv_fs_mkdtemp)
DEFINE_FUN4(uv_fs_rmdir)
DEFINE_FUN5(uv_fs_scandir)
DEFINE_FUN2(uv_fs_scandir_next)
DEFINE_FUN4(uv_fs_stat)
DEFINE_FUN4(uv_fs_fstat)
DEFINE_FUN5(uv_fs_rename)
DEFINE_FUN4(uv_fs_fsync)
DEFINE_FUN4(uv_fs_fdatasync)
DEFINE_FUN_EX(int, uv_fs_ftruncate, (void* p1, void* p2, void* p3, __int64 p4, void* p5),
    (p1, p2, p3, p4, p5))
DEFINE_FUN_EX(int, uv_fs_sendfile, (void* p1, void* p2, void* p3, void* p4, __int64 p5, DWORD p6, void* p7),
    (p1, p2, p3, p4, p5, p6, p7))

DEFINE_FUN5(uv_fs_access)
DEFINE_FUN5(uv_fs_chmod)
DEFINE_FUN_EX(int, uv_fs_utime, (void* p1, void* p2, int p3, double p4, double p5, void* p6), (p1, p2, p3, p4, p5, p6))
DEFINE_FUN_EX(int, uv_fs_futime, (void* p1, void* p2, int p3, double p4, double p5, void* p6), (p1, p2, p3, p4, p5, p6))
DEFINE_FUN4(uv_fs_lstat)
DEFINE_FUN5(uv_fs_link)

DEFINE_FUN6(uv_fs_symlink)
DEFINE_FUN4(uv_fs_readlink)
DEFINE_FUN4(uv_fs_realpath)
DEFINE_FUN5(uv_fs_fchmod)
DEFINE_FUN6(uv_fs_chown)
DEFINE_FUN6(uv_fs_fchown)

DEFINE_FUN2(uv_fs_poll_init)
DEFINE_FUN4(uv_fs_poll_start)
DEFINE_FUN1(uv_fs_poll_stop)
DEFINE_FUN3(uv_fs_poll_getpath)

DEFINE_FUN2(uv_signal_init)
DEFINE_FUN3(uv_signal_start)
DEFINE_FUN1(uv_signal_stop)
DEFINE_FUN1(uv_loadavg)

DEFINE_FUN2(uv_fs_event_init)
DEFINE_FUN4(uv_fs_event_start)
DEFINE_FUN1(uv_fs_event_stop)
DEFINE_FUN3(uv_fs_event_getpath)
DEFINE_FUN3(uv_ip4_addr)
DEFINE_FUN3(uv_ip6_addr)

DEFINE_FUN3(uv_ip4_name)
DEFINE_FUN3(uv_ip6_name)

DEFINE_FUN4(uv_inet_ntop)
DEFINE_FUN3(uv_inet_pton)

DEFINE_FUN2(uv_exepath)
DEFINE_FUN2(uv_cwd)
DEFINE_FUN2(uv_chdir)

DEFINE_FUN_EX(__int64, uv_get_free_memory, (void), ())
DEFINE_FUN_EX(__int64, uv_get_total_memory, (void), ())

DEFINE_FUN_EX(__int64, uv_hrtime, (void), ())
DEFINE_FUN0(uv_disable_stdio_inheritance)

DEFINE_FUN2(uv_dlopen)
DEFINE_FUN1(uv_dlclose)
DEFINE_FUN3(uv_dlsym)
DEFINE_FUN4(uv_dlerror)

DEFINE_FUN1(uv_mutex_init)
DEFINE_FUN1(uv_mutex_destroy)
DEFINE_FUN1(uv_mutex_lock)
DEFINE_FUN1(uv_mutex_trylock)
DEFINE_FUN1(uv_mutex_unlock)

DEFINE_FUN1(uv_rwlock_init)
DEFINE_FUN1(uv_rwlock_destroy)
DEFINE_FUN1(uv_rwlock_rdlock)
DEFINE_FUN1(uv_rwlock_tryrdlock)
DEFINE_FUN1(uv_rwlock_rdunlock)
DEFINE_FUN1(uv_rwlock_wrlock)
DEFINE_FUN1(uv_rwlock_trywrlock)
DEFINE_FUN1(uv_rwlock_wrunlock)

DEFINE_FUN2(uv_sem_init)
DEFINE_FUN1(uv_sem_destroy)
DEFINE_FUN1(uv_sem_post)
DEFINE_FUN3(uv_sem_wait)
DEFINE_FUN3(uv_sem_trywait)

DEFINE_FUN1(uv_cond_init)
DEFINE_FUN1(uv_cond_destroy)
DEFINE_FUN1(uv_cond_signal)
DEFINE_FUN1(uv_cond_broadcast)

DEFINE_FUN2(uv_barrier_init)
DEFINE_FUN1(uv_barrier_destroy)
DEFINE_FUN1(uv_barrier_wait)

DEFINE_FUN2(uv_cond_wait)
DEFINE_FUN_EX(int, uv_cond_timedwait, (void* p1, void* p2, __int64 p3), (p1, p2, p3))

DEFINE_FUN2(uv_once)

DEFINE_FUN1(uv_key_create)
DEFINE_FUN1(uv_key_delete)
DEFINE_FUN1(uv_key_get)
DEFINE_FUN2(uv_key_set)

DEFINE_FUN3(uv_thread_create)
DEFINE_FUN0(uv_thread_self)
DEFINE_FUN1(uv_thread_join)
DEFINE_FUN2(uv_thread_equal)