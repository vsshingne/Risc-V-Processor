`timescale 1ns/1ps

module instruction_memory
(
    /* verilator lint_off UNUSEDSIGNAL */
    input  logic [31:0] address,
    /* verilator lint_on UNUSEDSIGNAL */

    output logic [31:0] instruction
);

    // 256 x 32-bit ROM
    logic [31:0] memory [0:255];

    // Load Program Binary Hex
    initial
    begin
        $readmemh("software/program.hex", memory);
    end

    // Word Addressing (Fetch instruction word)
    assign instruction = memory[address[9:2]];

endmodule
