# .section .text
# .globl _start

# _start:

#     addi x1, x0, 5
#     addi x2, x0, 10

#     blt x1, x2, less

#     addi x3, x0, 100      # MUST BE SKIPPED

# less:

#     addi x4, x0, 20

# loop:
#     j loop

.section .text
.globl _start

_start:

    addi x1, x0, 10
    addi x2, x0, 5

    blt x1, x2, less

    addi x3, x0, 100      # MUST EXECUTE

less:

    addi x4, x0, 20

loop:
    j loop