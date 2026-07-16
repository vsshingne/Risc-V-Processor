`timescale 1ns/1ps

module instruction_memory
(
    input logic [31:0] address,

    output logic [31:0] instruction
);

     
    // 256 x 32-bit ROM
     

    logic [31:0] memory [0:255];

     
    // Load Program
     

    initial
    begin
        $readmemh("software/program.hex",memory);
    end

     
    // Word Addressing
     

    assign instruction = memory[address[9:2]];

endmodule