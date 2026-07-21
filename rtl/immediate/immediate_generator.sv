`timescale 1ns/1ps

module immediate_generator
(
    /* verilator lint_off UNUSEDSIGNAL */
    input  logic [31:0] instruction,
    /* verilator lint_on UNUSEDSIGNAL */

    output logic [31:0] imm_i,
    output logic [31:0] imm_s,
    output logic [31:0] imm_b,
    output logic [31:0] imm_u,
    output logic [31:0] imm_j
);

    always_comb
    begin
        // I-Type Immediate
        imm_i = {
            {20{instruction[31]}},
            instruction[31:20]
        };

        // S-Type Immediate
        imm_s = {
            {20{instruction[31]}},
            instruction[31:25],
            instruction[11:7]
        };

        // B-Type Immediate
        imm_b = {
            {19{instruction[31]}},
            instruction[31],
            instruction[7],
            instruction[30:25],
            instruction[11:8],
            1'b0
        };

        // U-Type Immediate
        imm_u = {
            instruction[31:12],
            12'b0
        };

        // J-Type Immediate
        imm_j = {
            {11{instruction[31]}},
            instruction[31],
            instruction[19:12],
            instruction[20],
            instruction[30:21],
            1'b0
        };
    end

endmodule
