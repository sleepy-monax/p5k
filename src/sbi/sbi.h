#pragma once

#include <p5k-base/base.h>
#include <p5k-base/io.h>

enum sbierr {
  SBI_SUCCESS = 0,
  SBI_ERR_FAILED = -1,
  SBI_ERR_NOT_SUPPORTED = -2,
  SBI_ERR_INVALID_PARAM = -3,
  SBI_ERR_DENIED = -4,
  SBI_ERR_INVALID_ADDRESS = -5,
  SBI_ERR_ALREADY_AVAILABLE = -6,
  SBI_ERR_ALREADY_STARTED = -7,
  SBI_ERR_ALREADY_STOPPED = -8,
  SBI_ERR_NO_SHMEM = -9,
};

typedef struct {
  long error;
  long value;
} sbiret;

sbiret _sbi_call_impl(long arg0, long arg1, long arg2, long arg3, long arg4,
                      long arg5, long fid, long eid) {
  register long a0 __asm__("a0") = arg0;
  register long a1 __asm__("a1") = arg1;
  register long a2 __asm__("a2") = arg2;
  register long a3 __asm__("a3") = arg3;
  register long a4 __asm__("a4") = arg4;
  register long a5 __asm__("a5") = arg5;
  register long a6 __asm__("a6") = fid;
  register long a7 __asm__("a7") = eid;

  __asm__ __volatile__("ecall"
                       : "=r"(a0), "=r"(a1)
                       : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
                         "r"(a6), "r"(a7)
                       : "memory");

  return (sbiret){.error = a0, .value = a1};
}

#define _sbi_call(eid, fid, arg0, arg1, arg2, arg3, arg4, arg5, ...)           \
  _sbi_call_impl(arg0, arg1, arg2, arg3, arg4, arg5, fid, eid)

#define sbi_call(eid, ...) _sbi_call(eid, __VA_ARGS__, 0, 0, 0, 0, 0, 0)

/* --- Base Extension ------------------------------------------------------- */

#define SBI_BASE_EXT_ID (0x10)

sbiret sbi_get_spec_version(void) { return sbi_call(SBI_BASE_EXT_ID, 0); }

#define SBI_IMPL_FOREACH(ITER)                                                 \
  ITER(BERKELEY_BOOT_LOADER, 0)                                                \
  ITER(OPENSBI, 1)                                                             \
  ITER(XVISOR, 2)                                                              \
  ITER(KVM, 3)                                                                 \
  ITER(RUSTSBI, 4)                                                             \
  ITER(DIOSIX, 5)                                                              \
  ITER(COFFER, 6)                                                              \
  ITER(XEN, 7)                                                                 \
  ITER(POLARFIRE_HSS, 8)

enum sbi_impl_id {
#define ITER(ID, VAL) SBI_IMPL_ID_##ID = VAL,
  SBI_IMPL_FOREACH(ITER)
#undef ITER
};

str sbi_impl_id_to_str(enum sbi_impl_id id) {
  switch (id) {
#define ITER(ID, VAL)                                                          \
  case SBI_IMPL_ID_##ID:                                                       \
    return _s(#ID);
    SBI_IMPL_FOREACH(ITER)
#undef ITER
  default:
    return _s("UNKNOWN");
  }
}

sbiret sbi_get_impl_id(void) { return sbi_call(SBI_BASE_EXT_ID, 1); }

sbiret sbi_get_impl_version(void) { return sbi_call(SBI_BASE_EXT_ID, 2); }

sbiret sbi_probe_extension(long extension_id) {
  return sbi_call(SBI_BASE_EXT_ID, 3, extension_id);
}

sbiret sbi_get_mvendorid(void) { return sbi_call(SBI_BASE_EXT_ID, 4); }

sbiret sbi_get_marchid(void) { return sbi_call(SBI_BASE_EXT_ID, 5); }

sbiret sbi_get_mimpid(void) { return sbi_call(SBI_BASE_EXT_ID, 6); }

/* --- Legacy Extension ----------------------------------------------------- */

long sbi_console_putchar(u8 ch) { return sbi_call(0x1, 0, ch).error; }

long sbi_console_getchar(void) { return sbi_call(0x2, 0).value; }

res _sbi_console_write(void *, bytes buf) {
  for (usize i = 0; i < buf.len; i++)
    sbi_console_putchar(buf.buf[i]);
  return uok(buf.len);
}

io sbi_console_io(void) {
  return (io){
      .write = _sbi_console_write,
  };
}

/* --- System Reset Extension ----------------------------------------------- */

#define SBI_SYSTEM_RESET_EXT_ID (0x53525354)

enum sbi_system_reset_type {
  SBI_RESET_TYPE_SHUTDOWN = 0,
  SBI_RESET_TYPE_COLD_REBOOT = 1,
  SBI_RESET_TYPE_WARM_REBOOT = 2,
};

enum sbi_system_reset_reason {
  SBI_RESET_REASON_NONE = 0,
  SBI_RESET_REASON_SYSTEM_FAILURE = 1,
  SBI_RESET_REASON_SYSTEM_SHUTDOWN = 2,
};

sbiret sbi_system_reset(enum sbi_system_reset_type reset_type,
                        enum sbi_system_reset_reason reset_reason) {
  return sbi_call(SBI_SYSTEM_RESET_EXT_ID, 0, reset_type, reset_reason);
}

/* --- Debug Console Extension ---------------------------------------------- */

#define SBI_DEBUG_CONSOLE_EXT_ID (0x4442434E)

sbiret sbi_debug_console_write_byte(u8 byte) {
  return sbi_call(SBI_DEBUG_CONSOLE_EXT_ID, 0x2, byte);
}