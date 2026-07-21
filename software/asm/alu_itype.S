.section .text
.globl _start

_start:

    # Phase 1 — XORI, ORI, ANDI
    # Operand: x1 = 10 (0b1010), immediate = 12 (0b1100)

    addi  x1, x0, 10        # x1 = 10   (0b1010)
    xori  x2, x1, 12        # x2 = 10 ^ 12 = 6    (0b1010 ^ 0b1100 = 0b0110)
    ori   x3, x1, 12        # x3 = 10 | 12 = 14   (0b1010 | 0b1100 = 0b1110)
    andi  x4, x1, 12        # x4 = 10 & 12 = 8    (0b1010 & 0b1100 = 0b1000)

    # Expected after tick 4 (PC=0x10): x1=10, x2=6, x3=14, x4=8

    # Phase 2 — SLTI, SLTIU (with negative immediate edge cases)
    #
    # Edge case: imm = -1  →  12-bit sign-extended  →  0xFFFFFFFF
    #   SLTI:  0 <s  -1       →  false (0 is greater than -1 in signed)
    #   SLTIU: 0 <u  0xFFFFFFFF  →  true  (0 is less than 4294967295 unsigned)

    slti  x1, x0, 1         # x1 = (0 <s 1) = 1   (normal signed less-than)
    sltiu x2, x0, 1         # x2 = (0 <u 1) = 1   (normal unsigned less-than)
    slti  x3, x0, -1        # x3 = (0 <s -1) = 0  (0 > -1 in signed arithmetic)
    sltiu x4, x0, -1        # x4 = (0 <u 0xFFFFFFFF) = 1  (max unsigned value)

    # Expected after tick 8 (PC=0x20): x1=1, x2=1, x3=0, x4=1

    # Phase 3 — SLLI, SRLI, SRAI (sign-bit behavior on 0xFFFFFFFF)
    #
    # All three shifts applied to -1 (0xFFFFFFFF), shift amount = 1
    #   SLLI: logical left   →  0xFFFFFFFE  (bit 0 dropped, 0 inserted at LSB)
    #   SRLI: logical right  →  0x7FFFFFFF  (MSB becomes 0 — no sign extension)
    #   SRAI: arith  right   →  0xFFFFFFFF  (MSB stays 1 — sign preserved)
     

    addi  x1, x0, -1        # x1 = 0xFFFFFFFF  (operand setup)
    slli  x2, x1, 1         # x2 = 0xFFFFFFFF << 1 = 0xFFFFFFFE
    srli  x3, x1, 1         # x3 = 0xFFFFFFFF >> 1 = 0x7FFFFFFF  (logical)
    srai  x4, x1, 1         # x4 = sra(0xFFFFFFFF, 1) = 0xFFFFFFFF  (arithmetic)

    # Expected after tick 12 (PC=0x30):
    #   x1=0xFFFFFFFF, x2=0xFFFFFFFE, x3=0x7FFFFFFF, x4=0xFFFFFFFF

     
    # Phase 4 — Maximum shift amount (31): SRLI vs SRAI distinction
    #
    #   SRLI shamt=31: 0xFFFFFFFF >> 31 = 0x00000001  (only bit 31 survives)
    #   SRAI shamt=31: sra(-1, 31) = 0xFFFFFFFF       (all bits become sign bit)
    #   SLLI shamt=31: 0xFFFFFFFF << 31 = 0x80000000  (only LSB survives in bit 31)
     

    addi  x1, x0, -1        # x1 = 0xFFFFFFFF  (reload)
    srli  x2, x1, 31        # x2 = 0xFFFFFFFF >> 31 = 0x00000001  (logical, max shift)
    srai  x3, x1, 31        # x3 = sra(0xFFFFFFFF, 31) = 0xFFFFFFFF  (arithmetic, max shift)
    slli  x4, x1, 31        # x4 = 0xFFFFFFFF << 31 = 0x80000000  (left shift, max)

    # Expected after tick 16 (PC=0x40):
    #   x1=0xFFFFFFFF, x2=0x00000001, x3=0xFFFFFFFF, x4=0x80000000

loop:
    j loop
