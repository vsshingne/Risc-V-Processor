`timescale 1ns/1ps

import riscv_pkg::*;

module control_unit
(
    input  logic [6:0] opcode,
    input  logic [2:0] funct3,
    input  logic [6:0] funct7,

    output logic reg_write,
    output logic mem_read,
    output logic mem_write,

    output logic branch,
    output logic jump,

    output logic alu_src,

    output logic result_src,

    output alu_op_t  alu_op,
    output mem_size_t mem_size,
    output logic mem_unsigned,
    output imm_sel_t imm_sel,
    output logic branch_not_equal,
    output logic branch_less_than,
    output logic branch_greater_equal,

    output logic branch_less_than_unsigned,
    output logic branch_greater_equal_unsigned,

    output logic [1:0] alu_a_sel
);




always_comb
begin

    // Default values
    reg_write = 0;
    mem_read = 0;
    mem_write = 0;
    branch = 0;
    jump = 0;
    alu_src = 0;
    result_src = 0;
    branch_not_equal = 0;   
    branch_less_than = 0;
    branch_greater_equal = 0;
    branch_less_than_unsigned = 0;
    branch_greater_equal_unsigned = 0;

    alu_op = ALU_ADD;
    mem_size = MEM_WORD;
    mem_unsigned = 0;
    imm_sel = IMM_I;
    alu_a_sel = 2'b00;

    case (opcode)

          
        // R-Type
          
        7'b0110011:
        begin
            reg_write = 1;
            alu_src = 0;

            case ({funct7, funct3})

                {7'b0000000,3'b000}: alu_op = ALU_ADD;
                {7'b0100000,3'b000}: alu_op = ALU_SUB;
                {7'b0000000,3'b111}: alu_op = ALU_AND;
                {7'b0000000,3'b110}: alu_op = ALU_OR;
                {7'b0000000,3'b100}: alu_op = ALU_XOR;
                {7'b0000000,3'b001}: alu_op = ALU_SLL;
                {7'b0000000,3'b101}: alu_op = ALU_SRL;
                {7'b0100000,3'b101}: alu_op = ALU_SRA;
                {7'b0000000,3'b010}: alu_op = ALU_SLT;
                {7'b0000000,3'b011}: alu_op = ALU_SLTU;

                default: alu_op = ALU_ADD;

            endcase
        end

          
        // I-Type Arithmetic (ADDI, ANDI, ORI, ...)
          
        7'b0010011:
        begin
            reg_write = 1;
            alu_src = 1;
            imm_sel = IMM_I;

            case (funct3)

                3'b000: alu_op = ALU_ADD;   // ADDI
                3'b111: alu_op = ALU_AND;   // ANDI
                3'b110: alu_op = ALU_OR;    // ORI
                3'b100: alu_op = ALU_XOR;   // XORI
                3'b010: alu_op = ALU_SLT;   // SLTI
                3'b011: alu_op = ALU_SLTU;  // SLTIU

                3'b001:
                    alu_op = ALU_SLL;       // SLLI

                3'b101:
                begin
                    if(funct7 == 7'b0000000)
                        alu_op = ALU_SRL;   // SRLI
                    else if(funct7 == 7'b0100000)
                        alu_op = ALU_SRA;   // SRAI
                end

                default: alu_op = ALU_ADD;

            endcase
        end

          
        // LOAD
          
        7'b0000011:
        begin
            imm_sel = IMM_I;
            alu_op = ALU_ADD;
            mem_size = MEM_WORD;
            mem_unsigned = 0;

            case (funct3)

                3'b010:
                begin
                    reg_write = 1;
                    mem_read = 1;
                    alu_src = 1;
                    result_src = 1;
                end

                default:
                begin
                    reg_write = 0;
                    mem_read = 0;
                    alu_src = 0;
                    result_src = 0;
                end

            endcase
        end

          
        // STORE
          
        7'b0100011:
        begin
            imm_sel = IMM_S;
            alu_op = ALU_ADD;
            mem_size = MEM_WORD;

            case (funct3)

                3'b010:
                begin
                    mem_write = 1;
                    alu_src = 1;
                end

                default:
                begin
                    mem_write = 0;
                    alu_src = 0;
                end

            endcase
        end

        // JAL
        7'b1101111:
        begin
            reg_write = 1;

            jump = 1;

            imm_sel = IMM_J;

            // rd <- PC + 4
            result_src = 0;

            alu_op = ALU_ADD;
        end
          
        // BRANCH
          
        7'b1100011:
        begin
            imm_sel = IMM_B;
            alu_op  = ALU_SUB;

            case (funct3)

                // BEQ
                3'b000:
                begin
                    branch = 1;
                    branch_not_equal = 0;
                    branch_less_than = 0;
                end

                // BNE
                3'b001:
                begin
                    branch = 1;
                    branch_not_equal = 1;
                    branch_less_than = 0;
                end

                // BLT
                3'b100:
                begin
                    branch = 1;
                    branch_not_equal = 0;
                    branch_less_than = 1;
                end

                // BGE
                3'b101:
                begin
                    branch = 1;
                    branch_not_equal = 0;
                    branch_less_than = 0;
                    branch_greater_equal = 1;
                end

                default:
                begin
                    branch = 0;
                    branch_not_equal = 0;
                    branch_less_than = 0;
                end

                // BLTU
                3'b110:
                begin
                    branch = 1;
                    branch_less_than_unsigned = 1;
                end

                // BGEU
                3'b111:
                begin
                    branch = 1;
                    branch_greater_equal_unsigned = 1;
                end

            endcase
        end

        // LUI — rd = 0 + imm_u  (operand A forced to zero)
        7'b0110111:
        begin
            reg_write  = 1;
            alu_src    = 1;
            imm_sel    = IMM_U;
            alu_op     = ALU_ADD;
            alu_a_sel  = 2'b10;
        end

        // AUIPC — rd = PC + imm_u  (operand A is current PC)
        7'b0010111:
        begin
            reg_write  = 1;
            alu_src    = 1;
            imm_sel    = IMM_U;
            alu_op     = ALU_ADD;
            alu_a_sel  = 2'b01;
        end

        default:
        begin
            // Keep default values
        end

    endcase

end

endmodule
