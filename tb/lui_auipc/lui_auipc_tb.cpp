// ================================================================
//  lui_auipc_tb.cpp
//  Verilator self-checking testbench for LUI and AUIPC
//
//  Program: software/asm/lui_auipc.S
//
//  Instruction sequence:
//    0x00  lui    x1, 1         x1 = 0x00001000
//    0x04  lui    x2, 0xFFFFF   x2 = 0xFFFFF000
//    0x08  auipc  x3, 1         x3 = 0x08 + 0x1000 = 0x00001008
//    0x0C  auipc  x4, 0         x4 = 0x0C          = 0x0000000C
//    0x10  lui    x1, 0         x1 = 0x00000000
//    0x14  auipc  x2, 0         x2 = 0x14           = 0x00000014
//    0x18  lui    x3, 0x80000   x3 = 0x80000000
//    0x1C  auipc  x4, 0xFFFFF   x4 = 0x1C + 0xFFFFF000 = 0xFFFFF01C
//    0x20  j loop               (infinite)
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

void check_regs(Vriscv_cpu &dut,
                uint32_t ex1, uint32_t ex2,
                uint32_t ex3, uint32_t ex4)
{
    check(ex1, dut.debug_x1, "x1");
    check(ex2, dut.debug_x2, "x2");
    check(ex3, dut.debug_x3, "x3");
    check(ex4, dut.debug_x4, "x4");
}

int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    Vriscv_cpu dut;

    VerilatedVcdC *trace = new VerilatedVcdC;
    dut.trace(trace, 99);
    trace->open("waveforms/lui_auipc.vcd");

    vluint64_t sim_time = 0;

    // RESET
    std::cout << CYAN << "\n[RESET]\n" << RESET;
    dut.reset = 1;
    dut.eval();
    trace->dump(sim_time);
    check(0x00000000, dut.debug_pc, "PC after reset");
    check_regs(dut, 0, 0, 0, 0);
    dut.reset = 0;

    // ============================================================
    // Phase 1: LUI (positive), LUI (max-field), AUIPC, AUIPC (zero)
    // ============================================================
    std::cout << CYAN << "\n====== Phase 1: LUI / AUIPC basic cases ======\n" << RESET;

    // Tick 1: lui x1, 1  →  x1 = 0x00001000
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick 1] lui x1, 1\n" << RESET;
    check(0x00000004, dut.debug_pc, "PC = 0x04");
    check(0x00001000, dut.debug_x1, "x1 = 0x00001000 (LUI imm=1)");

    // Tick 2: lui x2, 0xFFFFF  →  x2 = 0xFFFFF000  (-4096 signed)
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick 2] lui x2, 0xFFFFF\n" << RESET;
    check(0x00000008, dut.debug_pc, "PC = 0x08");
    check(0xFFFFF000, dut.debug_x2, "x2 = 0xFFFFF000 (LUI max-field)");

    // Tick 3: auipc x3, 1  →  x3 = 0x08 + 0x1000 = 0x00001008
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick 3] auipc x3, 1  (PC=0x08 + 0x1000 = 0x1008)\n" << RESET;
    check(0x0000000C, dut.debug_pc, "PC = 0x0C");
    check(0x00001008, dut.debug_x3, "x3 = 0x00001008 (AUIPC imm=1 @ PC=0x08)");

    // Tick 4: auipc x4, 0  →  x4 = 0x0C + 0 = 0x0000000C
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick 4] auipc x4, 0  (PC=0x0C + 0 = 0x0C)\n" << RESET;
    check(0x00000010, dut.debug_pc, "PC = 0x10");
    check(0x0000000C, dut.debug_x4, "x4 = 0x0000000C (AUIPC zero imm = current PC)");

    std::cout << CYAN << "--- Phase 1 register state ---\n" << RESET;
    check_regs(dut, 0x00001000, 0xFFFFF000, 0x00001008, 0x0000000C);

    // ============================================================
    // Phase 2: LUI zero-imm, AUIPC zero-imm, LUI MSB, AUIPC neg-imm
    // ============================================================
    std::cout << CYAN << "\n====== Phase 2: edge cases ======\n" << RESET;

    // Tick 5: lui x1, 0  →  x1 = 0
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick 5] lui x1, 0\n" << RESET;
    check(0x00000014, dut.debug_pc, "PC = 0x14");
    check(0x00000000, dut.debug_x1, "x1 = 0 (LUI zero immediate)");

    // Tick 6: auipc x2, 0  →  x2 = 0x14 (= current PC)
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick 6] auipc x2, 0  (PC=0x14)\n" << RESET;
    check(0x00000018, dut.debug_pc, "PC = 0x18");
    check(0x00000014, dut.debug_x2, "x2 = 0x14 (AUIPC zero imm = PC of this instr)");

    // Tick 7: lui x3, 0x80000  →  x3 = 0x80000000  (MSB set)
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick 7] lui x3, 0x80000\n" << RESET;
    check(0x0000001C, dut.debug_pc, "PC = 0x1C");
    check(0x80000000, dut.debug_x3, "x3 = 0x80000000 (LUI MSB set)");

    // Tick 8: auipc x4, 0xFFFFF  →  x4 = 0x1C + 0xFFFFF000 = 0xFFFFF01C
    tick(dut, trace, sim_time);
    std::cout << CYAN << "[Tick 8] auipc x4, 0xFFFFF  (0x1C + 0xFFFFF000 = 0xFFFFF01C)\n" << RESET;
    check(0x00000020, dut.debug_pc, "PC = 0x20");
    check(0xFFFFF01C, dut.debug_x4, "x4 = 0xFFFFF01C (AUIPC + large upper imm)");

    std::cout << CYAN << "--- Phase 2 register state ---\n" << RESET;
    check_regs(dut, 0x00000000, 0x00000014, 0x80000000, 0xFFFFF01C);

    // Summary
    trace->close();
    delete trace;

    std::cout << "\n========================================\n";
    std::cout << "  LUI / AUIPC Regression Results\n";
    std::cout << "========================================\n";
    std::cout << GREEN << "  Passed : " << passed << RESET << "\n";
    if (failed > 0)
        std::cout << RED << "  Failed : " << failed << RESET << "\n";
    else
        std::cout << "  Failed : " << failed << "\n";
    std::cout << "========================================\n\n";

    std::cout << std::hex << std::setfill('0');
    std::cout << "Final state:\n";
    std::cout << "  PC          : 0x" << std::setw(8) << dut.debug_pc           << "\n";
    std::cout << "  Instruction : 0x" << std::setw(8) << dut.debug_instruction   << "\n";
    std::cout << "  ALU op A    : 0x" << std::setw(8) << dut.debug_alu_operand_a << "\n";
    std::cout << "  ALU op B    : 0x" << std::setw(8) << dut.debug_alu_operand_b << "\n";
    std::cout << "  ALU result  : 0x" << std::setw(8) << dut.debug_alu_result    << "\n";
    std::cout << std::dec;

    return failed;
}
