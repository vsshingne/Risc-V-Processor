#include <iostream>
#include <iomanip>
#include <string>

#include "Vregister_file.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#define GREEN "\033[32m"
#define RED   "\033[31m"
#define RESET "\033[0m"

int passed = 0;
int failed = 0;

void tick(
    Vregister_file& rf,
    VerilatedVcdC* trace,
    vluint64_t& sim_time)
{
    rf.clk = 0;
    rf.eval();
    trace->dump(sim_time);

    sim_time += 5;

    rf.clk = 1;
    rf.eval();
    trace->dump(sim_time);

    sim_time += 5;
}

void check(
    uint32_t expected,
    uint32_t actual,
    const std::string& name)
{
    if(expected == actual)
    {
        std::cout
            << GREEN
            << "[PASS] "
            << RESET
            << name
            << std::endl;

        passed++;
    }
    else
    {
        std::cout
            << RED
            << "[FAIL] "
            << RESET
            << name
            << std::endl;

        std::cout
            << "Expected : 0x"
            << std::hex << expected
            << std::endl;

        std::cout
            << "Actual   : 0x"
            << actual
            << std::endl;

        failed++;
    }
}

int main(int argc,char** argv)
{
    Verilated::commandArgs(argc,argv);

    Verilated::traceEverOn(true);

    vluint64_t sim_time = 0;

    Vregister_file rf;

    VerilatedVcdC* trace = new VerilatedVcdC;

    rf.trace(trace,99);

    trace->open("waveforms/register_file.vcd");

      
    // Reset
      

    rf.reset = 1;
    rf.we = 0;

    tick(rf,trace,sim_time);

    rf.reset = 0;

      
    // Write x5 = 123
      

    rf.we = 1;
    rf.rd_addr = 5;
    rf.rd_data = 123;

    tick(rf,trace,sim_time);

    rf.we = 0;

    rf.rs1_addr = 5;

    rf.eval();

    check(123,rf.rs1_data,"WRITE x5");

      
    // Write x10 = 999
      

    rf.we = 1;
    rf.rd_addr = 10;
    rf.rd_data = 999;

    tick(rf,trace,sim_time);

    rf.we = 0;

    rf.rs1_addr = 10;

    rf.eval();

    check(999,rf.rs1_data,"WRITE x10");

      
    // Read Two Registers
      

    rf.rs1_addr = 5;
    rf.rs2_addr = 10;

    rf.eval();

    check(123,rf.rs1_data,"READ RS1");
    check(999,rf.rs2_data,"READ RS2");

      
    // x0 must remain zero
      

    rf.we = 1;
    rf.rd_addr = 0;
    rf.rd_data = 0xFFFFFFFF;

    tick(rf,trace,sim_time);

    rf.we = 0;

    rf.rs1_addr = 0;

    rf.eval();

    check(0,rf.rs1_data,"X0 IMMUTABLE");

      
    // Write Disable
      

    rf.we = 0;
    rf.rd_addr = 5;
    rf.rd_data = 5555;

    tick(rf,trace,sim_time);

    rf.rs1_addr = 5;

    rf.eval();

    check(123,rf.rs1_data,"WRITE DISABLE");

      

    trace->close();

    delete trace;

    std::cout
        << "\n========================\n";

    std::cout
        << "Passed : "
        << passed
        << std::endl;

    std::cout
        << "Failed : "
        << failed
        << std::endl;

    std::cout
        << "========================\n";

    return failed;
}