#ifndef ALU_OPS_HPP
#define ALU_OPS_HPP

#include <cstdint>

enum ALU_OP : uint8_t
{
    ALU_ADD  = 0,
    ALU_SUB  = 1,
    ALU_AND  = 2,
    ALU_OR   = 3,
    ALU_XOR  = 4,
    ALU_SLL  = 5,
    ALU_SRL  = 6,
    ALU_SRA  = 7,
    ALU_SLT  = 8,
    ALU_SLTU = 9
};

#endif