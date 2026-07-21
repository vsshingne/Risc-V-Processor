.section .text
.globl _start

_start:

    # Phase 1: LUI (positive, max-field), AUIPC (non-zero and zero offsets)
    # -------------------------------------------------------------------------

    lui    x1, 1            # x1 = 1 << 12 = 0x00001000          (PC = 0x00)
    lui    x2, 0xFFFFF      # x2 = 0xFFFFF << 12 = 0xFFFFF000    (PC = 0x04)
    auipc  x3, 1            # x3 = 0x08 + 0x1000 = 0x00001008    (PC = 0x08)
    auipc  x4, 0            # x4 = 0x0C + 0 = 0x0000000C         (PC = 0x0C)

    # Expected after tick 4 (PC=0x10):
    #   x1=0x00001000  x2=0xFFFFF000  x3=0x00001008  x4=0x0000000C

    # Phase 2: LUI zero-imm, AUIPC zero-imm, LUI MSB, AUIPC large neg-imm
    # -------------------------------------------------------------------------

    lui    x1, 0            # x1 = 0                              (PC = 0x10)
    auipc  x2, 0            # x2 = 0x14 + 0 = 0x00000014         (PC = 0x14)
    lui    x3, 0x80000      # x3 = 0x80000 << 12 = 0x80000000    (PC = 0x18)
    auipc  x4, 0xFFFFF      # x4 = 0x1C + 0xFFFFF000 = 0xFFFFF01C (PC = 0x1C)

    # Expected after tick 8 (PC=0x20):
    #   x1=0x00000000  x2=0x00000014  x3=0x80000000  x4=0xFFFFF01C

loop:
    j loop
