`timescale 1ns/1ps

module riscv_cpu
(
    // Inputs
    input  logic clk,
    input  logic reset,

    // Debug Interface Ports
    output logic [31:0] debug_pc,
    output logic [31:0] debug_instruction,

    output logic [6:0]  debug_opcode,
    output logic [4:0]  debug_rd,
    output logic [4:0]  debug_rs1,
    output logic [4:0]  debug_rs2,
    output logic [2:0]  debug_funct3,
    output logic [6:0]  debug_funct7,

    output logic [31:0] debug_rs1_data,
    output logic [31:0] debug_rs2_data,

    output logic [31:0] debug_immediate,

    output logic [31:0] debug_alu_operand_a,
    output logic [31:0] debug_alu_operand_b,
    output logic [31:0] debug_alu_result,
    output logic        debug_zero,

    output logic [31:0] debug_memory_address,
    output logic [31:0] debug_memory_write_data,
    output logic [31:0] debug_memory_read_data,

    output logic [31:0] debug_writeback,
    output logic        debug_reg_write,

    output logic [31:0] debug_x0,
    output logic [31:0] debug_x1,
    output logic [31:0] debug_x2,
    output logic [31:0] debug_x3,
    output logic [31:0] debug_x4,

    output logic [31:0] debug_pc_next,
    output logic        debug_jump,

    output logic        debug_branch,
    output logic        debug_pc_write,
    output logic        debug_branch_not_equal,
    output logic        debug_branch_less_than,
    output logic        debug_branch_greater_equal,

    output logic        debug_branch_less_than_unsigned,
    output logic        debug_branch_greater_equal_unsigned,

    output logic        debug_equal,
    output logic        debug_lt_signed,
    output logic        debug_lt_unsigned
);

    import riscv_pkg::*;

    // Program Counter Signals
    logic [31:0] pc;
    logic [31:0] pc_next;
    logic        pc_write;

    assign pc_write = 1'b1;

    // Instruction Signal
    logic [31:0] instruction;

    // Decoder Output Signals
    logic [6:0] opcode;
    logic [4:0] rd;
    logic [4:0] rs1;
    logic [4:0] rs2;
    logic [2:0] funct3;
    logic [6:0] funct7;

    // Register File Output Signals
    logic [31:0] rs1_data;
    logic [31:0] rs2_data;

    // Immediate Generator Output Signals
    logic [31:0] imm_i;
    logic [31:0] imm_s;
    logic [31:0] imm_b;
    logic [31:0] imm_u;
    logic [31:0] imm_j;

    logic [31:0] imm_data;

    // Control Signals
    logic reg_write;
    logic mem_read;
    logic mem_write;

    logic branch;
    logic jump;
    logic jalr;

    logic alu_src;
    logic result_src;

    alu_op_t   alu_op;
    mem_size_t mem_size;
    logic      mem_unsigned;
    imm_sel_t  imm_sel;

    logic [1:0] alu_a_sel;   // 2'b00=rs1_data  2'b01=pc  2'b10=zero

    // Execute Stage Signals
    logic [31:0] alu_operand_a;
    logic [31:0] alu_operand_b;
    logic [31:0] alu_result;

    logic zero;

    // Operand A MUX (selects rs1_data, PC, or zero)
    always_comb
    begin
        case (alu_a_sel)
            2'b01:   alu_operand_a = pc;
            2'b10:   alu_operand_a = 32'd0;
            default: alu_operand_a = rs1_data;
        endcase
    end

    // Memory Stage Signals
    logic [31:0] memory_data;

    // Writeback Stage Signals
    logic [31:0] writeback_data;

    // Branch / Jump Signals
    logic branch_not_equal;

    logic equal;
    logic lt_signed;
    logic lt_unsigned;

    logic branch_less_than;
    logic branch_greater_equal;

    logic branch_less_than_unsigned;
    logic branch_greater_equal_unsigned;

    // Debug Interface Assignments
    assign debug_pc          = pc;
    assign debug_instruction = instruction;

    assign debug_opcode = opcode;
    assign debug_rd     = rd;
    assign debug_rs1    = rs1;
    assign debug_rs2    = rs2;
    assign debug_funct3 = funct3;
    assign debug_funct7 = funct7;

    assign debug_rs1_data = rs1_data;
    assign debug_rs2_data = rs2_data;

    assign debug_immediate = imm_data;

    assign debug_alu_operand_a = alu_operand_a;
    assign debug_alu_operand_b = alu_operand_b;
    assign debug_alu_result    = alu_result;
    assign debug_zero          = zero;

    assign debug_memory_address    = alu_result;
    assign debug_memory_write_data = rs2_data;
    assign debug_memory_read_data  = memory_data;

    assign debug_writeback = writeback_data;
    assign debug_reg_write = reg_write;

    assign debug_pc_next = pc_next;
    assign debug_jump    = jump;

    assign debug_branch   = branch;
    assign debug_pc_write = pc_write;

    assign debug_branch_not_equal = branch_not_equal;
    assign debug_branch_less_than = branch_less_than;
    assign debug_branch_greater_equal = branch_greater_equal;

    assign debug_branch_less_than_unsigned = branch_less_than_unsigned;
    assign debug_branch_greater_equal_unsigned = branch_greater_equal_unsigned;

    assign debug_equal       = equal;
    assign debug_lt_signed   = lt_signed;
    assign debug_lt_unsigned = lt_unsigned;

    // Program Counter Unit
    program_counter u_program_counter
    (
        .clk      (clk),
        .reset    (reset),
        .pc_write (1'b1),
        .pc_next  (pc_next),
        .pc       (pc)
    );

    // Instruction Memory Unit
    instruction_memory u_instruction_memory
    (
        .address     (pc),
        .instruction (instruction)
    );

    // Instruction Decoder Unit
    instruction_decoder u_instruction_decoder
    (
        .instruction (instruction),
        .opcode (opcode),
        .rd     (rd),
        .rs1    (rs1),
        .rs2    (rs2),
        .funct3 (funct3),
        .funct7 (funct7)
    );

    // Immediate Generator Unit
    immediate_generator u_immediate_generator
    (
        .instruction (instruction),
        .imm_i       (imm_i),
        .imm_s       (imm_s),
        .imm_b       (imm_b),
        .imm_u       (imm_u),
        .imm_j       (imm_j)
    );

    // Immediate Select MUX
    always_comb
    begin
        case (imm_sel)
            IMM_I:   imm_data = imm_i;
            IMM_S:   imm_data = imm_s;
            IMM_B:   imm_data = imm_b;
            IMM_U:   imm_data = imm_u;
            IMM_J:   imm_data = imm_j;
            default: imm_data = 32'd0;
        endcase
    end

    // Control Unit
    control_unit u_control_unit
    (
        .opcode     (opcode),
        .funct3     (funct3),
        .funct7     (funct7),

        .reg_write  (reg_write),
        .mem_read   (mem_read),
        .mem_write  (mem_write),

        .branch     (branch),
        .jump       (jump),

        .alu_src    (alu_src),
        .result_src (result_src),

        .alu_op     (alu_op),
        .mem_size   (mem_size),
        .mem_unsigned(mem_unsigned),
        .imm_sel    (imm_sel),
        .branch_not_equal(branch_not_equal),
        .branch_less_than(branch_less_than),
        .branch_greater_equal(branch_greater_equal),
        .branch_less_than_unsigned(branch_less_than_unsigned),
        .branch_greater_equal_unsigned(branch_greater_equal_unsigned),
        .alu_a_sel  (alu_a_sel),
        .jalr       (jalr)
    );

    // Register File Unit
    register_file u_register_file
    (
        .clk      (clk),
        .reset    (reset),
        .we       (reg_write),
        .rs1_addr (rs1),
        .rs2_addr (rs2),
        .rd_addr  (rd),
        .rd_data  (writeback_data),
        .rs1_data (rs1_data),
        .rs2_data (rs2_data),
        .debug_x0(debug_x0),
        .debug_x1(debug_x1),
        .debug_x2(debug_x2),
        .debug_x3(debug_x3),
        .debug_x4(debug_x4)
    );

    // ALU Operand B MUX
    alu_mux u_alu_mux
    (
        .rs2_data      (rs2_data),
        .imm_data      (imm_data),
        .alu_src       (alu_src),
        .alu_operand_b (alu_operand_b)
    );

    // ALU Unit
    alu u_alu
    (
        .a      (alu_operand_a),
        .b      (alu_operand_b),
        .alu_op (alu_op),
        .result (alu_result),
        .zero   (zero)
    );

    // Data Memory Unit
    data_memory u_data_memory
    (
        .clk          (clk),
        .mem_read     (mem_read),
        .mem_write    (mem_write),
        .mem_size     (mem_size),
        .mem_unsigned (mem_unsigned),
        .address      (alu_result),
        .write_data   (rs2_data),
        .read_data    (memory_data)
    );

    // Writeback MUX Unit
    wb_mux u_wb_mux
    (
        .alu_result     (alu_result),
        .memory_data    (memory_data),
        .pc             (pc),
        .result_src     (result_src),
        .jump           (jump),
        .writeback_data (writeback_data)
    );

    // PC Selection MUX Unit
    pc_mux u_pc_mux
    (
        .pc                            (pc),
        .immediate                     (imm_data),
        .alu_result                    (alu_result),
        .branch                        (branch),
        .branch_not_equal              (branch_not_equal),
        .jump                          (jump),
        .jalr                          (jalr),
        .zero                          (zero),
        .pc_next                       (pc_next),
        .branch_less_than              (branch_less_than),
        .branch_greater_equal          (branch_greater_equal),
        .branch_less_than_unsigned     (branch_less_than_unsigned),
        .branch_greater_equal_unsigned (branch_greater_equal_unsigned),
        .lt_signed                     (lt_signed),
        .lt_unsigned                   (lt_unsigned)
    );

    // Branch Comparator Unit
    branch_comparator u_branch_comparator
    (
        .rs1        (rs1_data),
        .rs2        (rs2_data),
        .equal      (equal),
        .lt_signed  (lt_signed),
        .lt_unsigned(lt_unsigned)
    );

endmodule

