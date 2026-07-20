# .section .text
# .globl _start

# _start:

#     addi x1, x0, 5
#     addi x2, x0, 5

#     beq x1, x2, equal

#     addi x3, x0, 100      # should be skipped

# equal:

#     addi x4, x0, 20

# loop:
#     j loop
.section .text
.globl _start

_start:

    addi x1, x0, 5
    addi x2, x0, 10

    beq x1, x2, equal

    addi x3, x0, 100      # MUST EXECUTE

equal:

    addi x4, x0, 20

loop:
    j loop
