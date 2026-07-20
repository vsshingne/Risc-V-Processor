`timescale 1ns/1ps

module pc_mux
(
    input  logic [31:0] pc,
    input  logic [31:0] immediate,

    input  logic        branch,
    input  logic        branch_not_equal,
    input  logic        jump,
    input  logic        zero,

    output logic [31:0] pc_next
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
        if (!branch_not_equal && zero)
            pc_next = pc + immediate;

        // BNE
        else if (branch_not_equal && !zero)
            pc_next = pc + immediate;

        // Branch condition not satisfied
        else
            pc_next = pc + 32'd4;
    end
    else
    begin
        pc_next = pc + 32'd4;
    end
end

endmodule