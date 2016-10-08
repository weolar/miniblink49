/***
*strstrea.h - definitions/declarations for strstreambuf, strstream
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       This file defines the classes, values, macros, and functions
*       used by the strstream and strstreambuf classes.
*       [AT&T C++]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifdef  __cplusplus

#ifndef _INC_STRSTREAM
#define _INC_STRSTREAM

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif


#ifdef  _MSC_VER

// Currently, all MS C compilers for Win32 platforms default to 8 byte
// alignment.
#pragma pack(push,8)

#endif  // _MSC_VER

/* Define _CRTIMP */

#ifndef _CRTIMP
#ifdef  _DLL
#define _CRTIMP __declspec(dllimport)
#else   /* ndef _DLL */
#define _CRTIMP
#endif  /* _DLL */
#endif  /* _CRTIMP */


#include <useoldio.h>
#include <iostream.h>

#ifdef  _MSC_VER
#pragma warning(disable:4514)           // disable unwanted /W4 warning
// #pragma warning(default:4514)        // use this to reenable, if necessary
#endif  // _MSC_VER

class _CRTIMP strstreambuf : public streambuf  {
public:
                strstreambuf();
                strstreambuf(int);
                strstreambuf(char *, int, char * = 0);
                strstreambuf(unsigned char *, int, unsigned char * = 0);
                strstreambuf(signed char *, int, signed char * = 0);
                strstreambuf(void * (*a)(long), void (*f) (void *));
                ~strstreambuf();

        void    freeze(int =1);
        char * str();

virtual int     overflow(int);
virtual int     underflow();
virtual streambuf* setbuf(char *, int);
virtual streampos seekoff(streamoff, ios::seek_dir, int);
virtual int     sync();         // not in spec.

protected:
virtual int     doallocate();
private:
        int     x_dynamic;
        int     x_bufmin;
        int     _fAlloc;
        int     x_static;
        void * (* x_alloc)(long);
        void    (* x_free)(void *);
};

class _CRTIMP istrstream : public istream {
public:
                istrstream(char *);
                istrstream(char *, int);
                ~istrstream();

inline  strstreambuf* rdbuf() const { return (strstreambuf*) ios::rdbuf(); }
inline  char * str() { return rdbuf()->str(); }
};

class _CRTIMP ostrstream : public ostream {
public:
                ostrstream();
                ostrstream(char *, int, int = ios::out);
                ~ostrstream();

inline  int     pcount() const { return rdbuf()->out_waiting(); }
inline  strstreambuf* rdbuf() const { return (strstreambuf*) ios::rdbuf(); }
inline  char *  str() { return rdbuf()->str(); }
};

class _CRTIMP strstream : public iostream {    // strstreambase ???
public:
                strstream();
                strstream(char *, int, int);
                ~strstream();

inline  int     pcount() const { return rdbuf()->out_waiting(); } // not in spec.
inline  strstreambuf* rdbuf() const { return (strstreambuf*) ostream::rdbuf(); }
inline  char * str() { return rdbuf()->str(); }
};

#ifdef  _MSC_VER
// Restore previous packing
#pragma pack(pop)
#endif  // _MSC_VER

#endif  // _INC_STRSTREAM

#endif /* __cplusplus */

