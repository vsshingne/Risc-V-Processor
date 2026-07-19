#include <iostream>

#include "Vwb_mux.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

int passed = 0;
int failed = 0;

void check(uint32_t expected,
           uint32_t actual,
           const std::string& test)
{
    if (expected == actual)
    {
        std::cout << GREEN << "[PASS] " << RESET << test << '\n';
        passed++;
    }
    else
    {
        std::cout << RED << "[FAIL] " << RESET << test << '\n';
        std::cout << "Expected: 0x" << std::hex << expected << '\n';
        std::cout << "Actual  : 0x" << actual << std::dec << '\n';
        failed++;
    }
}

int main(int argc, char** argv)
{
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    Vwb_mux dut;

    VerilatedVcdC* trace = new VerilatedVcdC;
    dut.trace(trace, 99);
    trace->open("waveforms/wb_mux.vcd");

    vluint64_t sim_time = 0;

    auto dump = [&]()
    {
        dut.eval();
        trace->dump(sim_time);
        sim_time += 10;
    };

      
    // Test 1 : ALU Result
      

    dut.alu_result   = 0x11111111;
    dut.memory_data  = 0xAAAAAAAA;
    dut.result_src   = 0;

    dump();

    check(0x11111111,
          dut.writeback_data,
          "Write Back = ALU");

      
    // Test 2 : Memory Result
      

    dut.alu_result   = 0x11111111;
    dut.memory_data  = 0xDEADBEEF;
    dut.result_src   = 1;

    dump();

    check(0xDEADBEEF,
          dut.writeback_data,
          "Write Back = Memory");

      

    trace->close();
    delete trace;

    std::cout << "\nPassed : " << passed << '\n';
    std::cout << "Failed : " << failed << '\n';

    return failed;
}