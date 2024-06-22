
#ifndef NodeRegisterHelp_h
#define NodeRegisterHelp_h

#define NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(modname, regfunc, priv) \
    extern "C" {                                                                \
    static node::node_module _module = {                                        \
        NODE_MODULE_VERSION,                                                    \
        NM_F_BUILTIN,                                                           \
        NULL,                                                                   \
        __FILE__,                                                               \
        NULL,                                                                   \
        (node::addon_context_register_func)(regfunc),                           \
        NODE_STRINGIFY(modname),                                                \
        priv,                                                                   \
        NULL                                                                    \
    };                                                                          \
    void __cdecl _register_##modname(void);                                     \
    void(__cdecl * _register_##modname##_)(void) = _register_##modname;         \
    void __cdecl _register_##modname(void)                                      \
    {                                                                           \
        node_module_register(&_module);                                         \
    }                                                                           \
    }

#define NODE_MODULE_LOAD_BUILTIN_SCRIPT_MANUAL(modname, regfunc, priv)  \
    extern "C" {                                                        \
    static node::node_module _module = {                                \
        NODE_MODULE_VERSION,                                            \
        NM_F_BUILTIN,                                                   \
        NULL,                                                           \
        __FILE__,                                                       \
        NULL,                                                           \
        (node::addon_context_register_func)(regfunc),                   \
        NODE_STRINGIFY(modname),                                        \
        priv,                                                           \
        NULL                                                            \
    };                                                                  \
    void __cdecl _register_##modname(void);                             \
    void(__cdecl * _register_##modname##_)(void) = _register_##modname; \
    void __cdecl _register_##modname(void)                              \
    {                                                                   \
        loadNodeScriptFromRes(priv);                                    \
        node_module_register(&_module);                                 \
    }                                                                   \
    }

#define NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(modname) \
    extern "C" void __cdecl _register_##modname(void);

#define NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(modname) \
    _register_##modname();

namespace atom {
void loadNodeScriptFromRes(void* nativePath);
}

#endif