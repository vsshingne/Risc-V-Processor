#include <iostream>

#include "Vdata_memory.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

int passed = 0;
int failed = 0;

void check(
    uint32_t expected,
    uint32_t actual,
    const std::string& test)
{
    if(expected == actual)
    {
        std::cout
            << GREEN
            << "[PASS] "
            << RESET
            << test
            << std::endl;

        passed++;
    }
    else
    {
        std::cout
            << RED
            << "[FAIL] "
            << RESET
            << test
            << std::endl;

        std::cout
            << "Expected : 0x"
            << std::hex
            << expected
            << std::endl;

        std::cout
            << "Actual   : 0x"
            << std::hex
            << actual
            << std::endl;

        std::cout << std::dec;

        failed++;
    }
}

void tick(
    Vdata_memory& dut,
    VerilatedVcdC* trace,
    vluint64_t& sim_time)
{
    dut.clk = 0;
    dut.eval();
    trace->dump(sim_time);
    sim_time += 5;

    dut.clk = 1;
    dut.eval();
    trace->dump(sim_time);
    sim_time += 5;

    trace->flush();
}

int main(int argc,char** argv)
{
    Verilated::commandArgs(argc,argv);

    Verilated::traceEverOn(true);

    vluint64_t sim_time = 0;

    Vdata_memory dut;

    VerilatedVcdC* trace = new VerilatedVcdC;

    dut.trace(trace,99);

    trace->open("waveforms/data_memory.vcd");

      
    // Initialize
      

    dut.mem_read = 0;
    dut.mem_write = 0;
    dut.mem_size = 2;
    dut.mem_unsigned = 0;
    dut.address = 0;
    dut.write_data = 0;

      
    // Test 1
    // Write 0x12345678 @ address 0
      

    dut.address = 0;
    dut.write_data = 0x12345678;
    dut.mem_write = 1;

    tick(dut,trace,sim_time);

    dut.mem_write = 0;
    dut.mem_read = 1;

    dut.eval();

    check(0x12345678,dut.read_data,"Write/Read Address 0");

      
    // Test 2
    // Write 0xDEADBEEF @ address 4
      

    dut.mem_read = 0;
    dut.mem_write = 1;

    dut.address = 4;
    dut.write_data = 0xDEADBEEF;

    tick(dut,trace,sim_time);

    dut.mem_write = 0;
    dut.mem_read = 1;

    dut.eval();

    check(0xDEADBEEF,dut.read_data,"Write/Read Address 4");

      
    // Test 3
    // mem_read disabled
      

    dut.mem_read = 0;

    dut.eval();

    check(0,dut.read_data,"Read Disabled");

      
    // Test 4
    // Write disabled
      

    dut.mem_write = 0;
    dut.mem_read = 1;

    dut.address = 4;
    dut.write_data = 0xAAAAAAAA;

    tick(dut,trace,sim_time);

    dut.eval();

    check(0xDEADBEEF,dut.read_data,"Write Disabled");

      

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
