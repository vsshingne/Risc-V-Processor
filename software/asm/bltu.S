# .section .text
# .globl _start

# _start:

#     li x1, 5
#     li x2, 10

#     bltu x1, x2, less

#     li x3, 100          # MUST BE SKIPPED

# less:

#     li x4, 20

# loop:
#     j loop

.section .text
.globl _start

_start:

    li x1, 10
    li x2, 5

    bltu x1, x2, less

    li x3, 100

less:

    li x4, 20

loop:
    j loop