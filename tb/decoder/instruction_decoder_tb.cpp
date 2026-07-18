#include <iostream>

#include "Vinstruction_decoder.h"
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

void dump(
    Vinstruction_decoder& dec,
    VerilatedVcdC* trace,
    vluint64_t& sim_time)
{
    trace->dump(sim_time);

    dec.eval();

    sim_time += 5;

    trace->dump(sim_time);

    trace->flush();

    sim_time += 5;
}


int main(int argc,char** argv)
{
    Verilated::commandArgs(argc, argv);

    Verilated::traceEverOn(true);

    vluint64_t sim_time = 0;

    Vinstruction_decoder dec;

    VerilatedVcdC* trace = new VerilatedVcdC;

    dec.trace(trace, 99);

    trace->open("waveforms/instruction_decoder.vcd");
      
    // add x3,x1,x3
      

      
    // Instruction 0
      

    dec.instruction = 0x00000013;

    dump(dec,trace,sim_time);

    check(0x13,dec.opcode,"NOP opcode");

      
    // Instruction 1
      

    dec.instruction = 0x00100093;

    dump(dec,trace,sim_time);

    check(1,dec.rd,"ADDI rd");

      
    // Instruction 2
      

    dec.instruction = 0x00200113;

    dump(dec,trace,sim_time);

    check(2,dec.rd,"ADDI rd2");

      
    // Instruction 3
      

    dec.instruction = 0x003081B3;

    dump(dec,trace,sim_time);

    check(0x33,dec.opcode,"ADD opcode");
    check(3,dec.rd,"ADD rd");
    check(0,dec.funct3,"ADD funct3");
    check(1,dec.rs1,"ADD rs1");
    check(3,dec.rs2,"ADD rs2");
    check(0,dec.funct7,"ADD funct7");

    std::cout
        << "\nPassed : "
        << passed
        << "\nFailed : "
        << failed
        << std::endl;
    trace->close(); 
    delete trace;
    return failed;
}