// https://operating-system-in-1000-lines.vercel.app/ja/exception
// https://github.com/nuta/microkernel-book/tree/main
// https://github.com/riscv-non-isa/riscv-sbi-doc/blob/master/riscv-sbi.adoc
// https://five-embeddev.com/riscv-isa-manual/latest/csr.html
// https://five-embeddev.com/riscv-isa-manual/latest/supervisor.html#scauses
// https://riscv.org/wp-content/uploads/2017/05/riscv-spec-v2.2.pdf
// https://github.com/riscv-software-src/opensbi/blob/master/docs/firmware/fw.md
// https://github.com/riscv-non-isa/riscv-sbi-doc

#include <p5k-base/base.h>
#include <riscv/riscv.h>
#include <sbi/sbi.h>

extern sym __kernel_start, __kernel_end;
extern sym __bss_start, __bss_end;

/* --- Kernel Base ---------------------------------------------------------- */

void p5k_panic(str fmt, ...) {
  va_list vargs;
  va_start(vargs, fmt);
  var io = sbi_console_io();
  io_print(io, _s("p5k: "));
  io_vprint(io, fmt, vargs);
  io_print(io, _s("\n\n(fatal error, system halted)\n"));
  va_end(vargs);

  sbi_system_reset(SBI_RESET_TYPE_SHUTDOWN, SBI_RESET_REASON_SYSTEM_FAILURE);
}

void p5k_log(str fmt, ...) {
  va_list vargs;
  va_start(vargs, fmt);
  var io = sbi_console_io();
  io_print(io, _s("p5k: "));
  io_vprint(io, fmt, vargs);
  io_putc(io, '\n');
  va_end(vargs);
}

void p5K_unreachable(void) { p5k_panic(_s("unreachable")); }

/* --- Trap Handling -------------------------------------------------------- */

typedef struct {
  usize ra, gp, tp, t0, t1, t2, t3, t4, t5, t6, a0, a1, a2, a3, a4, a5, a6, a7,
      s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, sp;
} __attribute__((packed)) p5k_frame;

extern void _p5k_trap(void);
void p5k_trap(p5k_frame *) {
  let scause = riscv_csrr(scause);
  let stval = riscv_csrr(stval);
  let sepc = riscv_csrr(sepc);

  p5k_panic(_s("trap: scause=%x, stval=%x, sepc=%x"), scause, stval, sepc);
}

/* --- Kernel Object -------------------------------------------------------- */

typedef struct {
} p5k_vmo;

typedef struct {
} p5k_space;

typedef struct {
} p5k_task;

typedef struct {
  usize id;

  enum p5k_type {
    P5K_TYPE_NONE,
    P5K_TYPE_TASK,
    P5K_TYPE_SPACE,
    P5K_TYPE_VMO,
  } type;

  union {
    p5k_task task;
    p5k_space space;
    p5k_vmo vmo;
  };
} p5k_object;

p5k_object *p5k_create(enum p5k_type type);

p5k_object *p5k_ref(p5k_object *obj);

p5k_object *p5k_deref(p5k_object *obj);

/* --- Kernel Entry Point --------------------------------------------------- */

void p5k_entry(usize hart, usize dtb) {
  mem_zero((bytes){__bss_end - __bss_start, __bss_start});
  sbi_console_putchar('\n');
  p5k_log(_s("p5k version 0.0.1"), hart, dtb);
  p5k_log(_s("hart=%x, dtb=%x"), hart, dtb);
  p5k_log(_s("kernel=%x-%x"), &__kernel_start, &__kernel_end);

  riscv_csrw(stvec, (usize)_p5k_trap);
  riscv_unimp();

  p5K_unreachable();
}