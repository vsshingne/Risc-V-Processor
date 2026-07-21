.section .text
.globl _start

_start:

    addi x1, x0, 32
    addi x2, x0, 1963

    sw   x2, 12(x1)
    lw   x3, 12(x1)

    addi x4, x3, 5

loop:
    j loop
