#pragma once

#include "base.h"
#include "io.h"

typedef struct {
  bytes buf;
  u8 const *curr;
} cursor;

cursor cursor_make(bytes b) { return (cursor){b, b.buf}; }

usize cursor_rem(cursor m ref) { return m->buf.len - (m->curr - m->buf.buf); }

res cursor_read(cursor m ref, usize n, u8 buf[n]) {
  if (n > cursor_rem(m)) {
    n = cursor_rem(m);
  }

  for (usize i = 0; i < n; i++) {
    buf[i] = m->curr[i];
  }

  m->curr += n;

  return uok(n);
}

res cursor_seek(cursor m ref, usize n, io_whence w) {
  switch (w.type) {
  case IO_WHENCE_CURR:
    if (n > cursor_rem(m)) {
      return err(RES_OUT_OF_BOUNDS);
    }

    m->curr += n;
    break;
  case IO_WHENCE_START:
    if (n > m->buf.len) {
      return err(RES_OUT_OF_BOUNDS);
    }

    m->curr = m->buf.buf + n;
    break;
  case IO_WHENCE_END:
    if (n > m->buf.len) {
      return err(RES_OUT_OF_BOUNDS);
    }

    m->curr = m->buf.buf + m->buf.len - n;
    break;
  }

  return uok(m->curr - m->buf.buf);
}

res cursor_u8be(cursor m ref, u8 *v) { return cursor_read(m, 1, v); }

res cursor_u16be(cursor m ref, u16 *v) {
  let res = cursor_read(m, 2, (u8 *)v);
  *v = bswap16(*v);
  return res;
}

res cursor_u32be(cursor m ref, u32 *v) {
  let res = cursor_read(m, 4, (u8 *)v);
  *v = bswap32(*v);
  return res;
}

res cursor_u64be(cursor m ref, u64 *v) {
  let res = cursor_read(m, 8, (u8 *)v);
  *v = bswap64(*v);
  return res;
}
