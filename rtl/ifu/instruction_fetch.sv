`timescale 1ns/1ps

module instruction_fetch
(
    input  logic        clk,
    input  logic        reset,
    input  logic        pc_write,

    input  logic [31:0] pc_next,

    output logic [31:0] pc,
    output logic [31:0] instruction
);

program_counter pc_inst
(
    .clk(clk),
    .reset(reset),
    .pc_write(pc_write),
    .pc_next(pc_next),
    .pc(pc)
);

instruction_memory imem_inst
(
    .address(pc),
    .instruction(instruction)
);

endmodule