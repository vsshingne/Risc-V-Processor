#include <iostream>

#include "Valu_mux.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

int passed = 0;
int failed = 0;

void check(uint32_t expected,
           uint32_t actual,
           const std::string &test)
{
    if(expected == actual)
    {
        std::cout << GREEN << "[PASS] " << RESET << test << std::endl;
        passed++;
    }
    else
    {
        std::cout << RED << "[FAIL] " << RESET << test << std::endl;
        std::cout << "Expected : 0x" << std::hex << expected << std::endl;
        std::cout << "Actual   : 0x" << std::hex << actual << std::endl;
        std::cout << std::dec;
        failed++;
    }
}

int main(int argc,char** argv)
{
    Verilated::commandArgs(argc,argv);

    Verilated::traceEverOn(true);

    Valu_mux dut;

    VerilatedVcdC* trace = new VerilatedVcdC;

    dut.trace(trace,99);

    trace->open("waveforms/alu_mux.vcd");

    vluint64_t sim_time = 0;

    auto dump = [&]()
    {
        dut.eval();
        trace->dump(sim_time);
        sim_time += 10;
    };

      
    // Test 1 : Register Source
      

    dut.rs2_data = 0x12345678;
    dut.imm_data = 0xAAAAAAAA;
    dut.alu_src = 0;

    dump();

    check(0x12345678,
          dut.alu_operand_b,
          "ALU Source = Register");

      
    // Test 2 : Immediate Source
      

    dut.rs2_data = 0x12345678;
    dut.imm_data = 0x55555555;
    dut.alu_src = 1;

    dump();

    check(0x55555555,
          dut.alu_operand_b,
          "ALU Source = Immediate");

      

    trace->close();
    delete trace;

    std::cout << "\nPassed : " << passed << std::endl;
    std::cout << "Failed : " << failed << std::endl;

    return failed;
}