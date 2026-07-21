.section .text
.globl _start

_start:

    addi x1, x0, 64
    addi x2, x0, -1

    sw   x2, 4(x1)
    lw   x3, 4(x1)

    addi x4, x3, 1

loop:
    j loop
