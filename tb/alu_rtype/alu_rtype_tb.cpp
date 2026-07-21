// ============================================================
//  alu_rtype_tb.cpp
//  Verilator self-checking testbench for RV32I R-type ALU
//  instructions: XOR, OR, AND, SLT, SLTU, SLL, SRL, SRA
//
//  Program: software/asm/alu_rtype.S
//  Assembly sequence (PC 0x00 … 0x30):
//    0x00  addi  x1, x0, 10       x1 = 10
//    0x04  addi  x2, x0, 12       x2 = 12
//    0x08  xor   x3, x1, x2       x3 = 6
//    0x0C  or    x4, x1, x2       x4 = 14   <- check phase 1
//    0x10  and   x1, x1, x2       x1 = 8
//    0x14  slt   x2, x1, x2       x2 = 1
//    0x18  sltu  x3, x2, x1       x3 = 1
//    0x1C  sll   x4, x1, x2       x4 = 16   <- check phase 2
//    0x20  srl   x1, x4, x2       x1 = 8
//    0x24  addi  x2, x0, -1       x2 = 0xFFFFFFFF
//    0x28  sra   x3, x2, x1       x3 = 0xFFFFFFFF
//    0x2C  addi  x4, x0, 1        x4 = 1    <- check phase 3
//    0x30  j loop                 (infinite)
// ============================================================

#include <iostream>
#include <iomanip>
#include <string>

#include "Vriscv_cpu.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

// ─── ANSI color helpers ────────────────────────────────────
#define GREEN "\033[32m"
#define RED   "\033[31m"
#define CYAN  "\033[36m"
#define RESET "\033[0m"

static int passed = 0;
static int failed = 0;

// ─── Self-checking primitive ───────────────────────────────
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
                  << expected << std::dec << " (" << expected << ")\n";

        std::cout << "  Actual   : 0x"
                  << std::hex << std::setw(8) << std::setfill('0')
                  << actual   << std::dec << " (" << actual   << ")\n";

        failed++;
    }
}

// ─── Clock helper ──────────────────────────────────────────
void tick(Vriscv_cpu     &dut,
          VerilatedVcdC  *trace,
          vluint64_t     &t)
{
    dut.clk = 0; dut.eval(); trace->dump(t); t += 5;
    dut.clk = 1; dut.eval(); trace->dump(t); t += 5;
    trace->flush();
}

// ─── Register bundle check ─────────────────────────────────
void check_regs(Vriscv_cpu &dut,
                uint32_t ex1, uint32_t ex2,
                uint32_t ex3, uint32_t ex4)
{
    check(ex1, dut.debug_x1, "x1");
    check(ex2, dut.debug_x2, "x2");
    check(ex3, dut.debug_x3, "x3");
    check(ex4, dut.debug_x4, "x4");
}

// ─── Main ──────────────────────────────────────────────────
int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    Vriscv_cpu dut;

    VerilatedVcdC *trace = new VerilatedVcdC;
    dut.trace(trace, 99);
    trace->open("waveforms/alu_rtype.vcd");

    vluint64_t sim_time = 0;

     
    // RESET
     
    std::cout << CYAN << "\n[RESET]\n" << RESET;
    dut.reset = 1;
    dut.eval();
    trace->dump(sim_time);

    check(0x00000000, dut.debug_pc, "PC after reset");
    check_regs(dut, 0, 0, 0, 0);

    dut.reset = 0;

     
    // Tick 1 : addi x1, x0, 10  →  x1 = 10
     
    tick(dut, trace, sim_time);
    std::cout << CYAN << "\n[Tick 1] addi x1, x0, 10\n" << RESET;
    check(0x00000004, dut.debug_pc, "PC = 4");
    check(10, dut.debug_x1, "x1 = 10");

     
    // Tick 2 : addi x2, x0, 12  →  x2 = 12
     
    tick(dut, trace, sim_time);
    std::cout << CYAN << "\n[Tick 2] addi x2, x0, 12\n" << RESET;
    check(0x00000008, dut.debug_pc, "PC = 8");
    check(12, dut.debug_x2, "x2 = 12");

     
    // Tick 3 : xor x3, x1, x2  →  x3 = 10 ^ 12 = 6
     
    tick(dut, trace, sim_time);
    std::cout << CYAN << "\n[Tick 3] xor x3, x1, x2   (10 ^ 12 = 6)\n" << RESET;
    check(0x0000000C, dut.debug_pc, "PC = 0xC");
    check(6, dut.debug_x3, "x3 = 6  (XOR)");

     
    // Tick 4 : or x4, x1, x2  →  x4 = 10 | 12 = 14
     
    tick(dut, trace, sim_time);
    std::cout << CYAN << "\n[Tick 4] or x4, x1, x2    (10 | 12 = 14)\n" << RESET;
    check(0x00000010, dut.debug_pc, "PC = 0x10");
    check(14, dut.debug_x4, "x4 = 14 (OR)");

    std::cout << CYAN << "\n--- Phase 1 register state ---\n" << RESET;
    check_regs(dut, 10, 12, 6, 14);

     
    // Tick 5 : and x1, x1, x2  →  x1 = 10 & 12 = 8
     
    tick(dut, trace, sim_time);
    std::cout << CYAN << "\n[Tick 5] and x1, x1, x2   (10 & 12 = 8)\n" << RESET;
    check(0x00000014, dut.debug_pc, "PC = 0x14");
    check(8, dut.debug_x1, "x1 = 8  (AND)");

     
    // Tick 6 : slt x2, x1, x2  →  x2 = (8 < 12) signed = 1
     
    tick(dut, trace, sim_time);
    std::cout << CYAN << "\n[Tick 6] slt x2, x1, x2   (8 < 12 signed = 1)\n" << RESET;
    check(0x00000018, dut.debug_pc, "PC = 0x18");
    check(1, dut.debug_x2, "x2 = 1  (SLT)");

     
    // Tick 7 : sltu x3, x2, x1  →  x3 = (1 <u 8) unsigned = 1
     
    tick(dut, trace, sim_time);
    std::cout << CYAN << "\n[Tick 7] sltu x3, x2, x1  (1 <u 8 = 1)\n" << RESET;
    check(0x0000001C, dut.debug_pc, "PC = 0x1C");
    check(1, dut.debug_x3, "x3 = 1  (SLTU)");

     
    // Tick 8 : sll x4, x1, x2  →  x4 = 8 << (1 & 0x1F) = 16
     
    tick(dut, trace, sim_time);
    std::cout << CYAN << "\n[Tick 8] sll x4, x1, x2   (8 << 1 = 16)\n" << RESET;
    check(0x00000020, dut.debug_pc, "PC = 0x20");
    check(16, dut.debug_x4, "x4 = 16 (SLL)");

    std::cout << CYAN << "\n--- Phase 2 register state ---\n" << RESET;
    check_regs(dut, 8, 1, 1, 16);

     
    // Tick 9 : srl x1, x4, x2  →  x1 = srl(16, 1) = 8
     
    tick(dut, trace, sim_time);
    std::cout << CYAN << "\n[Tick 9] srl x1, x4, x2   (16 >> 1 = 8)\n" << RESET;
    check(0x00000024, dut.debug_pc, "PC = 0x24");
    check(8, dut.debug_x1, "x1 = 8  (SRL)");

     
    // Tick 10 : addi x2, x0, -1  →  x2 = 0xFFFFFFFF
     
    tick(dut, trace, sim_time);
    std::cout << CYAN << "\n[Tick 10] addi x2, x0, -1  (x2 = 0xFFFFFFFF)\n" << RESET;
    check(0x00000028, dut.debug_pc, "PC = 0x28");
    check(0xFFFFFFFF, dut.debug_x2, "x2 = 0xFFFFFFFF");

     
    // Tick 11 : sra x3, x2, x1  →  x3 = sra(0xFFFFFFFF, 8) = 0xFFFFFFFF
    //           Arithmetic right-shift of -1 by any amount stays -1.
    //           Verifies sign-bit replication.
     
    tick(dut, trace, sim_time);
    std::cout << CYAN << "\n[Tick 11] sra x3, x2, x1  (sra(0xFFFFFFFF,8) = 0xFFFFFFFF)\n" << RESET;
    check(0x0000002C, dut.debug_pc, "PC = 0x2C");
    check(0xFFFFFFFF, dut.debug_x3, "x3 = 0xFFFFFFFF (SRA sign preserved)");

     
    // Tick 12 : addi x4, x0, 1  →  x4 = 1 (sentinel)
     
    tick(dut, trace, sim_time);
    std::cout << CYAN << "\n[Tick 12] addi x4, x0, 1  (sentinel)\n" << RESET;
    check(0x00000030, dut.debug_pc, "PC = 0x30");

    std::cout << CYAN << "\n--- Phase 3 register state ---\n" << RESET;
    check_regs(dut, 8, 0xFFFFFFFF, 0xFFFFFFFF, 1);

     
    // Summary
     
    trace->close();
    delete trace;

    std::cout << "\n========================================\n";
    std::cout << "  ALU R-type Regression Results\n";
    std::cout << "========================================\n";
    std::cout << GREEN << "  Passed : " << passed << RESET << "\n";
    if (failed > 0)
        std::cout << RED   << "  Failed : " << failed << RESET << "\n";
    else
        std::cout << "  Failed : " << failed << "\n";
    std::cout << "========================================\n\n";

    // Dump final ALU signals for waveform cross-reference
    std::cout << std::hex << std::setfill('0');
    std::cout << "Final ALU state:\n";
    std::cout << "  PC          : 0x" << std::setw(8) << dut.debug_pc          << "\n";
    std::cout << "  Instruction : 0x" << std::setw(8) << dut.debug_instruction  << "\n";
    std::cout << "  ALU op A    : 0x" << std::setw(8) << dut.debug_alu_operand_a << "\n";
    std::cout << "  ALU op B    : 0x" << std::setw(8) << dut.debug_alu_operand_b << "\n";
    std::cout << "  ALU result  : 0x" << std::setw(8) << dut.debug_alu_result    << "\n";
    std::cout << "  Writeback   : 0x" << std::setw(8) << dut.debug_writeback     << "\n";
    std::cout << std::dec;

    return failed;  // 0 = success, non-zero = fail count
}
