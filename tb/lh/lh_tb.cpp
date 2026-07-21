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
    trace->open("waveforms/lh.vcd");

    vluint64_t sim_time = 0;

    // RESET
    dut.reset = 1;
    dut.eval();
    trace->dump(sim_time);
    check(0x00000000, dut.debug_pc, "PC after reset");
    dut.reset = 0;

    // 1. addi x1, x0, 32
    tick(dut, trace, sim_time);

    // 2. lui x2, 0x80001
    tick(dut, trace, sim_time);

    // 3. ori x2, x2, 0x234
    tick(dut, trace, sim_time);

    // 4. sw x2, 0(x1)
    tick(dut, trace, sim_time);

    // 5. lh x3, 0(x1)
    check(0x03, dut.debug_opcode, "LH opcode");
    check(0x1, dut.debug_funct3, "LH funct3");
    check(32, dut.debug_memory_address, "LH address 32");
    check(0x00001234, dut.debug_memory_read_data, "LH positive halfword sign extension");
    tick(dut, trace, sim_time);
    check(0x00001234, dut.debug_x3, "x3 loaded positive halfword (0x1234)");

    // 6. lh x4, 2(x1)
    check(34, dut.debug_memory_address, "LH address 34");
    check(0xFFFF8000, dut.debug_memory_read_data, "LH negative halfword sign extension");
    tick(dut, trace, sim_time);
    check(0xFFFF8000, dut.debug_x4, "x4 loaded sign-extended negative halfword (0x8000)");

    // 7. lh x3, 1(x1)  (Unaligned halfword access)
    check(33, dut.debug_memory_address, "LH address 33 (unaligned)");
    check(0x00000000, dut.debug_memory_read_data, "LH unaligned access returns 0");
    tick(dut, trace, sim_time);
    check(0x00000000, dut.debug_x3, "x3 receives 0 on unaligned LH");

    trace->close();
    delete trace;

    std::cout << "\nPassed : " << passed
              << "\nFailed : " << failed
              << std::endl;

    return failed;
}
