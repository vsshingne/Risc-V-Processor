#include <iostream>
#include <iomanip>

#include "Vriscv_cpu.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

int passed = 0;
int failed = 0;

void check(uint32_t expected,
           uint32_t actual,
           const std::string &msg)
{
    if(expected == actual)
    {
        std::cout << GREEN << "[PASS] " << RESET
                  << msg << std::endl;
        passed++;
    }
    else
    {
        std::cout << RED << "[FAIL] " << RESET
                  << msg << std::endl;
        std::cout << "Expected : 0x"
                  << std::hex << expected << std::endl;
        std::cout << "Actual   : 0x"
                  << actual << std::endl;
        std::cout << std::dec;
        failed++;
    }
}

void tick(
    Vriscv_cpu &dut,
    VerilatedVcdC *trace,
    vluint64_t &time)
{
    dut.clk = 0;
    dut.eval();
    trace->dump(time);
    time += 5;

    dut.clk = 1;
    dut.eval();
    trace->dump(time);
    time += 5;

    trace->flush();
}

int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    Vriscv_cpu dut;
    VerilatedVcdC *trace = new VerilatedVcdC;
    dut.trace(trace, 99);
    trace->open("waveforms/lb.vcd");

    vluint64_t sim_time = 0;

    // RESET
    dut.reset = 1;
    dut.eval();
    trace->dump(sim_time);
    check(0x00000000, dut.debug_pc, "PC after reset");
    dut.reset = 0;

    // 1. addi x1, x0, 32
    tick(dut, trace, sim_time);
    check(0x00000004, dut.debug_pc, "PC after ADDI x1");
    check(32, dut.debug_x1, "x1 base address");

    // 2. lui x2, 0x12FF8
    tick(dut, trace, sim_time);

    // 3. ori x2, x2, 0x07F
    tick(dut, trace, sim_time);
    check(0x12FF807F, dut.debug_x2, "x2 stored pattern");

    // 4. sw x2, 0(x1)
    tick(dut, trace, sim_time);
    check(0x00000010, dut.debug_pc, "PC before LB 0(x1)");

    // 5. lb x3, 0(x1)
    check(0x03, dut.debug_opcode, "LB opcode");
    check(0x0, dut.debug_funct3, "LB funct3");
    check(32, dut.debug_memory_address, "LB address 32");
    check(0x0000007F, dut.debug_memory_read_data, "LB positive byte sign extension");
    check(1, dut.debug_reg_write, "LB reg_write");
    tick(dut, trace, sim_time);
    check(0x0000007F, dut.debug_x3, "x3 loaded sign-extended positive byte (0x7F)");

    // 6. lb x4, 1(x1)
    check(33, dut.debug_memory_address, "LB address 33");
    check(0xFFFFFF80, dut.debug_memory_read_data, "LB negative byte sign extension (0x80)");
    tick(dut, trace, sim_time);
    check(0xFFFFFF80, dut.debug_x4, "x4 loaded sign-extended negative byte (0x80)");

    // 7. lb x3, 2(x1)
    check(34, dut.debug_memory_address, "LB address 34");
    check(0xFFFFFFFF, dut.debug_memory_read_data, "LB negative byte sign extension (0xFF)");
    tick(dut, trace, sim_time);
    check(0xFFFFFFFF, dut.debug_x3, "x3 loaded sign-extended negative byte (0xFF)");

    trace->close();
    delete trace;

    std::cout << "\nPassed : " << passed
              << "\nFailed : " << failed
              << std::endl;

    return failed;
}
