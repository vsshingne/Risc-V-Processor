#include <iostream>

#include "Vriscv_cpu.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

int passed = 0;
int failed = 0;

void check(uint32_t expected,
           uint32_t actual,
           const std::string &msg)
{
    if(expected == actual)
    {
        std::cout << GREEN << "[PASS] " << RESET
                  << msg << std::endl;

        passed++;
    }
    else
    {
        std::cout << RED << "[FAIL] " << RESET
                  << msg << std::endl;

        std::cout << "Expected : 0x"
                  << std::hex << expected << std::endl;

        std::cout << "Actual   : 0x"
                  << actual << std::endl;

        std::cout << std::dec;

        failed++;
    }
}

void tick(
    Vriscv_cpu &dut,
    VerilatedVcdC *trace,
    vluint64_t &time)
{
    dut.clk = 0;
    dut.eval();
    trace->dump(time);
    time += 5;

    dut.clk = 1;
    dut.eval();
    trace->dump(time);
    time += 5;

    trace->flush();
}

int main(int argc,char **argv)
{
    Verilated::commandArgs(argc,argv);
    Verilated::traceEverOn(true);

    Vriscv_cpu dut;

    VerilatedVcdC *trace = new VerilatedVcdC;

    dut.trace(trace,99);

    trace->open("waveforms/lw.vcd");

    vluint64_t sim_time = 0;

      
    // RESET
      

    dut.reset = 1;

    dut.eval();
    trace->dump(sim_time);

    check(0x00000000,dut.debug_pc,"PC after reset");

    dut.reset = 0;

      
    // addi x1,x0,32
      

    tick(dut,trace,sim_time);

    check(0x00000004,dut.debug_pc,"PC after ADDI x1");
    check(32,dut.debug_x1,"x1 base address");

      
    // addi x2,x0,1963
      

    tick(dut,trace,sim_time);

    check(0x00000008,dut.debug_pc,"PC before SW");
    check(1963,dut.debug_x2,"x2 store value");

      
    // sw x2,12(x1)
      

    check(0x23,dut.debug_opcode,"SW opcode");
    check(0x2,dut.debug_funct3,"SW funct3");
    check(12,dut.debug_immediate,"SW immediate");
    check(44,dut.debug_memory_address,"SW address");
    check(1963,dut.debug_memory_write_data,"SW write data");
    check(0,dut.debug_reg_write,"SW reg_write");

    tick(dut,trace,sim_time);

    check(0x0000000C,dut.debug_pc,"PC before LW");

      
    // lw x3,12(x1)
      

    check(0x03,dut.debug_opcode,"LW opcode");
    check(0x2,dut.debug_funct3,"LW funct3");
    check(12,dut.debug_immediate,"LW immediate");
    check(44,dut.debug_memory_address,"LW address");
    check(1963,dut.debug_memory_read_data,"LW memory read data");
    check(1963,dut.debug_writeback,"LW writeback data");
    check(1,dut.debug_reg_write,"LW reg_write");

    tick(dut,trace,sim_time);

    check(0x00000010,dut.debug_pc,"PC after LW");
    check(1963,dut.debug_x3,"x3 loaded value");

      
    // addi x4,x3,5
      

    tick(dut,trace,sim_time);

    check(0x00000014,dut.debug_pc,"PC after ADDI x4");
    check(1968,dut.debug_x4,"x4 uses loaded value");

    trace->close();

    delete trace;

    std::cout << "\nPassed : "
              << passed
              << "\nFailed : "
              << failed
              << std::endl;

    return failed;
}
