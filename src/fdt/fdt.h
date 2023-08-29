#pragma once

#include <p5k-base/cursor.h>

#define FDT_MAGIC 0xd00dfeed

typedef struct {
  enum {
    FDT_BEGIN_NODE = 1,
    FDT_END_NODE = 2,
    FDT_PROP = 3,
    FDT_NOP = 4,
    FDT_END = 9,
  } type;
} fdt_tok;

typedef struct {
  u32 magic;
  u32 totalsize;
  u32 off_dt_struct;
  u32 off_dt_strings;
  u32 off_mem_rsvmap;
  u32 version;
  u32 last_comp_version;
  u32 boot_cpuid_phys;
  u32 size_dt_strings;
  u32 size_dt_struct;
} fdt_header;

struct fdt {
  cursor cur;
};

struct fdt_node {};

/* --- Parser --------------------------------------------------------------- */

res fdt_parse_header(cursor c ref, fdt_header h ref) {
  try(cursor_u32be(c, &h->magic));
  try(cursor_u32be(c, &h->totalsize));
  try(cursor_u32be(c, &h->off_dt_struct));
  try(cursor_u32be(c, &h->off_dt_strings));
  try(cursor_u32be(c, &h->off_mem_rsvmap));
  try(cursor_u32be(c, &h->version));
  try(cursor_u32be(c, &h->last_comp_version));
  try(cursor_u32be(c, &h->boot_cpuid_phys));
  try(cursor_u32be(c, &h->size_dt_strings));
  try(cursor_u32be(c, &h->size_dt_struct));

  return ok();
}


