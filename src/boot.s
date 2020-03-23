                /* Multiboot flags */
                .set    MB_FLAG_ALIGN,   1 << 0 /* Align loaded modules on page boundaries */
                .set    MB_FLAG_MEMINFO, 1 << 1 /* Provide memory map */

                /* Other multiboot constants */
                .set    MB_FLAGS, MB_FLAG_ALIGN | MB_FLAG_MEMINFO
                .set    MB_MAGIC, 0x1BADB002    /* This magic number lets bootloader find the header */
                .set    MB_CHECKSUM, -(MB_MAGIC + MB_FLAGS)

                /*
                 * Multiboot header that marks the program as a kernel.
                 * It is placed in its own section so it can be forced to be within
                 * the first 8 KiB of the kernel file (as required by the standard).
                 */
                .section .multiboot
                .align  4
                .long   MB_MAGIC
                .long   MB_FLAGS
                .long   MB_CHECKSUM

                /*
                 * According to the multiboot standard bootloader will not initialize a stack for us.
                 * Place the stack in its own section so it can be marked nobits, which means
                 * the kernel file is smaller because it does not contain an uninitialized stack.
                 * The stack on x86 must be 16-byte aligned according to the System V ABI standard
                 * and de-facto extensions. The compiler will assume the stack is properly aligned
                 * and failure to align the stack will result in undefined behavior.
                 */
                .section .bss
                .align  16
stack_bottom:
                .skip   16 * 1024
stack_top:

                /*
                 * The linker script specifies _start as the entry point to the kernel and the
                 * bootloader will jump to this position once the kernel has been loaded. It
                 * doesn't make sense to return from this function as the bootloader is gone.
                 */
                .section .text
                .global _start
                .type   _start, @function
_start:
                mov     $stack_top, %esp        /* Set up stack for C part of the kernel */
                call    kernel_main

                /* If the system has nothing more to do, put the computer into an infinite loop. */
1:              cli
                hlt
                jmp     1b      /* Halt CPU again if it ever wakes up (e.g. due to a non-maskable interrupt
                                   occurring or due to system management mode). */

                /*
                 * Set the size of the _start symbol.
                 * This is useful when debugging or when call tracing is implemented.
                 */
                .size   _start, . - _start
