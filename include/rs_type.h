#pragma once

#if !defined(__clang__)
#  error "rs_types.hpp requires Clang (including clang-cl)."
#endif

// C++20 check (works for clang + clang-cl)
#if defined(_MSVC_LANG)
#  if _MSVC_LANG < 202002L
#    error "rs_types.hpp requires C++20."
#  endif
#else
#  if __cplusplus < 202002L
#    error "rs_types.hpp requires C++20."
#  endif
#endif

#include <cstdint>
#include <cstddef>
#include <climits>
#include <type_traits>
#include <limits>

// ------------------------------
// 8-bit byte
// ------------------------------
static_assert(CHAR_BIT == 8, "Engine requires 8-bit bytes (CHAR_BIT == 8).");

// ------------------------------
// exact-width
// ------------------------------
#if !defined(INT8_MAX)  || !defined(UINT8_MAX)  || \
    !defined(INT16_MAX) || !defined(UINT16_MAX) || \
    !defined(INT32_MAX) || !defined(UINT32_MAX) || \
    !defined(INT64_MAX) || !defined(UINT64_MAX)
#  error "Exact-width integer types are not available on this platform."
#endif

// ------------------------------
// Signed / Unsigned integers
// ------------------------------
typedef std::int8_t    RS_int8_t;
typedef std::uint8_t   RS_uint8_t;

typedef std::int16_t   RS_int16_t;
typedef std::uint16_t  RS_uint16_t;

typedef std::int32_t   RS_int32_t;
typedef std::uint32_t  RS_uint32_t;

typedef std::int64_t   RS_int64_t;
typedef std::uint64_t  RS_uint64_t;

static_assert(sizeof(RS_int8_t)  == 1 && std::is_signed_v<RS_int8_t>,  "RS_int8_t  must be 8-bit signed.");
static_assert(sizeof(RS_uint8_t) == 1 && std::is_unsigned_v<RS_uint8_t>,"RS_uint8_t must be 8-bit unsigned.");

static_assert(sizeof(RS_int16_t)  == 2 && std::is_signed_v<RS_int16_t>,  "RS_int16_t  must be 16-bit signed.");
static_assert(sizeof(RS_uint16_t) == 2 && std::is_unsigned_v<RS_uint16_t>,"RS_uint16_t must be 16-bit unsigned.");

static_assert(sizeof(RS_int32_t)  == 4 && std::is_signed_v<RS_int32_t>,  "RS_int32_t  must be 32-bit signed.");
static_assert(sizeof(RS_uint32_t) == 4 && std::is_unsigned_v<RS_uint32_t>,"RS_uint32_t must be 32-bit unsigned.");

static_assert(sizeof(RS_int64_t)  == 8 && std::is_signed_v<RS_int64_t>,  "RS_int64_t  must be 64-bit signed.");
static_assert(sizeof(RS_uint64_t) == 8 && std::is_unsigned_v<RS_uint64_t>,"RS_uint64_t must be 64-bit unsigned.");

#if INTPTR_MAX == INT64_MAX
typedef std::intptr_t   RS_isize64_t;
typedef std::uintptr_t  RS_usize64_t;
static_assert(sizeof(RS_isize64_t) == 8 && sizeof(RS_usize64_t) == 8, "Pointer size must be 64-bit.");
#elif INTPTR_MAX == INT32_MAX
typedef std::intptr_t   RS_isize32_t;
typedef std::uintptr_t  RS_usize32_t;
static_assert(sizeof(RS_isize32_t) == 4 && sizeof(RS_usize32_t) == 4, "Pointer size must be 32-bit.");
#else
#  error "Unsupported pointer size (neither 32-bit nor 64-bit)."
#endif

// ------------------------------
// Boolean=
// ------------------------------
typedef std::uint8_t RS_bool8_t;
static_assert(sizeof(RS_bool8_t) == 1, "RS_bool8_t must be 8-bit.");

// ------------------------------
// Character / Code units=
// ------------------------------
typedef char      RS_char8_t;     // 8-bit code unit / bytes
typedef char8_t   RS_u8char8_t;   // C++20 UTF-8 literal code unit type
typedef char16_t  RS_char16_t;    // UTF-16 code unit (16-bit by standard)
typedef char32_t  RS_char32_t;    // UTF-32 code unit (32-bit by standard)

static_assert(sizeof(RS_char8_t)    == 1, "RS_char8_t must be 8-bit.");
static_assert(sizeof(RS_u8char8_t)  == 1, "RS_u8char8_t must be 8-bit.");
static_assert(sizeof(RS_char16_t)   == 2, "RS_char16_t must be 16-bit.");
static_assert(sizeof(RS_char32_t)   == 4, "RS_char32_t must be 32-bit.");

// wchar_t 
typedef std::uint16_t RS_wchar16_t;
typedef std::uint32_t RS_wchar32_t;
static_assert(sizeof(RS_wchar16_t) == 2, "RS_wchar16_t must be 16-bit.");
static_assert(sizeof(RS_wchar32_t) == 4, "RS_wchar32_t must be 32-bit.");

// ------------------------------
// Floating point
// ------------------------------
typedef float  RS_float32_t;
typedef double RS_float64_t;

static_assert(sizeof(RS_float32_t) == 4, "RS_float32_t must be 32-bit IEEE-like float.");
static_assert(sizeof(RS_float64_t) == 8, "RS_float64_t must be 64-bit IEEE-like double.");

namespace RS
{
    // signed ints
    typedef RS_int8_t   i8;
    typedef RS_int16_t  i16;
    typedef RS_int32_t  i32;
    typedef RS_int64_t  i64;

    // unsigned ints
    typedef RS_uint8_t  u8;
    typedef RS_uint16_t u16;
    typedef RS_uint32_t u32;
    typedef RS_uint64_t u64;

    // floats
    typedef RS_float32_t f32;
    typedef RS_float64_t f64;

    // bool (engine-style fixed 8-bit)
    typedef RS_bool8_t b8;

    // byte
    typedef RS_uint8_t byte;

    // chars / code units
    typedef RS_char8_t    c8;     // plain 8-bit char
    typedef RS_u8char8_t  u8c;    // char8_t (C++20)
    typedef RS_char16_t   c16;    // char16_t
    typedef RS_char32_t   c32;    // char32_t
    typedef RS_wchar16_t  wc16;   // forced 16-bit wide
    typedef RS_wchar32_t  wc32;   // forced 32-bit wide

    // pointer-size integers (platform dependent)
#if INTPTR_MAX == INT64_MAX
    typedef RS_isize64_t isize;
    typedef RS_usize64_t usize;
#elif INTPTR_MAX == INT32_MAX
    typedef RS_isize32_t isize;
    typedef RS_usize32_t usize;
#else
#   error "Unsupported pointer size"
#endif

    // misc handy aliases
    typedef usize size;     // often used for sizes
    typedef isize ssize;    // signed size
} // namespace RSES
