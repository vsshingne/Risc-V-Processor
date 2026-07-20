`timescale 1ns/1ps

module pc_mux
(
    input  logic [31:0] pc,
    input  logic [31:0] immediate,

    input  logic        branch,
    input  logic        branch_not_equal,
    input  logic        jump,
    input  logic        zero,

    output logic [31:0] pc_next,
    input logic branch_less_than,
    input logic branch_greater_equal,

    input logic branch_less_than_unsigned,
    input logic branch_greater_equal_unsigned,

    input logic lt_signed,
    input logic lt_unsigned
);

always_comb
begin
    if (jump)
    begin
        pc_next = pc + immediate;
    end
    else if (branch)
    begin

        // BEQ
        if (!branch_not_equal &&
            !branch_less_than &&
            zero)

            pc_next = pc + immediate;

        // BNE
        else if (branch_not_equal &&
                 !branch_less_than &&
                 !zero)

            pc_next = pc + immediate;

        // BLT
        else if (branch_less_than &&
                 lt_signed)

            pc_next = pc + immediate;

        // BGE
        else if(branch_greater_equal &&
                !lt_signed)

            pc_next = pc + immediate;

        // BLTU
        else if (branch_less_than_unsigned &&
                lt_unsigned)

            pc_next = pc + immediate;

        // BGEU
        else if (branch_greater_equal_unsigned &&
                !lt_unsigned)

            pc_next = pc + immediate;

        else

            pc_next = pc + 32'd4;
    end
    else
    begin
        pc_next = pc + 32'd4;
    end
end

endmodule