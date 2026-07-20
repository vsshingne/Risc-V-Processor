`timescale 1ns/1ps

module branch_comparator
(
    input  logic [31:0] rs1,
    input  logic [31:0] rs2,

    output logic        equal,
    output logic        lt_signed,
    output logic        lt_unsigned
);

always_comb
begin
    equal       = (rs1 == rs2);

    lt_signed   = ($signed(rs1) < $signed(rs2));

    lt_unsigned = (rs1 < rs2);
end

endmodule