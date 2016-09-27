
#ifndef _QT_GLOBAL_H_
#define _QT_GLOBAL_H_ 

# define QT_BEGIN_NAMESPACE
# define QT_END_NAMESPACE
# define QT_USE_NAMESPACE
# define QT_BEGIN_INCLUDE_NAMESPACE
# define QT_END_INCLUDE_NAMESPACE

#define Q_DECL_EXPORT 
#define Q_CORE_EXPORT 

#define QT3_SUPPORT 

#define Q_DECL_DEPRECATED __declspec(deprecated)
#define Q_DECL_CONSTRUCTOR_DEPRECATED Q_DECL_DEPRECATED
#define QT_ASCII_CAST_WARN_CONSTRUCTOR Q_DECL_CONSTRUCTOR_DEPRECATED

#define Q_REQUIRED_RESULT

#if defined(__i386__) || defined(_WIN32) || defined(_WIN32_WCE)
#  if defined(Q_CC_GNU)
#if !defined(Q_CC_INTEL) && ((100*(__GNUC__ - 0) + 10*(__GNUC_MINOR__ - 0) + __GNUC_PATCHLEVEL__) >= 332)
#    define QT_FASTCALL __attribute__((regparm(3)))
#else
#    define QT_FASTCALL
#endif
#  elif defined(Q_CC_MSVC)
#    define QT_FASTCALL __fastcall
#  else
#     define QT_FASTCALL
#  endif
#else
#  define QT_FASTCALL
#endif

#endif // _QT_GLOBAL_H_