.section .text
.globl _start

_start:

    addi x1, x0, 100
    addi x2, x0, 200

loop:
    j loop