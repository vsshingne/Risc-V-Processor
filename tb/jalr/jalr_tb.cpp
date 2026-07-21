// ================================================================
//  jalr_tb.cpp
//  Verilator self-checking testbench for JALR instruction
//
//  Program: software/asm/jalr.S
// ================================================================

#include <iostream>
#include <iomanip>
#include <string>

#include "Vriscv_cpu.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#define GREEN "\033[32m"
#define RED   "\033[31m"
#define CYAN  "\033[36m"
#define RESET "\033[0m"

static int passed = 0;
static int failed = 0;

void check(uint32_t expected,
           uint32_t actual,
           const std::string &msg)
{
    if (expected == actual)
    {
        std::cout << GREEN << "[PASS] " << RESET << msg << std::endl;
        passed++;
    }
    else
    {
        std::cout << RED << "[FAIL] " << RESET << msg << std::endl;
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

void tick(Vriscv_cpu    &dut,
          VerilatedVcdC *trace,
          vluint64_t    &t)
{
    dut.clk = 0; dut.eval(); trace->dump(t); t += 5;
    dut.clk = 1; dut.eval(); trace->dump(t); t += 5;
    trace->flush();
}

int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    Vriscv_cpu dut;

    VerilatedVcdC *trace = new VerilatedVcdC;
    dut.trace(trace, 99);
    trace->open("waveforms/jalr.vcd");

    vluint64_t sim_time = 0;

    // RESET
    std::cout << CYAN << "\n[RESET]\n" << RESET;
    dut.reset = 1;
    dut.eval();
    trace->dump(sim_time);
    check(0x00000000, dut.debug_pc, "PC after reset");
    dut.reset = 0;

    // ============================================================
    // Phase 1: Basic JALR (offset 0), rd receives PC+4 (0x08)
    // ============================================================
    std::cout << CYAN << "\n====== Phase 1: Basic JALR ======\n" << RESET;

    // Tick 1: addi x1, x0, 20 (x1 = 0x14)
    tick(dut, trace, sim_time);
    check(0x00000004, dut.debug_pc, "PC = 0x04");
    check(20, dut.debug_x1, "x1 = 20 (target address)");

    // Tick 2: jalr x2, x1, 0 (jump to 0x14, x2 = 0x08)
    tick(dut, trace, sim_time);
    check(0x00000014, dut.debug_pc, "PC = 0x14 (jumped to target1)");
    check(0x00000008, dut.debug_x2, "x2 = 0x08 (return address PC+4)");

    // Tick 3: addi x3, x0, 42
    tick(dut, trace, sim_time);
    check(0x00000018, dut.debug_pc, "PC = 0x18");
    check(42, dut.debug_x3, "x3 = 42");

    // ============================================================
    // Phase 2: Odd Target Address & Positive Immediate
    // ============================================================
    std::cout << CYAN << "\n====== Phase 2: Odd Target Address & Pos Imm ======\n" << RESET;

    // Tick 4: addi x1, x0, 39 (x1 = 0x27)
    tick(dut, trace, sim_time);
    check(0x0000001C, dut.debug_pc, "PC = 0x1C");
    check(0x00000027, dut.debug_x1, "x1 = 0x27 (odd target base)");

    // Tick 5: jalr x4, x1, 5 -> target = (0x27 + 5) & ~1 = 0x2C, x4 = 0x20
    tick(dut, trace, sim_time);
    check(0x0000002C, dut.debug_pc, "PC = 0x2C (bit 0 cleared target2)");
    check(0x00000020, dut.debug_x4, "x4 = 0x20 (return address PC+4)");

    // Tick 6: addi x3, x3, 1
    tick(dut, trace, sim_time);
    check(0x00000030, dut.debug_pc, "PC = 0x30");
    check(43, dut.debug_x3, "x3 = 43");

    // ============================================================
    // Phase 3: Negative Immediate JALR
    // ============================================================
    std::cout << CYAN << "\n====== Phase 3: Negative Immediate ======\n" << RESET;

    // Tick 7: addi x1, x0, 80 (x1 = 0x50)
    tick(dut, trace, sim_time);
    check(0x00000034, dut.debug_pc, "PC = 0x34");
    check(80, dut.debug_x1, "x1 = 80 (0x50)");

    // Tick 8: jalr x2, x1, -12 -> target = (0x50 - 12) & ~1 = 0x44, x2 = 0x38
    tick(dut, trace, sim_time);
    check(0x00000044, dut.debug_pc, "PC = 0x44 (jumped to target3)");
    check(0x00000038, dut.debug_x2, "x2 = 0x38 (return address PC+4)");

    // Tick 9: addi x3, x3, 1
    tick(dut, trace, sim_time);
    check(0x00000048, dut.debug_pc, "PC = 0x48");
    check(44, dut.debug_x3, "x3 = 44");

    // ============================================================
    // Phase 4: JALR with rd = x0
    // ============================================================
    std::cout << CYAN << "\n====== Phase 4: rd = x0 ======\n" << RESET;

    // Tick 10: addi x1, x0, 92 (x1 = 0x5C)
    tick(dut, trace, sim_time);
    check(0x0000004C, dut.debug_pc, "PC = 0x4C");
    check(92, dut.debug_x1, "x1 = 92 (0x5C)");

    // Tick 11: jalr x0, x1, 0 -> target = 0x5C, x0 stays 0
    tick(dut, trace, sim_time);
    check(0x0000005C, dut.debug_pc, "PC = 0x5C (jumped to target4)");
    check(0x00000000, dut.debug_x0, "x0 remains 0");

    // Tick 12: addi x4, x0, 99
    tick(dut, trace, sim_time);
    check(0x00000060, dut.debug_pc, "PC = 0x60");
    check(99, dut.debug_x4, "x4 = 99");

    // Summary
    trace->close();
    delete trace;

    std::cout << "\n========================================\n";
    std::cout << "  JALR Regression Results\n";
    std::cout << "========================================\n";
    std::cout << GREEN << "  Passed : " << passed << RESET << "\n";
    if (failed > 0)
        std::cout << RED << "  Failed : " << failed << RESET << "\n";
    else
        std::cout << "  Failed : " << failed << "\n";
    std::cout << "========================================\n\n";

    return failed;
}
