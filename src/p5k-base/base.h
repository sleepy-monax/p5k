#pragma once

#include <stdarg.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define nil ((void *)0)
#define let __auto_type const
#define var __auto_type
#define ref [static 1]

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t usize;
typedef ptrdiff_t isize;

typedef __fp16 f16;
typedef float f32;
typedef double f64;

typedef u8 sym[];

#define bswap16(x) __builtin_bswap16(x)
#define bswap32(x) __builtin_bswap32(x)
#define bswap64(x) __builtin_bswap64(x)

typedef u8 byte;

typedef struct {
  usize len;
  u8 *buf;
} bytes;

typedef struct {
  enum res_type {
    RES_OK,
    RES_OUT_OF_BOUNDS,
  } type;

  union {
    usize uvalue;
    isize ivalue;
  };
} res;

res ok() { return (res){RES_OK, .uvalue = 0}; }
res iok(isize v) { return (res){RES_OK, .ivalue = v}; }
res uok(usize v) { return (res){RES_OK, .uvalue = v}; }
res err(enum res_type t) { return (res){t, .uvalue = 0}; }

#define try(expr)                                                              \
  ({                                                                           \
    res _res = (expr);                                                         \
    if (_res.type != RES_OK) {                                                 \
      return _res;                                                             \
    }                                                                          \
    _res;                                                                      \
  })

typedef struct {
  usize len;
  u8 const *buf;
} str;

typedef char const *cstr;

#define _s(s) ((str){sizeof(s) - 1, (u8 const *)(s)})

bytes mem_zero(bytes buf) {
  for (usize i = 0; i < buf.len; i++)
    buf.buf[i] = 0;
  return buf;
}

bytes mem_copy(bytes dst, bytes src) {
  for (usize i = 0; i < dst.len && i < src.len; i++)
    dst.buf[i] = src.buf[i];
  return dst;
}