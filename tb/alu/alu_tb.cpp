#include <iostream>
#include <iomanip>
#include <string>
#include <cstdint>

#include "Valu.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#include "../common/alu_ops.hpp"

#define GREEN "\033[32m"
#define RED   "\033[31m"
#define RESET "\033[0m"

int passed = 0;
int failed = 0;

void test(
    Valu& alu,
    VerilatedVcdC* trace,
    vluint64_t& sim_time,
    uint32_t a,
    uint32_t b,
    ALU_OP op,
    uint32_t expected,
    bool expected_zero,
    const std::string& name)
{
   
    // Apply Inputs
  

    alu.a = a;
    alu.b = b;
    alu.alu_op = op;
    sim_time += 5;

    alu.eval();


    trace->dump(sim_time);
    sim_time += 5;

    bool pass =
        (alu.result == expected) &&
        (alu.zero == expected_zero);

    if(pass)
    {
        std::cout
            << GREEN
            << "[PASS] "
            << RESET
            << std::left
            << std::setw(12)
            << name
            << "@ "
            << sim_time
            << " ns"
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
            << "\n";

        std::cout
            << "A             : 0x"
            << std::hex << std::setw(8) << std::setfill('0') << a
            << "\n";

        std::cout
            << "B             : 0x"
            << std::setw(8) << b
            << "\n";

        std::cout
            << "Expected      : 0x"
            << std::setw(8) << expected
            << "\n";

        std::cout
            << "Actual        : 0x"
            << std::setw(8) << alu.result
            << "\n";

        std::cout
            << "Expected Zero : "
            << std::dec << expected_zero
            << "\n";

        std::cout
            << "Actual Zero   : "
            << (int)alu.zero
            << "\n\n";

        failed++;
    }
}

int main(int argc,char** argv)
{
    Verilated::commandArgs(argc,argv);

    Verilated::traceEverOn(true);

    vluint64_t sim_time = 0;

    Valu alu;

    VerilatedVcdC* trace = new VerilatedVcdC;

    alu.trace(trace,99);

    trace->open("waveforms/alu.vcd");

    std::cout
        << "=====================================\n";

    std::cout
        << "        RV32I ALU Verification\n";

    std::cout
        << "=====================================\n\n";

     
    // Arithmetic
     

    test(alu,trace,sim_time,10,20,ALU_ADD,30,false,"ADD");
    test(alu,trace,sim_time,50,30,ALU_SUB,20,false,"SUB");

    test(alu,trace,sim_time,20,20,ALU_SUB,0,true,"SUB_ZERO");

    test(alu,trace,sim_time,0,0,ALU_ADD,0,true,"ADD_ZERO");

    test(alu,trace,sim_time,
         0xFFFFFFFF,
         1,
         ALU_ADD,
         0,
         true,
         "ADD_WRAP");

     
    // Logic
     

    test(alu,trace,sim_time,0x55,0x0F,ALU_AND,0x05,false,"AND");

    test(alu,trace,sim_time,0x55,0x0F,ALU_OR,0x5F,false,"OR");

    test(alu,trace,sim_time,0x55,0x0F,ALU_XOR,0x5A,false,"XOR");

     
    // Shift
     

    test(alu,trace,sim_time,8,2,ALU_SLL,32,false,"SLL");

    test(alu,trace,sim_time,32,2,ALU_SRL,8,false,"SRL");

    test(alu,trace,sim_time,
         0xFFFFFFF0,
         2,
         ALU_SRA,
         0xFFFFFFFC,
         false,
         "SRA");

     
    // Comparison
     

    test(alu,trace,sim_time,5,10,ALU_SLT,1,false,"SLT");

    test(alu,trace,sim_time,10,5,ALU_SLT,0,true,"SLT_FALSE");

    test(alu,trace,sim_time,5,10,ALU_SLTU,1,false,"SLTU");

    test(alu,trace,sim_time,
         0xFFFFFFFF,
         1,
         ALU_SLTU,
         0,
         true,
         "SLTU_FALSE");

     

    trace->close();

    delete trace;

    std::cout
        << "\n=====================================\n";

    std::cout
        << GREEN
        << "Passed : "
        << RESET
        << passed
        << "\n";

    std::cout
        << RED
        << "Failed : "
        << RESET
        << failed
        << "\n";

    std::cout
        << "=====================================\n";

    return failed;
}