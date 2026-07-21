.section .text
.globl _start

_start:
    addi x1, x0, 32

    # Initialize address 32 with 0x11223344
    lui  x2, 0x11223
    ori  x2, x2, 0x344
    sw   x2, 0(x1)

    # Prepare store halfword value 0xBBCC in x4
    addi x4, x0, 0xBB
    slli x4, x4, 8
    ori  x4, x4, 0xCC

    # Store Halfword to offset 2 (address 34)
    sh   x4, 2(x1)

    # Read back full word: expected 0xBBCC3344 into x3
    lw   x3, 0(x1)

    # Read back halfwords into x4
    lhu  x4, 2(x1)     # Modified halfword -> expected 0xBBCC
    lhu  x4, 0(x1)     # Neighbor halfword -> expected 0x3344

loop:
    j loop
