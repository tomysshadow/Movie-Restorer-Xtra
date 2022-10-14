/*
ADOBE SYSTEMS INCORPORATED
Copyright 1994 - 2008 Adobe Macromedia Software LLC
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.  If you have received this file from a 
source other than Adobe, then your use, modification, or distribution of it requires the prior 
written permission of Adobe.
*/

#ifndef _H_moatyedg
#define _H_moatyedg

/*
	File: moatyedg.h (windows)

	Abstract:
	
	Fundamental types for the MOA Services library.	
*/

/*---------------------------------------------------------------------------
/ 1. Include other headers we depend on.
/--------------------------------------------*/
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
#define MOAPLATFORM_WIN32
#if (!defined(WINDOWS) && !defined(_WINDOWS)) || (!defined(WIN32) && !defined(_WIN32))
	#error "This is a Win32 file."
#endif

#include <string.h>
#include <stdarg.h>

// get GUID defs from Windows Platform SDK
// #include <guiddef.h>

#ifndef _NO_MOA_WINDOWS_INCLUDE_
	#include <windows.h>

	// get STD COM macros from Windows Platform SDK
	#include <objbase.h>

	// undef GetClassInfo because windows.h will define it as a macro (via winuser.h)
	// that macro conflicts with moaxtra.h's use of DLL(GetClassInfo)
	// Just use the Ansi or Wide version directly: GetClassInfoA or GetClassInfoW
	#undef GetClassInfo
#endif

#if !defined(_INC_WINDOWS) && !defined(_WINDOWS_)
#pragma message("Warning: MOA is redeclaring Windows types")
		typedef void* HANDLE;
		extern __declspec(dllimport) int __stdcall wvsprintfA(char*, const char*, va_list);
		#define moa_wvsprintf wvsprintfA
		extern __declspec(dllimport) void __stdcall DebugBreak(void);
		extern __declspec(dllimport) void __stdcall OutputDebugStringA(const char *);
		#define moa_OutputDebugString OutputDebugStringA
		#if defined(_MSC_VER) && _MSC_VER == 1100 // VC5
		typedef HANDLE HINSTANCE;
		#endif
		#if defined(_MSC_VER) && _MSC_VER == 1200 // VC6
		typedef struct HINSTANCE__* HINSTANCE;
		#endif
#else
		#define moa_wvsprintf wvsprintfA
		#define moa_OutputDebugString OutputDebugStringA
#endif

/*---------------------------------------------------------------------------
/ 2. Define constants and macros.
/--------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*
	This is a unique string to decribe the MOAPLATFORM_ code.
	It should be exactly 5 characters, not including the
	null terminator.
*/
#define MOA_PLATFORM_NAME	"Win32"

/*-------------------------------------------------------------------------*/
/*
	if your compiler easily chokes on complex macros, define this.
	we remove some two-stage macros that reduce the flexibility
	but allow for compilation on older compilers.
*/
#undef COMPILER_LIMITED_MACRO_SPACE

/*-------------------------------------------------------------------------*/
/*
	if C++ bindings are allowed in this compiler configuration,
	define this; otherwise, undefine it.
*/
#undef MOA_CPLUS_BINDINGS_ALLOWED
#if defined(__cplusplus)
	#define MOA_CPLUS_BINDINGS_ALLOWED	1
#endif

/*-------------------------------------------------------------------------*/
/*
	If the current compiler configuration allows for auto-initialization
	of static variables to addresses (e.g., void* foo = &bar; ),
	define this; otherwise, undefine it. undefining it has the
	effect of always forcing "safer" vtables (rather than "smaller" vtables).
*/
#define MOA_STATIC_ADDR_ALLOWED

/*-------------------------------------------------------------------------*/
/*
	if the standard COM definitions have already been done
	by system includes, define _MOA_COM_PROVIDED_
*/
#undef _MOA_COM_PROVIDED_
#if defined(__COMPOBJ__) || defined(_OBJBASE_H_)
	#define _MOA_COM_PROVIDED_
#endif

/*-------------------------------------------------------------------------*/
/*
	This should define a type as being read-only constant data,
	and attempt to place all such data into a common segment
	for linkage redirection. usage is
	
		MOA_READONLY(MoaLong) gFoo = 1;
	
	declares gFoo to be a read-only MoaLong. Note that this
	is only allowed for statically initialized stuff.
*/
#define MOA_READONLY(macro_TYPE) const macro_TYPE

/*-------------------------------------------------------------------------*/
/*
	This macro is used to enforce that all methods and routines declared
	using STDMETHOD or STDAPI follow proper calling conventions.
	It doesn't change the declaration of the method, routine, or class,
	but should try to ensure the compiler is in the correct "mode",
	generating a compiler error if not.

	it's really just a trick used on 68k Macs to make sure you #pragma your
	stuff correctly: we prototype a function with pointers_in_D0
	in effect, then re-prototype it in all the places you need
	the pragma. if you forget, you'll get a compile error
	about "pointers_in_D0_reminder redeclared". unfortunately,
	we can't fix this for you automatically (since you can't include
	#pragma's inside macros) but we can type check and make sure you
	don't make a mistake.
*/
#define _MOA_PTRCHECK

/*-------------------------------------------------------------------------*/
/*
	If the wacky "pointers_in_D0" macro is required, define
	MOA_pointers_in_D0; otherwise, don't define it.
*/
#undef MOA_pointers_in_D0

/*-------------------------------------------------------------------------*/
/*
	If the compiler understands the "#pragma export" pragma,
	define MOA_EXPORT_LIST, otherwise undefine it.
*/
#undef MOA_EXPORT_LIST

/*-------------------------------------------------------------------------*/
/*
    Magic Cookie structure needed below.
*/
typedef struct _MoaXtraMagicCookie * MoaXtraMagicCookie;

/*-------------------------------------------------------------------------*/
/*
	The following X_ macros are used to set up any global context
	pointers necessary when entering and exiting method or API call.
	Usage is as follows:
	
	X_MAIN -- used when first entering the main entry point
		of an Xtra; argument is the address of the main routine.
	X_ENTER -- used immediately after opening brace of a method
		or function.
	X_EXIT -- used immediately before the closing brace of a method
		or function.
	X_RETURN and X_RETURN_VOID -- used to return a value from
		within an X_ENTER/X_EXIT block. return CANNOT be used
		within this block.
*/
#define X_MAIN(mainAddr)			{ }
#define X_ENTER						{
#define X_ENTER_(macro_COOKIE)		{
#define X_ENTER_A4(macro_A4)		{
#define X_ENTER_DLLENTRYPOINT		{
#define X_EXIT						}
#define X_RETURN(retType, retVal)	do { return(retVal); } while (0)
#define X_RETURN_VOID				do { return; } while (0)
#define X_STD_RETURN(retVal)	    X_RETURN(MoaError, (retVal))

#define X_ABORT						{ }

/*-------------------------------------------------------------------------*/
/*
	macro for declaring a function an external C function.
	says nothing about calling convention.
*/
#ifndef EXTERN_C
	#ifdef __cplusplus
		#define EXTERN_C	extern "C"
	#else
		#define EXTERN_C	extern
	#endif
#endif

#ifndef _MOA_COM_PROVIDED_

	#if defined(_MSC_VER) && _MSC_VER == 1100 // VC5

		/*
			The calling convention for a COM method (typically cdecl)
		*/
		#define STDMETHODCALLTYPE					__stdcall
		
		/*
			The calling convention for an API routine (typically pascal)
		*/
		#define STDAPICALLTYPE						__stdcall
		
		/* same as STDAPIIMP_ and STDAPIIMP, but an external prototype. */
		#define STDAPI_(macro_TYPE)					_MOA_PTRCHECK EXTERN_C macro_TYPE STDAPICALLTYPE
		#define STDAPI								STDAPI_(MoaError)
		
		/* declare a function of STDMETHOD convention returning a given type. */
		#define STDMETHODIMP_(returnType)			_MOA_PTRCHECK returnType STDMETHODCALLTYPE
		
		/* declare a function of STDMETHOD convention returning MoaError. */
		#define STDMETHODIMP						STDMETHODIMP_(MoaError)

	#endif

	#if defined(_MSC_VER) && _MSC_VER == 1200 // VC6

		typedef long HRESULT;
		
		/*
			The calling convention for a COM method (typically cdecl)
		*/
		#define STDMETHODCALLTYPE					__stdcall
		
		/*
			The calling convention for an API routine (typically pascal)
		*/
		#define STDAPICALLTYPE						__stdcall
		
		/* same as STDAPIIMP_ and STDAPIIMP, but an external prototype. */
		#define STDAPI_(macro_TYPE)					EXTERN_C macro_TYPE STDAPICALLTYPE
		#define STDAPI								EXTERN_C HRESULT STDAPICALLTYPE
		
		/* declare a function of STDMETHOD convention returning a given type. */
		#define STDMETHODIMP_(returnType)			returnType STDMETHODCALLTYPE
		
		/* declare a function of STDMETHOD convention returning MoaError. */
		#define STDMETHODIMP						HRESULT STDMETHODCALLTYPE

	#endif

#else
	
	/* COM provides these definitions */

#endif	/* _MOA_COM_PROVIDED_ */

/*-------------------------------------------------------------------------*/
/* declare a function of STDAPI convention returning a given type. */
#define STDAPIIMP_(macro_TYPE) 				_MOA_PTRCHECK macro_TYPE STDAPICALLTYPE

/*-------------------------------------------------------------------------*/
/* declare a function of STDAPI convention returning MoaError. */
#define STDAPIIMP 							STDAPIIMP_(MoaError)

/*-------------------------------------------------------------------------*/
/* same as STDMETHODIMP_ and STDMETHODIMP, but an external prototype. */
#define EXTERN_STDMETHODIMP_(returnType)	_MOA_PTRCHECK EXTERN_C returnType STDMETHODCALLTYPE
#define EXTERN_STDMETHODIMP					EXTERN_STDMETHODIMP_(MoaError)

/*-------------------------------------------------------------------------*/
/*
	macro to declare a procptr of STDMETHODCALLTYPE convention.
*/
#define MOA_STD_METHOD_PROCPTR(macro_RETURNTYPE, macro_PROCNAME) \
	macro_RETURNTYPE (STDMETHODCALLTYPE * macro_PROCNAME)

/*-------------------------------------------------------------------------*/
/*
	macro to declare a procptr of STDAPICALLTYPE convention.
*/
#define MOA_STD_API_PROCPTR(macro_RETURNTYPE, macro_PROCNAME) \
	macro_RETURNTYPE (STDAPICALLTYPE * macro_PROCNAME)

/*-------------------------------------------------------------------------*/
/*
	BEGIN_INTERFACE is used to define the initial padding (if any) in
	an interface's vtable. This should match the standard padding for
	COM objects on that platform. Windows uses no padding; Mac uses
	a single 4-byte pad. Other systems may or may not have standards
	defined.

	You should also define _MOA_VTABLE_EXTRA_ENTRIES to be the number of
	entries padded, and _MOA_VTABLE_FILL_EXTRA_ENTRIES to be enough
	declarations of NULL to fill the extra entries (e.g., if there
	are two extra entries, define it to be " NULL, NULL, "
*/
#ifndef BEGIN_INTERFACE
	#define BEGIN_INTERFACE
#endif

#define _MOA_VTABLE_EXTRA_ENTRIES 0
#define _MOA_VTABLE_FILL_EXTRA_ENTRIES		/* nothing */

/*-------------------------------------------------------------------------*/
/*
	this macro is used to declare the parent interface of
	IMoaUnknown. On most systems, IMoaUnknown won't have
	one; some systems (Mac notably) require IMoaUnknown
	to inherit from a built-in compiler class to form
	C++ vtables correctly.
*/
#define DECLARE_INTERFACE_IMOAUNKNOWN \
	DECLARE_INTERFACE(IMoaUnknown)

/*-------------------------------------------------------------------------*/
/*
	This macro calls a routine to flush the instruction cache (if necessary).
	This is used primarily by debugging code after clearing freed memory
	to bogus values.
*/
#define _MOA_FLUSH_CACHE

/*-------------------------------------------------------------------------*/
/*
	This should output a debug message to a debugging console,
	and stop execution (if possible). The string is a C string
	that can be trashed by the routine!
*/
#define _MOA_DEBUGSTR_(x) do { moa_OutputDebugString(x); /* DebugBreak(); */ } while (0)

/*-------------------------------------------------------------------------*/
/*
	This should be equivalent to the stdio sequence:
	
	va_list args;
	
	va_start(args, (msg));
	vsprintf((buf), (msg), args);
	va_end(args);
	
	if your platform doesn't support va_args, you should use:
	
	strcpy((buf), (msg));
	
*/
#define _MOA_VSPRINTF_(buf, msg) \
	do { \
		va_list __args__; \
		va_start(__args__, (msg)); \
		moa_wvsprintf((buf), (msg), __args__); \
		va_end(__args__); \
	} while (0)

/*-------------------------------------------------------------------------*/
/*
	This macro needs to call the C++ new_handler in order to handle
	a failure from operator new. although theoretically a standard
	technique, some compilers do things differently (e.g., MSVC).
	This macro needs to call the current new_handler. if there
	is no new_handler, it should return() a value of 0.
*/
#if defined(_MSC_VER) && !defined(__INTEL__)
	/* alas, MS does this differently... */
	#define _MOA_DO_NEW_HANDLER \
		do { \
			_PNH pehf = _set_new_handler(0); \
			_set_new_handler(pehf); \
			if (pehf) pehf(size); else return(0); \
		} while (0)
#else
	/* but CW does it the usual way */ 
	#define _MOA_DO_NEW_HANDLER \
		do { \
			void (*pehf)() = set_new_handler(0); \
			set_new_handler(pehf); \
			if (pehf) pehf(); else return(0); \
		} while (0)
#endif

/*-------------------------------------------------------------------------*/
/*
	The maximum possible length of a pathname for this platform's
	file system.
*/
#define MOA_MAX_PATHNAME		1024

/*-------------------------------------------------------------------------*/
/*
	The maximum possible length of a filename for this platform's
	file system.
*/
#define MOA_MAX_FILENAME		1024

/*-------------------------------------------------------------------------*/
/* macro to use for any pointer which has to be at least a 32 bit pointer */
#ifndef FAR
	#define far
	#define FAR far
#endif

/*-------------------------------------------------------------------------*/
/* Macro to use for any pointer where there may be math done on the pointer
/  causing it to access 64k or more beyond the pointer.  On most platforms
/  there is no such distinction. */

#ifndef HUGEP
	#define HUGEP
#endif

/*-------------------------------------------------------------------------*/
/*
	series of macros for operating on the "wide" data types.
	You may implement these as you like (inline assembly, conditional
	C++ inlines, whatever).
*/
#define LONG_TO_WIDE(L, W) \
		do { (W).lo = (MoaUlong)(L); (W).hi = (L) >= 0 ? 0 : -1; } while (0);

/* WIDE_TO_LONG ignores overflow -- use WIDE_FITS */
#define WIDE_TO_LONG(W, L) \
		do { (L) = (MoaLong)(W).lo; } while (0);

#define WIDE_FITS(W) \
		(((W).hi == 0 && ((W).lo & 0x80000000) == 0) || \
		((W).hi == -1 && ((W).lo & 0x80000000) != 0))

#define WIDE_GT_ZERO(x) \
		((x).hi >= 0 && (x).lo != 0)

#define WIDE_EQ_ZERO(x) \
		((x).hi == 0 && (x).lo == 0)

#define WIDE_LT_ZERO(x) \
		((x).hi < 0)

/* effects "x += y" where both x and y are MoaWide */
#define WIDE_ADD_WIDE(x, y) \
 	do { MoaUlong tmp = (x).lo; (x).hi += (y).hi; \
 		(x).lo += (y).lo; if ((x).lo < tmp) (x).hi++; } while (0)

/* effects "x = -x", where x is a MoaWide */
#define WIDE_NEGATE(x) \
	do { (x).hi = ~(x).hi; (x).lo = ~(x).lo; ++(x).lo; \
		if ((x).lo == 0) ++(x).hi; } while (0)

/*-------------------------------------------------------------------------*/
/* Macros for converting to/from Win types */
#define	MoaToWinPoint(moa,win) \
	do { (win)->x = (int)(moa)->x; \
		(win)->y = (int)(moa)->y; } while (0)

#define WinToMoaPoint(win,moa) \
	do { (moa)->x = (MoaCoord)(win)->x; \
		(moa)->y = (MoaCoord)(win)->y; } while (0)

#define	MoaToWinRect(a,b)  	do { \
							(b)->top    = (int)(a)->top;		\
							(b)->left   = (int)(a)->left;		\
							(b)->bottom = (int)(a)->bottom;		\
							(b)->right  = (int)(a)->right;		\
							} while (0)
							
#define	WinToMoaRect(a,b)  	do { \
							(b)->top    = (MoaCoord)(a)->top;	\
							(b)->left   = (MoaCoord)(a)->left;	\
							(b)->bottom = (MoaCoord)(a)->bottom;\
							(b)->right  = (MoaCoord)(a)->right;	\
							} while (0)

/*-------------------------------------------------------------------------*/
/*
	The value for a MoaFileRef that refers to no file (or no valid file).
*/
#define kMoaFileRef_NoRef ((MoaFileRef)0)

/*-------------------------------------------------------------------------*/
/*
	This should be defined to be a pointer-sized value that
	is always a "bad" pointer.
*/
#define MOA_CLOBBERVAL	((PMoaVoid)0x50ff8001)

/*-------------------------------------------------------------------------*/
/*
	This is a big honkin' piece of code that will be inserted into
	every non-App Xtra (i.e., whenever MOA_APP_CODE is not defined).
	You'll want to put stuff like a standard DLL entry point (or whatever)
	into this code.
*/


#define _MOA_XCODE_PREFACE \
			 \
			MoaFileRef gXtraFileRef = kMoaFileRef_NoRef; \
			 \
			EXTERN_C BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, PMoaVoid reserved) \
			{ \
				gXtraFileRef = hInst; \
			 	return 1; \
			}

/*---------------------------------------------------------------------------
/*
	given a pointer to a MoaSystemFileSpec, return its size
	in bytes. if pointer is NULL, return 0 (don't crash).
*/

#define GetFileSpecLen(pspec) ((pspec) ? (moa_strlen((ConstPMoaChar)(pspec)) + 1) : 0)

/*---------------------------------------------------------------------------
/*
	versions of classic C library routines that use FAR-sized
	pointers. You can point these at standard C routines, or
	write them yourself.
*/

#define moa_memcmp(a,b,c)	memcmp((a),(b),(c))
#define moa_memcpy(a,b,c)	memcpy((a),(b),(c))
#define moa_memmove(a,b,c)	memmove((a),(b),(c))
#define moa_memset(a,b,c)	memset((a),(b),(c))
#define moa_strcat(a,b)		strcat((a),(b))
#define moa_strcmp(a,b)		strcmp((a),(b))
#define moa_strcpy(a,b)		strcpy((a),(b))
#define moa_strncpy(a,b,c)	strncpy((a),(b),(c))
#define moa_strlen(a)		strlen((a))

/*---------------------------------------------------------------------------
/ 3. Declare structures and types.
/--------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*
	signed 8-bit character.
*/
typedef char			MoaChar;

/*-------------------------------------------------------------------------*/
/*
	unsigned 16-bit unicode character.
*/
typedef unsigned short		MoaUnichar;

/*-------------------------------------------------------------------------*/
/*
	unsigned 8-bit boolean
*/
typedef unsigned char		MoaBool;

/*-------------------------------------------------------------------------*/
/*
	unsigned 8-bit byte.
*/
typedef unsigned char		MoaByte;

/*-------------------------------------------------------------------------*/
/*
	signed 16-bit integer.
*/
typedef signed short 		MoaShort;

/*-------------------------------------------------------------------------*/
/*
	unsigned 16-bit integer.
*/
typedef unsigned short 		MoaUshort;

/*-------------------------------------------------------------------------*/
/*
	signed 32-bit integer.
*/
typedef long 				MoaLong;

/*-------------------------------------------------------------------------*/
/*
	unsigned 32-bit integer.
*/
typedef unsigned long 		MoaUlong;

/*-------------------------------------------------------------------------*/
/*
	signed 32-bit integer, treated as a 16.16 fixed-point number.
*/
typedef signed long			MoaFixed;

/*-------------------------------------------------------------------------*/
/*
	32-bit value; may be signed or unsigned, integral or pointer.
*/
/*
	This must match the Windows definition of HRESULT,
	which is a void* under Win16 and a long under Win32.
*/
typedef signed long	MoaError;

/*-------------------------------------------------------------------------*/
/*
	32-bit IEEE floating-point number.
*/
typedef float 				MoaFloat;

/*-------------------------------------------------------------------------*/
/*
	64-bit IEEE floating-point number.
*/
typedef double 			MoaDouble;

/*-------------------------------------------------------------------------*/
/*
 	pointer to void.
*/
typedef void * 			PMoaVoid;

/*-------------------------------------------------------------------------*/
/*
	const pointer to void.
*/
typedef const void * 	ConstPMoaVoid;

/*-------------------------------------------------------------------------*/
/*
	pointer to pointer to void.
*/
typedef void **	PPMoaVoid;

/*-------------------------------------------------------------------------*/
/* for types < 32 bits: versions for arg lists. Must be long on
	Macintosh systems. */
typedef MoaUlong			MoaUnicharParam;
typedef MoaLong				MoaCharParam;
typedef MoaUlong			MoaBoolParam;

/*-------------------------------------------------------------------------*/
/*
	Similar to MoaBoolParam, but must be binary-compatible
	with the Microsoft BOOL type (this is really only
	an issue on Win16 systems, that require it to be
	a 16-bit param)
*/
typedef long				MoaMsBoolParam;

/*-------------------------------------------------------------------------*/
/*
	a 64-bit integer struct. "hi" must be signed
	and "lo" must be unsigned. You may arrange them
	as you like.
*/
/*
	This definition is a binary match with MS's "LARGE_INTEGER" type.
*/
typedef struct MoaWide {
	MoaUlong 	lo;
	MoaLong		hi;
} MoaWide;

/*-------------------------------------------------------------------------*/
/*
	this type is a "cookie" used to refer to an Xtra's file container.
	it need not be a useful type, though it might be; at minimum,
	it needs to be a cookie to pass to IMoaCallback::MoaBeginUsingResources
	that can be used to access the Xtra's resource area.
*/
typedef HINSTANCE MoaFileRef;

/*-------------------------------------------------------------------------*/
/*
	this type is returned by IMoaCallback::MoaBeginUsingResources
	and must be used to directly access the Xtra's resource area.
*/
typedef HINSTANCE XtraResourceCookie;

/*-------------------------------------------------------------------------*/
/*
	a type large enough to hold the largest MoaSystemFileSpec.

	GetFileSpecLen(foo) <= sizeof(MoaSystemFileSpecBuf).

	it must be guaranteed that any buffer that is >= sizeof(MoaSystemFileSpecBuf)
	must be large enough to hold any system file spec.
*/
typedef MoaChar MoaSystemFileSpecBuf[MOA_MAX_PATHNAME];

/*-------------------------------------------------------------------------*/
/*
	Pointers to the platform-specific file spec. This may be
	a pathname or other platform-dependent structure of
	arbitrary or variable size.
*/
typedef MoaChar* PMoaSystemFileSpec;
typedef const MoaChar* ConstPMoaSystemFileSpec;

/*-------------------------------------------------------------------------*/
/*
	define the MoaID type. It must be a long, two shorts,
	and 8 bytes, all unsigned, with NO padding, to be
	exactly 16 bytes. If COM is provided, it must exactly
	match a GUID.
*/
#ifdef _MOA_COM_PROVIDED_
	typedef GUID MoaID;
#else
	#pragma pack(2)
	typedef struct {
		MoaUlong	Data1;
		MoaUshort	Data2;
		MoaUshort	Data3;
		MoaByte		Data4[8];
	} MoaID;
	#pragma pack()
#endif

/* --- BEGIN added by DJ from 1.48 release of MOA (Hopper MOA headers). --- */
	#define STDPROCPTR(macro_CALLCONVENTION, macro_RETURNTYPE, macro_PROCNAME) \
		macro_RETURNTYPE (macro_CALLCONVENTION * macro_PROCNAME)
/* --- END added by DJ from 1.48 release of MOA (Hopper MOA headers). --- */


/* HOPPER 1.60 MOA Changes BELOW. */
/* HOPPER 1.60 MOA Changes BELOW. */
/* HOPPER 1.60 MOA Changes BELOW. */
/* HOPPER 1.60 MOA Changes BELOW. */
/* HOPPER 1.60 MOA Changes BELOW. */


#define _MOA_COMPILER_A4CODE_PREFACE


#ifndef MOA_INTERLOCKED_INCREMENT
#define MOA_INTERLOCKED_INCREMENT(x) InterlockedIncrement(x)
#endif //MOA_INTERLOCKED_INCREMENT
 
#ifndef MOA_INTERLOCKED_DECREMENT
#define MOA_INTERLOCKED_DECREMENT(x) InterlockedDecrement(x)
#endif //MOA_INTERLOCKED_DECREMENT

#endif /* _H_moatyedg */





