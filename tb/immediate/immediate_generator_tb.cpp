#include <iostream>

#include "Vimmediate_generator.h"
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
    if(expected == actual)
    {
        std::cout << GREEN << "[PASS] " << RESET << test << std::endl;
        passed++;
    }
    else
    {
        std::cout << RED << "[FAIL] " << RESET << test << std::endl;

        std::cout << "Expected : 0x"
                  << std::hex << expected << std::endl;

        std::cout << "Actual   : 0x"
                  << std::hex << actual << std::endl;

        std::cout << std::dec;

        failed++;
    }
}

void dump(
    Vimmediate_generator& dut,
    VerilatedVcdC* trace,
    vluint64_t& sim_time)
{
    trace->dump(sim_time);

    dut.eval();

    sim_time += 5;

    trace->dump(sim_time);

    trace->flush();

    sim_time += 5;
}

int main(int argc,char** argv)
{
    Verilated::commandArgs(argc,argv);
    Verilated::traceEverOn(true);

    vluint64_t sim_time = 0;

    Vimmediate_generator dut;

    VerilatedVcdC* trace = new VerilatedVcdC;

    dut.trace(trace,99);

    trace->open("waveforms/immediate_generator.vcd");

      
    // I-Type : addi x1,x0,10
      

    dut.instruction = 0x00A00093;

    dump(dut,trace,sim_time);

    check(10,dut.imm_i,"I-Type +10");

      
    // I-Type : addi x1,x0,-1
      

    dut.instruction = 0xFFF00093;

    dump(dut,trace,sim_time);

    check(0xFFFFFFFF,dut.imm_i,"I-Type -1");

      
    // U-Type : lui x1,0x12345
      

    dut.instruction = 0x123450B7;

    dump(dut,trace,sim_time);

    check(0x12345000,dut.imm_u,"U-Type");

      
    // S-Type : sw x1,8(x2)
      

    dut.instruction = 0x00112423;

    dump(dut,trace,sim_time);

    check(8,dut.imm_s,"S-Type");

      
    // B-Type : beq x0,x0,+8
      

    dut.instruction = 0x00000463;

    dump(dut,trace,sim_time);

    check(8,dut.imm_b,"B-Type");

      
    // J-Type : jal x0,+8
      

    dut.instruction = 0x0080006F;

    dump(dut,trace,sim_time);

    check(8,dut.imm_j,"J-Type");

      

    trace->close();

    delete trace;

    std::cout << "\n=====================\n";
    std::cout << "Passed : " << passed << std::endl;
    std::cout << "Failed : " << failed << std::endl;
    std::cout << "=====================\n";

    return failed;
}