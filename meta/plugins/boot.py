import os

from cutekit import args, builder, cmds, shell

def bootCmd(args: args.Args) -> None:
    kernel = builder.build('p5k-core', 'riscv32-kernel')

    qemu = [
        "qemu-system-riscv32",
        "-machine", "virt",
        "-bios", "default",
        "-nographic",
        "-serial", "mon:stdio",
        "--no-reboot",
        "-kernel", kernel.outfile()
    ]

    shell.exec(*qemu)


cmds.append(cmds.Cmd('B', 'boot', 'Boot the kernel', bootCmd))