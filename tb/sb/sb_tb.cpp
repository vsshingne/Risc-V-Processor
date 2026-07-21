#include <iostream>
#include <iomanip>

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

int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    Vriscv_cpu dut;
    VerilatedVcdC *trace = new VerilatedVcdC;
    dut.trace(trace, 99);
    trace->open("waveforms/sb.vcd");

    vluint64_t sim_time = 0;

    // RESET
    dut.reset = 1;
    dut.eval();
    trace->dump(sim_time);
    check(0x00000000, dut.debug_pc, "PC after reset");
    dut.reset = 0;

    // 1. addi x1, x0, 32
    tick(dut, trace, sim_time);

    // 2. lui x2, 0x11223
    tick(dut, trace, sim_time);

    // 3. ori x2, x2, 0x344
    tick(dut, trace, sim_time);

    // 4. sw x2, 0(x1)
    tick(dut, trace, sim_time);

    // 5. addi x4, x0, 170  (0xAA)
    tick(dut, trace, sim_time);

    // 6. sb x4, 1(x1)
    check(0x23, dut.debug_opcode, "SB opcode");
    check(0x0, dut.debug_funct3, "SB funct3");
    check(33, dut.debug_memory_address, "SB address 33");
    check(170, dut.debug_memory_write_data, "SB write data 0xAA");
    check(0, dut.debug_reg_write, "SB reg_write disabled");
    tick(dut, trace, sim_time);

    // 7. lw x3, 0(x1)  -> read word
    tick(dut, trace, sim_time);
    check(0x1122AA44, dut.debug_x3, "x3 word after SB (only byte 1 modified)");

    // 8. lbu x4, 1(x1)
    tick(dut, trace, sim_time);
    check(0x000000AA, dut.debug_x4, "x4 modified byte value 0xAA");

    // 9. lbu x4, 0(x1)
    tick(dut, trace, sim_time);
    check(0x00000044, dut.debug_x4, "x4 neighbor byte 0 unmodified (0x44)");

    // 10. lbu x4, 2(x1)
    tick(dut, trace, sim_time);
    check(0x00000022, dut.debug_x4, "x4 neighbor byte 2 unmodified (0x22)");

    trace->close();
    delete trace;

    std::cout << "\nPassed : " << passed
              << "\nFailed : " << failed
              << std::endl;

    return failed;
}
