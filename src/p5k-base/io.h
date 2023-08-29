#pragma once

#include "base.h"

typedef struct {
  enum {
    IO_WHENCE_CURR,
    IO_WHENCE_START,
    IO_WHENCE_END,
  } type;
  isize off;
} io_whence;

inline io_whence io_whence_curr(isize off) {
  return (io_whence){IO_WHENCE_CURR, off};
}

inline io_whence io_whence_start(isize off) {
  return (io_whence){IO_WHENCE_START, off};
}

inline io_whence io_whence_end(isize off) {
  return (io_whence){IO_WHENCE_END, off};
}

typedef struct {
  void *ctx;
  res (*read)(void *, bytes);
  res (*write)(void *, bytes);
  res (*seek)(void *, io_whence);
  res (*flush)(void *);
} io;

res io_read(io io, bytes buf) { return io.read(io.ctx, buf); }

res io_write(io io, bytes buf) { return io.write(io.ctx, buf); }

res io_seek(io io, io_whence whence) { return io.seek(io.ctx, whence); }

res io_flush(io io) { return io.flush(io.ctx); }

res io_putc(io io, u8 c) { return io_write(io, (bytes){1, &c}); }

res io_vprint(io io, str fmt, va_list vargs) {
  usize written = 0;
  u8 const *f = fmt.buf;

  while (*f) {
    if (*f == '%') {
      f++;
      switch (*f) {
      case '\0':
      case '%':
        written += try(io_putc(io, '%')).uvalue;
        break;
      case 's': {
        const char *s = va_arg(vargs, const char *);
        while (*s) {
          written += try(io_putc(io, *s)).uvalue;
          s++;
        }
        break;
      }
      case 'd': {
        int value = va_arg(vargs, int);
        if (value < 0) {
          written += try(io_putc(io, '-')).uvalue;
          value = -value;
        }

        int divisor = 1;
        while (value / divisor > 9)
          divisor *= 10;

        while (divisor > 0) {
          written += try(io_putc(io, '0' + value / divisor)).uvalue;
          value %= divisor;
          divisor /= 10;
        }

        break;
      }
      case 'x': {
        int value = va_arg(vargs, int);
        for (int i = 7; i >= 0; i--) {
          int nibble = (value >> (i * 4)) & 0xf;
          written += try(io_putc(io, "0123456789abcdef"[nibble])).uvalue;
        }
      }
      }
    } else {
      written += try(io_putc(io, *f)).uvalue;
    }

    f++;
  }

  return uok(written);
}

res io_print(io io, str fmt, ...) {
  va_list vargs;
  va_start(vargs, fmt);
  res e = io_vprint(io, fmt, vargs);
  va_end(vargs);
  return e;
}
