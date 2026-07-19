`timescale 1ns/1ps

module wb_mux
(
    input  logic [31:0] alu_result,
    input  logic [31:0] memory_data,

    input  logic        result_src,

    output logic [31:0] writeback_data
);

    always_comb
    begin
        writeback_data = alu_result;

        if (result_src)
            writeback_data = memory_data;
    end

endmodule