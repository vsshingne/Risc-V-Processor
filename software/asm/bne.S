.section .text
.globl _start

_start:

    addi x1, x0, 5
    addi x2, x0, 5

    bne x1, x2, notequal

    addi x3, x0, 100      # MUST EXECUTE

notequal:

    addi x4, x0, 20

loop:
    j loop