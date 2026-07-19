`timescale 1ns/1ps

module alu_mux
(
    input  logic [31:0] rs2_data,
    input  logic [31:0] imm_data,

    input  logic        alu_src,

    output logic [31:0] alu_operand_b
);

    always_comb
    begin
        alu_operand_b = rs2_data;

        if (alu_src)
            alu_operand_b = imm_data;
    end

endmodule