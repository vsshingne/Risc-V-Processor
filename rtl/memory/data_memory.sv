`timescale 1ns/1ps

module data_memory
(
    input  logic        clk,

    input  logic        mem_read,
    input  logic        mem_write,

    input  logic [31:0] address,

    input  logic [31:0] write_data,

    output logic [31:0] read_data
);

    // 256 x 32-bit memory (1 KB)

    logic [31:0] memory [0:255];

      
    // Read Logic (Combinational)
      
    integer i;

    initial begin
        for (i = 0; i < 256; i++)
            memory[i] = 32'd0;
    end


    always_comb
    begin
        read_data = 32'd0;

        if (mem_read)
            read_data = memory[address[9:2]];
    end

      
    // Write Logic (Sequential)
      

    always_ff @(posedge clk)
    begin
        if(mem_write)
            memory[address[9:2]] <= write_data;
    end

endmodule
