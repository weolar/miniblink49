#pragma warning(3:4092)   // sizeof returns 'unsigned long'
#pragma warning(4:4096)   // '__cdecl' must be used with '...'
#pragma warning(4:4121)   // structure is sensitive to alignment
#pragma warning(3:4125)   // decimal digit in octal sequence
#pragma warning(3:4130)   // logical operation on address of string constant
#pragma warning(3:4132)   // const object should be initialized
#if _DBG_MEMCPY_INLINE_
#pragma warning(disable:4163) // not available as an intrinsic function
#endif
#pragma warning(4:4206)   // Source File is empty
#pragma warning(4:4101)   // Unreferenced local variable
#pragma warning(4:4208)   // delete[exp] - exp evaluated but ignored
#pragma warning(3:4212)   // function declaration used ellipsis
#pragma warning(3:4242)   // convertion possible loss of data
#if defined(_M_IA64)
#pragma warning(disable:4407) // NTBUG#476234:  Silent codegen bug is a warning
                              // with VC7.1.  Jason Shirk has fixed pdlparse in
                              // the VC code base, but IE won't fix their
                              // version until post server.
#pragma warning(disable:4714) // function marked as __forceinline not inlined
                              // (VC7.1:  inlining is not allowed in finally)
#endif
#pragma warning(4:4267)   // convertion from size_t to smaller type
#pragma warning(4:4312)   // conversion to type of greater size
#pragma warning(disable:4324)  // structure padded due to __declspec(align())
#pragma warning(error:4700)    // Local used w/o being initialized
#pragma warning(error:4259)    // pure virtual function was not defined
#pragma warning(disable:4071)  // no function prototype given - formals unspecified
#pragma warning(error:4013)    // function' undefined - assuming extern returning int
#pragma warning(error:4551)    // Function call missing argument list
#pragma warning(error:4806)    // unsafe operation involving type 'bool'
#pragma warning(4:4509)   // use of SEH with destructor
#pragma warning(4:4177)   // pragma data_seg s/b at global scope
#pragma warning(disable:4274)  // #ident ignored
#pragma warning(disable:4786)  // identifier was truncated to 255 chararcers in debug information.
#pragma warning(disable:4503)  // decorated name length exceeded, name was truncated.
#pragma warning(disable:4263)  // Derived override doesn't match base - who cares...
#pragma warning(disable:4264)  // base function is hidden - again who cares.
#pragma warning(disable:4710)  // Function marked as inline - wasn't
#pragma warning(disable:4917)  // A GUID can only be associated with a class, interface or namespace
#pragma warning(error:4552)    // <<, >> ops used to no effect (probably missing an = sign)
#pragma warning(error:4553)    // == op used w/o effect (probably s/b an = sign)
#pragma warning(3:4288)   // nonstandard extension used (loop counter)
#pragma warning(3:4532)   // jump out of __finally block
#pragma warning(error:4312)  // cast of 32-bit int to 64-bit ptr
#pragma warning(error:4296)  // expression is always true/false
#pragma warning(3:4546)   // function call before comma missing argument list
// disable until __noop(arg,arg,arg) doesn't generate false hits.
// #pragma warning(3:4547)   // '<' : operator before comma has no effect; expected operator with side-effect
// #pragma warning(3:4548)   // expression before comma has no effect; expected expression with side-effect

#if _MSC_VER > 1300
#pragma warning(disable:4197)   // illegal use of const/volatile: qualifier ignored (disabled until sources fixed)
#pragma warning(disable:4675)	// picked overload found via Koenig lookup
#pragma warning(disable:4356)	// static member cannot be initialized via derived class
#endif

#if 0
#pragma warning(3:4100)   // Unreferenced formal parameter
#pragma warning(3:4701)   // local may be used w/o init
#pragma warning(3:4702)   // Unreachable code
#pragma warning(3:4705)   // Statement has no effect
#pragma warning(3:4706)   // assignment w/i conditional expression
#pragma warning(3:4709)   // command operator w/o index expression
#endif

#ifndef __cplusplus
#undef try
#undef except
#undef finally
#undef leave
#define try                         __try
#define except                      __except
#define finally                     __finally
#define leave                       __leave
#endif

#if _MSC_VER <= 1400
#pragma warning(disable: 4068)	// turn off unknown pragma warning so prefast pragmas won't show
				// show up in build.wrn/build.err
#endif

#if defined(_M_IA64) && _MSC_VER > 1310
#define __TYPENAME typename
#elif defined(_M_IX86) && _MSC_FULL_VER >= 13102154
#define __TYPENAME typename
#elif defined(_M_AMD64) && _MSC_VER >= 1400
#define __TYPENAME typename
#else
#define __TYPENAME
#endif

