// ================================================================
//  alu_itype_tb.cpp
//  Verilator self-checking testbench for RV32I I-type ALU
//  instructions: XORI, ORI, ANDI, SLTI, SLTIU, SLLI, SRLI, SRAI
//
//  Program: software/asm/alu_itype.S  (16 instructions, 4 phases)
//
//  Instruction sequence:
//    0x00  addi  x1, x0, 10      x1 = 10
//    0x04  xori  x2, x1, 12      x2 = 6
//    0x08  ori   x3, x1, 12      x3 = 14
//    0x0C  andi  x4, x1, 12      x4 = 8           <- check phase 1
//    0x10  slti  x1, x0,  1      x1 = 1
//    0x14  sltiu x2, x0,  1      x2 = 1
//    0x18  slti  x3, x0, -1      x3 = 0
//    0x1C  sltiu x4, x0, -1      x4 = 1           <- check phase 2
//    0x20  addi  x1, x0, -1      x1 = 0xFFFFFFFF
//    0x24  slli  x2, x1,  1      x2 = 0xFFFFFFFE
//    0x28  srli  x3, x1,  1      x3 = 0x7FFFFFFF
//    0x2C  srai  x4, x1,  1      x4 = 0xFFFFFFFF  <- check phase 3
//    0x30  addi  x1, x0, -1      x1 = 0xFFFFFFFF
//    0x34  srli  x2, x1, 31      x2 = 0x00000001
//    0x38  srai  x3, x1, 31      x3 = 0xFFFFFFFF
//    0x3C  slli  x4, x1, 31      x4 = 0x80000000  <- check phase 4
//    0x40  j loop                (infinite)

#include <iostream>
#include <iomanip>
#include <string>

#include "Vriscv_cpu.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

//     ANSI color helpers                                     
#define GREEN "\033[32m"
#define RED   "\033[31m"
#define CYAN  "\033[36m"
#define RESET "\033[0m"

static int passed = 0;
static int failed = 0;

//     Self-checking primitive                                
void check(uint32_t expected,
           uint32_t actual,
           const std::string &msg)
{
    if (expected == actual)
    {
        std::cout << GREEN << "[PASS] " << RESET
                  << msg << std::endl;
        passed++;
    }
    else
    {
        std::cout << RED << "[FAIL] " << RESET
                  << msg << std::endl;

        std::cout << "  Expected : 0x"
                  << std::hex << std::setw(8) << std::setfill('0')
                  << expected << std::dec
                  << "  (" << static_cast<int32_t>(expected) << " signed)\n";

        std::cout << "  Actual   : 0x"
                  << std::hex << std::setw(8) << std::setfill('0')
                  << actual << std::dec
                  << "  (" << static_cast<int32_t>(actual) << " signed)\n";

        failed++;
    }
}

//     Clock helper                                           
void tick(Vriscv_cpu    &dut,
          VerilatedVcdC *trace,
          vluint64_t    &t)
{
    dut.clk = 0; dut.eval(); trace->dump(t); t += 5;
    dut.clk = 1; dut.eval(); trace->dump(t); t += 5;
    trace->flush();
}

//     Register bundle check                                  
void check_regs(Vriscv_cpu &dut,
                uint32_t ex1, uint32_t ex2,
                uint32_t ex3, uint32_t ex4)
{
    check(ex1, dut.debug_x1, "x1");
    check(ex2, dut.debug_x2, "x2");
    check(ex3, dut.debug_x3, "x3");
    check(ex4, dut.debug_x4, "x4");
}

//     Main                                                    
int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    Vriscv_cpu dut;

    VerilatedVcdC *trace = new VerilatedVcdC;
    dut.trace(trace, 99);
    trace->open("waveforms/alu_itype.vcd");

    vluint64_t sim_time = 0;

    //                                                                                        
    // RESET
    //                                                        
    std::cout << CYAN << "\n[RESET]\n" << RESET;
    dut.reset = 1;
    dut.eval();
    trace->dump(sim_time);

    check(0x00000000, dut.debug_pc, "PC after reset");
    check_regs(dut, 0, 0, 0, 0);

    dut.reset = 0;

    // ============================================================
    // Phase 1 — XORI, ORI, ANDI
    // ============================================================
    std::cout << CYAN << "\n====== Phase 1: XORI, ORI, ANDI ======\n" << RESET;

    // Tick 1 : addi x1, x0, 10  →  x1 = 10
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick  1] addi x1, x0, 10\n" << RESET;
    check(0x00000004, dut.debug_pc, "PC = 0x04");
    check(10, dut.debug_x1, "x1 = 10");

    // Tick 2 : xori x2, x1, 12  →  x2 = 10 ^ 12 = 6
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick  2] xori x2, x1, 12  (10 ^ 12 = 6)\n" << RESET;
    check(0x00000008, dut.debug_pc, "PC = 0x08");
    check(6, dut.debug_x2, "x2 = 6   (XORI)");

    // Tick 3 : ori x3, x1, 12  →  x3 = 10 | 12 = 14
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick  3] ori  x3, x1, 12  (10 | 12 = 14)\n" << RESET;
    check(0x0000000C, dut.debug_pc, "PC = 0x0C");
    check(14, dut.debug_x3, "x3 = 14  (ORI)");

    // Tick 4 : andi x4, x1, 12  →  x4 = 10 & 12 = 8
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick  4] andi x4, x1, 12  (10 & 12 = 8)\n" << RESET;
    check(0x00000010, dut.debug_pc, "PC = 0x10");
    check(8, dut.debug_x4, "x4 = 8   (ANDI)");

    std::cout << CYAN << "--- Phase 1 register state (x1=10, x2=6, x3=14, x4=8) ---\n" << RESET;
    check_regs(dut, 10, 6, 14, 8);

    // ============================================================
    // Phase 2 — SLTI, SLTIU (with negative immediate edge cases)
    // ============================================================
    std::cout << CYAN << "\n====== Phase 2: SLTI, SLTIU (edge cases) ======\n" << RESET;

    // Tick 5 : slti x1, x0, 1  →  x1 = (0 <s 1) = 1
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick  5] slti  x1, x0,  1  (0 <s 1 = 1)\n" << RESET;
    check(0x00000014, dut.debug_pc, "PC = 0x14");
    check(1, dut.debug_x1, "x1 = 1   (SLTI normal)");

    // Tick 6 : sltiu x2, x0, 1  →  x2 = (0 <u 1) = 1
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick  6] sltiu x2, x0,  1  (0 <u 1 = 1)\n" << RESET;
    check(0x00000018, dut.debug_pc, "PC = 0x18");
    check(1, dut.debug_x2, "x2 = 1   (SLTIU normal)");

    // Tick 7 : slti x3, x0, -1  →  x3 = (0 <s -1) = 0
    //   -1 sign-extended = 0xFFFFFFFF = -1 in signed  →  0 > -1  →  false
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick  7] slti  x3, x0, -1  (0 <s -1 = 0; 0 is above -1)\n" << RESET;
    check(0x0000001C, dut.debug_pc, "PC = 0x1C");
    check(0, dut.debug_x3, "x3 = 0   (SLTI negative imm, 0 > -1 signed)");

    // Tick 8 : sltiu x4, x0, -1  →  x4 = (0 <u 0xFFFFFFFF) = 1
    //   -1 sign-extended = 0xFFFFFFFF = 4294967295 unsigned  →  0 < 4294967295  →  true
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick  8] sltiu x4, x0, -1  (0 <u 0xFFFFFFFF = 1; max unsigned)\n" << RESET;
    check(0x00000020, dut.debug_pc, "PC = 0x20");
    check(1, dut.debug_x4, "x4 = 1   (SLTIU negative imm as unsigned max)");

    std::cout << CYAN << "--- Phase 2 register state (x1=1, x2=1, x3=0, x4=1) ---\n" << RESET;
    check_regs(dut, 1, 1, 0, 1);

    // ============================================================
    // Phase 3 — SLLI, SRLI, SRAI (sign-bit behavior, shamt=1)
    //   Operand: 0xFFFFFFFF (-1)
    //   SLLI: 0xFFFFFFFE  (bit 0 falls off, 0 enters at LSB)
    //   SRLI: 0x7FFFFFFF  (MSB becomes 0 — logical)
    //   SRAI: 0xFFFFFFFF  (MSB stays 1 — arithmetic)
    // ============================================================
    std::cout << CYAN << "\n====== Phase 3: SLLI, SRLI, SRAI (shamt=1, operand=0xFFFFFFFF) ======\n" << RESET;

    // Tick 9 : addi x1, x0, -1  →  x1 = 0xFFFFFFFF
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick  9] addi x1, x0, -1\n" << RESET;
    check(0x00000024, dut.debug_pc, "PC = 0x24");
    check(0xFFFFFFFF, dut.debug_x1, "x1 = 0xFFFFFFFF");

    // Tick 10 : slli x2, x1, 1  →  x2 = 0xFFFFFFFE
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick 10] slli x2, x1, 1  (0xFFFFFFFF << 1 = 0xFFFFFFFE)\n" << RESET;
    check(0x00000028, dut.debug_pc, "PC = 0x28");
    check(0xFFFFFFFE, dut.debug_x2, "x2 = 0xFFFFFFFE (SLLI)");

    // Tick 11 : srli x3, x1, 1  →  x3 = 0x7FFFFFFF  (logical: MSB→0)
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick 11] srli x3, x1, 1  (0xFFFFFFFF >> 1 = 0x7FFFFFFF, MSB=0)\n" << RESET;
    check(0x0000002C, dut.debug_pc, "PC = 0x2C");
    check(0x7FFFFFFF, dut.debug_x3, "x3 = 0x7FFFFFFF (SRLI, logical zero-fill)");

    // Tick 12 : srai x4, x1, 1  →  x4 = 0xFFFFFFFF  (arithmetic: MSB stays 1)
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick 12] srai x4, x1, 1  (sra(0xFFFFFFFF,1) = 0xFFFFFFFF, MSB=1)\n" << RESET;
    check(0x00000030, dut.debug_pc, "PC = 0x30");
    check(0xFFFFFFFF, dut.debug_x4, "x4 = 0xFFFFFFFF (SRAI, arithmetic sign-fill)");

    std::cout << CYAN << "--- Phase 3 register state ---\n" << RESET;
    check_regs(dut, 0xFFFFFFFF, 0xFFFFFFFE, 0x7FFFFFFF, 0xFFFFFFFF);

    // ============================================================
    // Phase 4 — Max shift amount (31): SRLI vs SRAI distinction
    // ============================================================
    std::cout << CYAN << "\n====== Phase 4: Max shift (shamt=31), SRLI vs SRAI ======\n" << RESET;

    // Tick 13 : addi x1, x0, -1  →  x1 = 0xFFFFFFFF
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick 13] addi x1, x0, -1  (reload 0xFFFFFFFF)\n" << RESET;
    check(0x00000034, dut.debug_pc, "PC = 0x34");
    check(0xFFFFFFFF, dut.debug_x1, "x1 = 0xFFFFFFFF");

    // Tick 14 : srli x2, x1, 31  →  x2 = 0x00000001  (only bit 31 survives)
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick 14] srli x2, x1, 31  (0xFFFFFFFF >> 31 = 0x00000001)\n" << RESET;
    check(0x00000038, dut.debug_pc, "PC = 0x38");
    check(0x00000001, dut.debug_x2, "x2 = 0x00000001 (SRLI max shift, logical)");

    // Tick 15 : srai x3, x1, 31  →  x3 = 0xFFFFFFFF  (all bits become sign bit)
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick 15] srai x3, x1, 31  (sra(0xFFFFFFFF,31) = 0xFFFFFFFF)\n" << RESET;
    check(0x0000003C, dut.debug_pc, "PC = 0x3C");
    check(0xFFFFFFFF, dut.debug_x3, "x3 = 0xFFFFFFFF (SRAI max shift, arithmetic)");

    // Tick 16 : slli x4, x1, 31  →  x4 = 0x80000000  (bit 0 of src shifts to bit 31)
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick 16] slli x4, x1, 31  (0xFFFFFFFF << 31 = 0x80000000)\n" << RESET;
    check(0x00000040, dut.debug_pc, "PC = 0x40");
    check(0x80000000, dut.debug_x4, "x4 = 0x80000000 (SLLI max shift)");

    std::cout << CYAN << "--- Phase 4 register state ---\n" << RESET;
    check_regs(dut, 0xFFFFFFFF, 0x00000001, 0xFFFFFFFF, 0x80000000);

    //                                                        
    // Summary
    //                                                        
    trace->close();
    delete trace;

    std::cout << "\n========================================\n";
    std::cout << "  ALU I-type Regression Results\n";
    std::cout << "========================================\n";
    std::cout << GREEN << "  Passed : " << passed << RESET << "\n";
    if (failed > 0)
        std::cout << RED << "  Failed : " << failed << RESET << "\n";
    else
        std::cout << "  Failed : " << failed << "\n";
    std::cout << "========================================\n\n";

    // Final ALU state dump (for waveform cross-reference)
    std::cout << std::hex << std::setfill('0');
    std::cout << "Final ALU state:\n";
    std::cout << "  PC          : 0x" << std::setw(8) << dut.debug_pc           << "\n";
    std::cout << "  Instruction : 0x" << std::setw(8) << dut.debug_instruction   << "\n";
    std::cout << "  ALU op A    : 0x" << std::setw(8) << dut.debug_alu_operand_a << "\n";
    std::cout << "  ALU op B    : 0x" << std::setw(8) << dut.debug_alu_operand_b << "\n";
    std::cout << "  ALU result  : 0x" << std::setw(8) << dut.debug_alu_result    << "\n";
    std::cout << "  Writeback   : 0x" << std::setw(8) << dut.debug_writeback     << "\n";
    std::cout << std::dec;

    return failed;  // 0 = all pass
}
