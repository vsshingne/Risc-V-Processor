.section .text
.globl _start

_start:
    addi x1, x0, 32

    # Write test word using SW: 0x80001234
    lui  x2, 0x80001
    ori  x2, x2, 0x234
    sw   x2, 0(x1)

    # LHU test cases
    lhu  x3, 0(x1)     # Load halfword 0x1234 -> expected 0x00001234
    lhu  x4, 2(x1)     # Load halfword 0x8000 -> expected 0x00008000 (zero-extended)

    # Unaligned LHU test case (address 33)
    lhu  x3, 1(x1)     # Unaligned -> expected 0x00000000

loop:
    j loop
