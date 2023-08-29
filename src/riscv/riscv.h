#pragma once

#include <p5k-base/base.h>

#define riscv_csrr(reg)                                                        \
  ({                                                                           \
    usize __tmp;                                                               \
    __asm__ __volatile__("csrr %0, " #reg : "=r"(__tmp));                      \
    __tmp;                                                                     \
  })

#define riscv_csrw(reg, value)                                                 \
  ({                                                                           \
    usize __tmp = (value);                                                     \
    __asm__ __volatile__("csrw " #reg ", %0" ::"r"(__tmp));                    \
  })

void riscv_unimp() { __asm__ __volatile__("unimp"); }

void riscv_wfi() { __asm__ __volatile__("wfi"); }

void riscv_di() { __asm__ __volatile__("csrci mstatus, 8"); }

void riscv_ei() { __asm__ __volatile__("csrsi mstatus, 8"); }