.section .text
.globl _start

_start:

    # Phase 1: XOR, OR
    # Setup: x1 = 10 (0b1010), x2 = 12 (0b1100)

    addi  x1, x0, 10        # x1 = 10 (0x0000000A)
    addi  x2, x0, 12        # x2 = 12 (0x0000000C)

    xor   x3, x1, x2        # x3 = 10 ^ 12 = 6  (0b1010 ^ 0b1100 = 0b0110)
    or    x4, x1, x2        # x4 = 10 | 12 = 14 (0b1010 | 0b1100 = 0b1110)

    # Expected after tick 4: x1=10, x2=12, x3=6, x4=14

    # Phase 2: AND, SLT, SLTU, SLL

    and   x1, x1, x2        # x1 = 10 & 12 = 8  (0b1010 & 0b1100 = 0b1000)
    slt   x2, x1, x2        # x2 = (8 < 12) signed = 1
    sltu  x3, x2, x1        # x3 = (1 <u 8) unsigned = 1
    sll   x4, x1, x2        # x4 = 8 << (1 & 0x1F) = 8 << 1 = 16

    # Expected after tick 8: x1=8, x2=1, x3=1, x4=16

    # Phase 3: SRL, SRA
    # Use 0xFFFFFFFF to verify arithmetic shift sign extension

    srl   x1, x4, x2        # x1 = srl(16, 1) = 8  (logical right shift)
    addi  x2, x0, -1        # x2 = 0xFFFFFFFF (-1 sign-extended)
    sra   x3, x2, x1        # x3 = sra(0xFFFFFFFF, 8) = 0xFFFFFFFF (sign preserved)
    addi  x4, x0, 1         # x4 = 1 (sentinel: confirms prior insts completed)

    # Expected after tick 12: x1=8, x2=0xFFFFFFFF, x3=0xFFFFFFFF, x4=1

loop:
    j loop
