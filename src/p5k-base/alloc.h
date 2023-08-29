#pragma once

#include "base.h"

typedef struct {
  void *ctx;
  void *(*alloc)(void *ctx, usize n, void *buf);
} alloc;

inline void *alloc_alloc(alloc alloc, usize n) {
  return alloc.alloc(alloc.ctx, n, nil);
}

inline void *alloc_allocz(alloc alloc, usize n) {
  let buf = alloc.alloc(alloc.ctx, n, nil);
  if (buf == nil)
    return buf;
  mem_zero((bytes){n, buf});
  return buf;
}

inline void alloc_free(alloc alloc, void *buf) {
  alloc.alloc(alloc.ctx, 0, buf);
}

inline void *alloc_realloc(alloc alloc, usize n, void *buf) {
  return alloc.alloc(alloc.ctx, n, buf);
}
