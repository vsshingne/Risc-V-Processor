#include <iostream>

#include "Vinstruction_memory.h"
#include "verilated.h"

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

int passed = 0;
int failed = 0;

void test(
    Vinstruction_memory& mem,
    uint32_t addr,
    uint32_t expected,
    const std::string& name)
{
    mem.address = addr;

    mem.eval();

    if(mem.instruction == expected)
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

        failed++;
    }
}

int main(int argc,char** argv)
{
    Verilated::commandArgs(argc,argv);

    Vinstruction_memory mem;

    test(mem,0,0x00000013,"Instr0");

    test(mem,4,0x00100093,"Instr1");

    test(mem,8,0x00200113,"Instr2");

    test(mem,12,0x003081B3,"Instr3");

    std::cout
        << "\nPassed : "
        << passed
        << "\nFailed : "
        << failed
        << std::endl;

    return failed;
}