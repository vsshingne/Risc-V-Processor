.section .text
.globl _start

_start:

    addi x1, x0, 5

    j target

    addi x2, x0, 100      # Should be skipped

target:

    addi x3, x0, 20

loop:
    j loop