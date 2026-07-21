`timescale 1ns/1ps

module wb_mux
(
    input  logic [31:0] alu_result,
    input  logic [31:0] memory_data,
    input  logic [31:0] pc,

    input  logic        result_src,
    input  logic        jump,

    output logic [31:0] writeback_data
);

    always_comb
    begin
        if (jump)
            writeback_data = pc + 32'd4;
        else if (result_src)
            writeback_data = memory_data;
        else
            writeback_data = alu_result;
    end

endmodule
