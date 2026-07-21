.section .text
.globl _start

_start:
    addi x1, x0, 32

    # Write test bytes using SW: 0x12FF807F
    lui  x2, 0x12FF8
    ori  x2, x2, 0x07F
    sw   x2, 0(x1)

    # LBU test cases
    lbu  x3, 0(x1)     # Load byte 0x7F -> expected 0x0000007F
    lbu  x4, 1(x1)     # Load byte 0x80 -> expected 0x00000080 (zero-extended)
    lbu  x3, 2(x1)     # Load byte 0xFF -> expected 0x000000FF (zero-extended)

loop:
    j loop
