//
// assert.h
//
// Defines the assert macro and related functionality.
//

#if defined _VCRT_BUILD && !defined _ASSERT_OK

#error assert.h not for CRT internal use

#endif

#ifdef __GNUC__

#include <crtdefs.h>

#undef assert

#if (defined _ISOC11_SOURCE || (defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L))

#undef static_assert

#define static_assert _Static_assert

#endif

#ifdef NDEBUG

#define assert(expression) ((void)0)

#else

extern void __cdecl _assert(
    char const* _Message,
    char const* _File,
    unsigned _Line
);

#define assert(expression) (void)((!!(expression)) || (_assert(#expression, __FILE__, (unsigned)(__LINE__)), 0))

#endif

#else /* MSVC */

#include <corecrt.h>

_CRT_BEGIN_C_HEADER

#undef assert

#ifdef NDEBUG

#define assert(expression) ((void)0)

#else

_ACRTIMP void __cdecl _assert(
    _In_z_ char const* _Message,
    _In_z_ char const* _File,
    _In_   unsigned    _Line
);

#define assert(expression) (void)((!!(expression)) || (_assert(#expression, __FILE__, (unsigned)(__LINE__)), 0))

#endif

_CRT_END_C_HEADER

#endif
