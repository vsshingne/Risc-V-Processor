`timescale 1ns/1ps

import riscv_pkg::*;

module data_memory
(
    input  logic        clk,

    input  logic        mem_read,
    input  logic        mem_write,

    input  mem_size_t   mem_size,
    input  logic        mem_unsigned,

    input  logic [31:0] address,
    input  logic [31:0] write_data,

    output logic [31:0] read_data
);


    // 1 KB Byte Addressable Memory


    logic [7:0] memory [0:1023];


    // Internal Signals


    logic [9:0] byte_addr;
    logic       address_aligned;

    logic [7:0] byte0;
    logic [7:0] byte1;
    logic [7:0] byte2;
    logic [7:0] byte3;

    assign byte_addr = address[9:0];


    // Initialize Memory


    integer i;

    initial
    begin
        for(i = 0; i < 1024; i = i + 1)
            memory[i] = 8'd0;
    end


    // Fetch Bytes


    always_comb
    begin
        byte0 = memory[byte_addr];
        byte1 = memory[byte_addr + 1];
        byte2 = memory[byte_addr + 2];
        byte3 = memory[byte_addr + 3];
    end


    // Alignment Check


    always_comb
    begin

        unique case(mem_size)

            MEM_BYTE:
                address_aligned = 1'b1;

            MEM_HALF:
                address_aligned = ~byte_addr[0];

            MEM_WORD:
                address_aligned = (byte_addr[1:0] == 2'b00);

            default:
                address_aligned = 1'b0;

        endcase

    end


    // Read Logic


    always_comb
    begin

        read_data = 32'd0;

        if(mem_read && address_aligned)
        begin

            unique case(mem_size)


                // BYTE (placeholder)


                MEM_BYTE:
                begin
                    read_data = 32'd0;
                end


                // HALFWORD (placeholder)


                MEM_HALF:
                begin
                    read_data = 32'd0;
                end


                // WORD


                MEM_WORD:
                begin
                    read_data =
                    {
                        byte3,
                        byte2,
                        byte1,
                        byte0
                    };
                end

                default:
                    read_data = 32'd0;

            endcase

        end

    end


    // Write Logic


    always_ff @(posedge clk)
    begin

        if(mem_write && address_aligned)
        begin

            unique case(mem_size)


                // BYTE (placeholder)


                MEM_BYTE:
                begin
                    // SB
                end


                // HALFWORD (placeholder)


                MEM_HALF:
                begin
                    // SH
                end


                // WORD


                MEM_WORD:
                begin
                    memory[byte_addr]     <= write_data[7:0];
                    memory[byte_addr + 1] <= write_data[15:8];
                    memory[byte_addr + 2] <= write_data[23:16];
                    memory[byte_addr + 3] <= write_data[31:24];
                end

            endcase

        end

    end

endmodule