# .section .text
# .globl _start

# _start:

#     li x1, 10
#     li x2, 5

#     bgeu x1, x2, greater

#     li x3, 100      # MUST BE SKIPPED

# greater:

#     li x4, 20

# loop:
#     j loop

.section .text
.globl _start

_start:

    li x1, 5
    li x2, 10

    bgeu x1, x2, greater

    li x3, 100

greater:

    li x4, 20

loop:
    j loop