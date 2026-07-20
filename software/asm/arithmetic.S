.section .text
.globl _start

_start:

    addi x1, x0, 5
    addi x2, x0, 10
    add  x3, x1, x2
    sub  x4, x3, x2

loop:
    j loop