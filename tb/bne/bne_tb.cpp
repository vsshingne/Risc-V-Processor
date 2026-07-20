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

void tick(Vriscv_cpu &dut,
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

    trace->open("waveforms/bne.vcd");

    vluint64_t sim_time = 0;

    dut.reset = 1;

    dut.eval();
    trace->dump(sim_time);

    check(0,dut.debug_pc,"PC after reset");

    dut.reset = 0;

    tick(dut,trace,sim_time);

    check(5,dut.debug_x1,"x1");

    tick(dut,trace,sim_time);

    check(5,dut.debug_x2,"x2");

    tick(dut,trace,sim_time);

    std::cout << "\nAfter BNE\n";

    std::cout << "PC      : 0x"
              << std::hex
              << dut.debug_pc
              << '\n';

    std::cout << "PC Next : 0x"
              << dut.debug_pc_next
              << '\n';

    std::cout << "Zero    : "
              << std::dec
              << (int)dut.debug_zero
              << '\n';

    std::cout << "Branch  : "
              << (int)dut.debug_branch
              << '\n';

    std::cout << "BNE     : "
              << (int)dut.debug_branch_not_equal
              << '\n';

    tick(dut,trace,sim_time);

    std::cout << "\nInstruction : 0x"
            << std::hex << dut.debug_instruction << '\n';

    std::cout << "Opcode : 0x"
            << (int)dut.debug_opcode << '\n';

    std::cout << "Funct3 : 0x"
            << (int)dut.debug_funct3 << '\n';

    std::cout << "Zero   : "
            << std::dec << (int)dut.debug_zero << '\n';

    std::cout << "Branch : "
            << (int)dut.debug_branch << '\n';

    std::cout << "BNE    : "
            << (int)dut.debug_branch_not_equal << '\n';

    check(5,dut.debug_x1,"x1");
    check(5,dut.debug_x2,"x2");
    check(100,dut.debug_x3,"x3 executed");
    tick(dut,trace,sim_time); 
    check(20,dut.debug_x4,"x4");

    trace->close();

    delete trace;

    std::cout << "\nPassed : "
              << passed
              << "\nFailed : "
              << failed
              << std::endl;

    return failed;
}