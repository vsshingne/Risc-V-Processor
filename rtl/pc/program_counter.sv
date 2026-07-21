`timescale 1ns/1ps

module program_counter
(
    input logic clk,
    input logic reset,
    input logic pc_write,

    input logic [31:0] pc_next,

    output logic [31:0] pc
);

    always_ff @(posedge clk)
    begin
        if (reset)
            pc <= 32'd0;
        else if (pc_write)
            pc <= pc_next;
    end

endmodule
