.section .text
.globl _start

_start:
    addi x1, x0, 32

    # Initialize address 32 with 0x11223344
    lui  x2, 0x11223
    ori  x2, x2, 0x344
    sw   x2, 0(x1)

    # Prepare store byte value 0xAA in x4
    addi x4, x0, 170   # 0xAA

    # Store Byte to offset 1 (address 33)
    sb   x4, 1(x1)

    # Read back full word: expected 0x1122AA44 into x3
    lw   x3, 0(x1)

    # Read back individual bytes into x4
    lbu  x4, 1(x1)     # Modified byte -> expected 0xAA
    lbu  x4, 0(x1)     # Neighbor byte 0 -> expected 0x44
    lbu  x4, 2(x1)     # Neighbor byte 2 -> expected 0x22

loop:
    j loop
