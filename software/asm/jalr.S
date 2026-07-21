.section .text
.globl _start

_start:
    # Phase 1: Basic JALR & return address (PC+4 -> x2) with positive offset 0
    # PC = 0x00: addi x1, x0, 20    (x1 = 0x14 = target1)
    # PC = 0x04: jalr x2, x1, 0     (target = 0x14, x2 = 0x08)
    # PC = 0x08, 0x0C, 0x10: nops/skipped
    # PC = 0x14: target1: addi x3, x0, 42 (x3 = 42)

    addi x1, x0, 20
    jalr x2, x1, 0
    addi x3, x0, 255
    addi x3, x0, 255
    addi x3, x0, 255

target1:
    addi x3, x0, 42

    # Phase 2: JALR with positive immediate & odd target address bit 0 clearing
    # PC = 0x18: addi x1, x0, 39    (x1 = 0x27, odd address!)
    # PC = 0x1C: jalr x4, x1, 5     (target = (0x27 + 5) & ~1 = 0x2C, x4 = 0x20)
    # PC = 0x20, 0x24, 0x28: skipped
    # PC = 0x2C: target2: addi x3, x3, 1 (x3 = 43)

    addi x1, x0, 39
    jalr x4, x1, 5
    addi x3, x0, 255
    addi x3, x0, 255
    addi x3, x0, 255

target2:
    addi x3, x3, 1

    # Phase 3: JALR with negative immediate
    # PC = 0x30: addi x1, x0, 80    (x1 = 0x50)
    # PC = 0x34: jalr x2, x1, -12   (target = (0x50 - 12) & ~1 = 0x44, x2 = 0x38)
    # PC = 0x38, 0x3C, 0x40: skipped
    # PC = 0x44: target3: addi x3, x3, 1 (x3 = 44)

    addi x1, x0, 80
    jalr x2, x1, -12
    addi x3, x0, 255
    addi x3, x0, 255
    addi x3, x0, 255

target3:
    addi x3, x3, 1

    # Phase 4: JALR with rd = x0 (discard return address)
    # PC = 0x48: addi x1, x0, 92    (x1 = 0x5C = target4)
    # PC = 0x4C: jalr x0, x1, 0     (target = 0x5C, x0 stays 0)
    # PC = 0x50, 0x54, 0x58: skipped
    # PC = 0x5C: target4: addi x4, x0, 99 (x4 = 99)

    addi x1, x0, 92
    jalr x0, x1, 0
    addi x3, x0, 255
    addi x3, x0, 255
    addi x3, x0, 255

target4:
    addi x4, x0, 99

loop:
    j loop
