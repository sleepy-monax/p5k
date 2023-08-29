#pragma once

#include "base.h"

typedef struct {
  void *ctx;
  void (*fn)(void *);
} defer;

static inline void _defer_cleanup(defer m ref) { m->fn(m->ctx); }

#define defer(expr)                                                            \
  __attribute__((cleanup(_defer_cleanup))) defer _defer_##__LINE__ = (expr)