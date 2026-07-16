`timescale 1ns/1ps

module register_file
(
    input  logic        clk,
    input  logic        reset,
    input  logic        we,

    input  logic [4:0]  rs1_addr,
    input  logic [4:0]  rs2_addr,

    input  logic [4:0]  rd_addr,
    input  logic [31:0] rd_data,

    output logic [31:0] rs1_data,
    output logic [31:0] rs2_data
);

      
    // Register Array
      

    logic [31:0] registers [31:0];

      
    // Combinational Read Ports
      

    assign rs1_data =
        (rs1_addr == 5'd0)
            ? 32'd0
            : registers[rs1_addr];

    assign rs2_data =
        (rs2_addr == 5'd0)
            ? 32'd0
            : registers[rs2_addr];

      
    // Sequential Write Port
      

    integer i;

    always_ff @(posedge clk)
    begin

        if(reset)
        begin

            for(i = 0; i < 32; i = i + 1)
                registers[i] <= 32'd0;

        end

        else if(we && (rd_addr != 5'd0))
        begin

            registers[rd_addr] <= rd_data;

        end

    end

endmodule