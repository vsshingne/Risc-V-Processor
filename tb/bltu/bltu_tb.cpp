#include <iostream>

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
                  << msg << '\n';
        passed++;
    }
    else
    {
        std::cout << RED << "[FAIL] " << RESET
                  << msg << '\n';

        std::cout << "Expected : 0x"
                  << std::hex << expected << '\n';

        std::cout << "Actual   : 0x"
                  << actual << '\n';

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

int main(int argc,char **argv)
{
    Verilated::commandArgs(argc,argv);
    Verilated::traceEverOn(true);

    Vriscv_cpu dut;

    VerilatedVcdC *trace = new VerilatedVcdC;

    dut.trace(trace,99);

    trace->open("waveforms/blt.vcd");

    vluint64_t sim_time = 0;

    dut.reset = 1;

    dut.eval();
    trace->dump(sim_time);

    check(0,dut.debug_pc,"PC after reset");

    dut.reset = 0;

    tick(dut,trace,sim_time);

    check(10,dut.debug_x1,"x1");

    tick(dut,trace,sim_time);

    check(5,dut.debug_x2,"x2");

    tick(dut,trace,sim_time);

    std::cout << "\nAfter BLTU\n";

    std::cout << "PC      : 0x"
              << std::hex
              << dut.debug_pc
              << '\n';

    std::cout << "PC Next : 0x"
              << dut.debug_pc_next
              << '\n';

    std::cout << "LT      : "
              << std::dec
              << (int)dut.debug_lt_signed
              << '\n';

    std::cout << "BLTU    : "
              << (int)dut.debug_branch_less_than_unsigned
              << '\n';

    tick(dut,trace,sim_time);

    check(10,dut.debug_x1,"x1");
    check(5,dut.debug_x2,"x2");
    tick(dut, trace, sim_time);          // executes addi x3

    check(100, dut.debug_x3, "x3 executed");

    tick(dut, trace, sim_time);          // executes addi x4

    check(20, dut.debug_x4, "x4");;

    trace->close();

    delete trace;

    std::cout << "\nPassed : "
              << passed
              << "\nFailed : "
              << failed
              << std::endl;

    return failed;
}
