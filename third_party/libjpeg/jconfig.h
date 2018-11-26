/* FLTK *************************************************************** */
/* FLTK   Comments marked with FLTK show modifications for FLTK which	*/
/* FLTK   should probably be preserved when the JPEG lib is upgraded.	*/
/* FLTK *************************************************************** */

/* jconfig.h.  Generated from jconfig.cfg by configure.  */
/* jconfig.cfg --- source file edited by configure script */
/* see jconfig.txt for explanations */

#define HAVE_PROTOTYPES 1
#define HAVE_UNSIGNED_CHAR 1
#define HAVE_UNSIGNED_SHORT 1
/* #undef void */
/* #undef const */
/* #undef CHAR_IS_UNSIGNED */
/* #define HAVE_STDDEF_H 1 */		/* FLTK */
#define HAVE_STDLIB_H 1
/* #define HAVE_LOCALE_H 1 */		/* FLTK */
/* #undef NEED_BSD_STRINGS */
/* #undef NEED_SYS_TYPES_H */
/* #undef NEED_FAR_POINTERS */
/* #undef NEED_SHORT_EXTERNAL_NAMES */
/* Define this if you get warnings about undefined structures. */
/* #undef INCOMPLETE_TYPES_BROKEN */

/* Define "boolean" as unsigned char, not enum, on Windows systems. */
#ifdef _WIN32
#ifndef __RPCNDR_H__		/* don't conflict if rpcndr.h already read */
typedef unsigned char boolean;
#endif
#ifndef FALSE			/* in case these macros already exist */
#define FALSE	0		/* values of boolean */
#endif
#ifndef TRUE
#define TRUE	1
#endif
#define HAVE_BOOLEAN		/* prevent jmorecfg.h from redefining it */
#endif

#ifdef JPEG_INTERNALS

/* #undef RIGHT_SHIFT_IS_UNSIGNED */
/* #define INLINE __inline__ */		/* FLTK */
/* These are for configuring the JPEG memory manager. */
/* #undef DEFAULT_MAX_MEM */
/* #undef NO_MKTEMP */

#endif /* JPEG_INTERNALS */

#ifdef JPEG_CJPEG_DJPEG

#define BMP_SUPPORTED		/* BMP image file format */
#define GIF_SUPPORTED		/* GIF image file format */
#define PPM_SUPPORTED		/* PBMPLUS PPM/PGM image file format */
/* #undef RLE_SUPPORTED */
#define TARGA_SUPPORTED		/* Targa image file format */

/* #undef TWO_FILE_COMMANDLINE */
/* #undef NEED_SIGNAL_CATCHER */
/* #undef DONT_USE_B_MODE */

#if 0					/* FLTK 1.3.3 (disabled in FLTK 1.3.4) */

#if defined(WIN32) || defined(__EMX__)
#  define USE_SETMODE
#endif /* WIN32 || __EMX__ */

#endif					/* FLTK 1.3.3 (disabled in FLTK 1.3.4) */

/* Define this if you want percent-done progress reports from cjpeg/djpeg. */
/* #undef PROGRESS_REPORT */

#endif /* JPEG_CJPEG_DJPEG */
