`timescale 1ns/1ps

module immediate_generator
(
    input  logic [31:0] instruction,

    output logic [31:0] imm_i,
    output logic [31:0] imm_s,
    output logic [31:0] imm_b,
    output logic [31:0] imm_u,
    output logic [31:0] imm_j
);

always_comb
begin

      
    // I-Type
      

    imm_i = {
        {20{instruction[31]}},
        instruction[31:20]
    };

      
    // S-Type
      

    imm_s = {
        {20{instruction[31]}},
        instruction[31:25],
        instruction[11:7]
    };

      
    // B-Type
      

    imm_b = {
        {19{instruction[31]}},
        instruction[31],
        instruction[7],
        instruction[30:25],
        instruction[11:8],
        1'b0
    };

      
    // U-Type
      

    imm_u = {
        instruction[31:12],
        12'b0
    };

      
    // J-Type
      

    imm_j = {
        {11{instruction[31]}},
        instruction[31],
        instruction[19:12],
        instruction[20],
        instruction[30:21],
        1'b0
    };

end

endmodule