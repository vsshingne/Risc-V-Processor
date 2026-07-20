.section .text
.globl _start

_start:

    addi x1, x0, 5
    addi x2, x0, 10

    bge x1, x2, greater

    addi x3, x0, 100      # MUST BE SKIPPED

greater:

    addi x4, x0, 20

loop:
    j loop