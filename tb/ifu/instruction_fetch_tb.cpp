#include <iostream>

#include "Vinstruction_fetch.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

int passed = 0;
int failed = 0;

void check(uint32_t expected,
           uint32_t actual,
           const std::string& msg)
{
    if(expected == actual)
    {
        std::cout
            << GREEN
            << "[PASS] "
            << RESET
            << msg
            << std::endl;

        passed++;
    }
    else
    {
        std::cout
            << RED
            << "[FAIL] "
            << RESET
            << msg
            << std::endl;

        std::cout
            << "Expected : 0x"
            << std::hex
            << expected
            << std::endl;

        std::cout
            << "Actual   : 0x"
            << actual
            << std::endl;

        failed++;
    }
}

void tick(
    Vinstruction_fetch& dut,
    VerilatedVcdC* trace,
    vluint64_t& time)
{
    dut.clk = 0;
    dut.eval();
    trace->dump(time);

    time += 5;

    dut.clk = 1;
    dut.eval();
    trace->dump(time);

    time += 5;
}

int main(int argc,char** argv)
{
    Verilated::commandArgs(argc,argv);

    Verilated::traceEverOn(true);

    vluint64_t sim_time = 0;

    Vinstruction_fetch dut;

    VerilatedVcdC* trace =
        new VerilatedVcdC;

    dut.trace(trace,99);

    trace->open("waveforms/instruction_fetch.vcd");

      
    // Reset
      

    dut.reset = 1;
    dut.pc_write = 0;

    tick(dut,trace,sim_time);

    check(0,dut.pc,"Reset PC");

      
    // Fetch instruction 0
      

    dut.reset = 0;
    dut.pc_write = 1;
    dut.pc_next = 0;

    tick(dut,trace,sim_time);

    check(0x00000013,
          dut.instruction,
          "Instruction 0");

      
    // Fetch instruction 1
      

    dut.pc_next = 4;

    tick(dut,trace,sim_time);

    check(4,dut.pc,"PC=4");

    check(0x00100093,
          dut.instruction,
          "Instruction 1");

      
    // Fetch instruction 2
      

    dut.pc_next = 8;

    tick(dut,trace,sim_time);

    check(8,dut.pc,"PC=8");

    check(0x00200113,
          dut.instruction,
          "Instruction 2");

      
    // Fetch instruction 3
      

    dut.pc_next = 12;

    tick(dut,trace,sim_time);

    check(12,dut.pc,"PC=12");

    check(0x003081B3,
          dut.instruction,
          "Instruction 3");

      

    trace->close();

    delete trace;

    std::cout
        << "\n=====================\n";

    std::cout
        << "Passed : "
        << passed
        << "\n";

    std::cout
        << "Failed : "
        << failed
        << "\n";

    return failed;
}