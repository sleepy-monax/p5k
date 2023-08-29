#pragma once

#include "base.h"

#define HEAP_MAGIC 0xc0c0c0c0c0c0c0c0
#define HEAP_DEAD 0xdeaddeaddeaddead
#define HEAP_ALIGN (64)
#define HEAP_PAGE_SIZE (4096)
#define HEAP_MIN_REQU (4096 * 4)
#define HEAP_ALIGNED(X) (((X) + (HEAP_ALIGN - 1)) & ~(HEAP_ALIGN - 1))
#define HEAP_PAGE_ALIGNED(X)                                                   \
  (((X) + (HEAP_PAGE_SIZE - 1)) & ~(HEAP_PAGE_SIZE - 1))

typedef struct heap_node {
  union {
    u64 magic;
    u8 m[8];
  };
  struct heap_node *prev;
  struct heap_node *next;
} heap_node;

#define HEAP_NODE(T)                                                           \
  union {                                                                      \
    heap_node base;                                                            \
    struct {                                                                   \
      u64 magic;                                                               \
      T *prev;                                                                 \
      T *next;                                                                 \
    };                                                                         \
  }

typedef struct heap_major {
  HEAP_NODE(struct heap_major);

  usize size;
  usize used;
  struct heap_minor *minor;
} heap_major;

typedef struct heap_minor {
  HEAP_NODE(struct heap_minor);

  usize size;
  usize used;
  struct heap_major *major;
} heap_minor;

typedef void HeapFreeBlockFn(void *ctx, void *ptr, usize size);

enum HeapLogType {
  HEAP_TRACE,
  HEAP_ERROR,
};

typedef struct {
  void *ctx;
  void *(*alloc)(void *ctx, usize size);
  void (*free)(void *ctx, void *ptr, usize size);
  void (*log)(void *ctx, enum HeapLogType type, cstr fmt, va_list args);

  heap_major *root;
  heap_major *best;
} heap;

/* ---- Internal functions -------------------------------------------------- */

/* Heap hook functions */

void *heap_alloc_block(heap *heap, usize size);

void heap_free_block(heap *heap, void *ptr, usize size);

void heap_trace(heap *heap, cstr msg, ...);

void heap_error(heap *heap, cstr msg, ...);

/* Heap node functions */

bool heap_node_check(heap *heap, heap_node *node);

void heap_node_append(heap_node *node, heap_node *other);

void heap_node_prepend(heap_node *node, heap_node *other);

void heap_node_remove(heap_node *node);

/* Heap major functions */

usize heap_major_avail(struct heap_major *maj);

struct heap_major *heap_major_create(heap *heap, usize size);

heap_minor *heap_major_alloc(heap *heap, struct heap_major *maj, usize size);

void heap_major_free(heap *heap, struct heap_major *maj);

/* Heap minor functions */

usize heap_minor_avail(heap_minor *min);

heap_minor *heap_minor_create(struct heap_major *maj, usize size);

heap_minor *heap_minor_split(heap_minor *min, usize size);

void heap_minor_free(heap *heap, heap_minor *min);

void heap_minor_resize(heap_minor *min, usize size);

heap_minor *heap_minor_from(void *ptr);

void *heap_minor_to(heap_minor *min);

/* ---- Public functions ---------------------------------------------------- */

void *heap_alloc(heap *heap, usize size);

void *heap_realloc(heap *heap, void *ptr, usize size);

void *heap_calloc(heap *heap, usize num, usize size);

void heap_free(heap *heap, void *ptr);
