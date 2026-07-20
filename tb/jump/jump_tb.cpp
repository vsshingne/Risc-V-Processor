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

    trace->open("waveforms/jump.vcd");

    vluint64_t sim_time = 0;

      
    // RESET
      

    dut.reset = 1;

    dut.eval();
    trace->dump(sim_time);

    check(0x00000000, dut.debug_pc, "PC after reset");

      
    // Instruction 1
    // addi x1,x0,5
      

    dut.reset = 0;

    tick(dut, trace, sim_time);

    check(0x00000004, dut.debug_pc, "PC after ADDI");

    check(5, dut.debug_x1, "x1");

      
    // Instruction 2
    // j target
      

    tick(dut, trace, sim_time);

    std::cout << "\nAfter JUMP\n";

    std::cout << "PC      : 0x"
              << std::hex
              << dut.debug_pc
              << '\n';

    std::cout << "PC Next : 0x"
              << dut.debug_pc_next
              << '\n';

    std::cout << "Jump    : "
              << std::dec
              << (int)dut.debug_jump
              << '\n';

      
    check(0x01400193,
      dut.debug_instruction,
      "Fetched target instruction");
    // One more cycle
      

    tick(dut, trace, sim_time);

    std::cout << "\nAfter Jump Target\n";

    std::cout << "PC : 0x"
              << std::hex
              << dut.debug_pc
              << '\n';

      
    // Final register values
      

    check(5, dut.debug_x1, "x1");

    check(0, dut.debug_x2, "x2 skipped");

    check(20, dut.debug_x3, "x3");

    trace->close();

    delete trace;

    std::cout
        << "\nPassed : "
        << passed
        << std::endl;

    std::cout
        << "Failed : "
        << failed
        << std::endl;

    return failed;
}