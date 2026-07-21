.section .text
.globl _start

_start:
    addi x1, x0, 32

    # Write test word using SW: 0x80001234
    lui  x2, 0x80001
    ori  x2, x2, 0x234
    sw   x2, 0(x1)

    # LH test cases
    lh   x3, 0(x1)     # Load positive halfword 0x1234 -> expected 0x00001234
    lh   x4, 2(x1)     # Load negative halfword 0x8000 -> expected 0xFFFF8000

    # Unaligned LH test case (address 33 is unaligned for halfword)
    lh   x3, 1(x1)     # Unaligned -> expected 0x00000000

loop:
    j loop
