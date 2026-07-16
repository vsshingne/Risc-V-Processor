#include <iostream>

#include "Vprogram_counter.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

int passed = 0;
int failed = 0;

void check(uint32_t expected,uint32_t actual,const std::string& msg)
{
    if(expected==actual)
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
            << "Expected : "
            << expected
            << std::endl;

        std::cout
            << "Actual   : "
            << actual
            << std::endl;

        failed++;
    }
}

void tick(
    Vprogram_counter& pc,
    VerilatedVcdC* trace,
    vluint64_t& time)
{
    pc.clk = 0;
    pc.eval();
    trace->dump(time);

    time += 5;

    pc.clk = 1;
    pc.eval();
    trace->dump(time);

    time += 5;
}

int main(int argc,char** argv)
{
    Verilated::commandArgs(argc,argv);

    Verilated::traceEverOn(true);

    vluint64_t sim_time = 0;

    Vprogram_counter pc;

    VerilatedVcdC* trace = new VerilatedVcdC;

    pc.trace(trace,99);

    trace->open("waveforms/program_counter.vcd");

      
    // Reset
      

    pc.reset = 1;
    pc.pc_write = 0;

    tick(pc,trace,sim_time);

    check(0,pc.pc,"RESET");

      
    // PC = 4
      

    pc.reset = 0;

    pc.pc_write = 1;
    pc.pc_next = 4;

    tick(pc,trace,sim_time);

    check(4,pc.pc,"PC=4");

      
    // PC = 8
      

    pc.pc_next = 8;

    tick(pc,trace,sim_time);

    check(8,pc.pc,"PC=8");

      
    // PC = 12
      

    pc.pc_next = 12;

    tick(pc,trace,sim_time);

    check(12,pc.pc,"PC=12");

      
    // Stall
      

    pc.pc_write = 0;
    pc.pc_next = 100;

    tick(pc,trace,sim_time);

    check(12,pc.pc,"STALL");

      
    // Reset Again
      

    pc.reset = 1;

    tick(pc,trace,sim_time);

    check(0,pc.pc,"RESET AGAIN");

    trace->close();

    delete trace;

    std::cout
        << "\n====================\n";

    std::cout
        << "Passed : "
        << passed
        << "\n";

    std::cout
        << "Failed : "
        << failed
        << "\n";

    std::cout
        << "====================\n";

    return failed;
}