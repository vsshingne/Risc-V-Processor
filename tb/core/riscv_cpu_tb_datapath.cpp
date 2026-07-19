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
        std::cout << GREEN
                  << "[PASS] "
                  << RESET
                  << msg
                  << std::endl;

        passed++;
    }
    else
    {
        std::cout << RED
                  << "[FAIL] "
                  << RESET
                  << msg
                  << std::endl;

        std::cout << "Expected : 0x"
                  << std::hex
                  << expected
                  << std::endl;

        std::cout << "Actual   : 0x"
                  << actual
                  << std::endl;

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

    trace->open("waveforms/riscv_cpu.vcd");

    vluint64_t sim_time = 0;

      
    // Reset
      

      
    // Reset
      

    dut.reset = 1;

    // Evaluate without advancing the clock
    dut.eval();
    trace->dump(sim_time);

    // Verify reset state
    check(0x00000000, dut.debug_pc, "PC after reset");
    check(0x00500093, dut.debug_instruction, "Instruction @0");

    // Decoder for addi x1,x0,5
    check(0x13, dut.debug_opcode, "Opcode");
    check(1,    dut.debug_rd,     "RD");
    check(0,    dut.debug_rs1,    "RS1");
    check(5,    dut.debug_rs2,    "RS2 Field");
    check(0,    dut.debug_funct3, "Funct3");
    check(0,    dut.debug_funct7, "Funct7");

    
    //immediate checks
    check(5, dut.debug_immediate, "Immediate");

    check(0, dut.debug_rs1_data,      "RS1 Data");
    check(0, dut.debug_rs2_data,      "RS2 Data");

    check(0, dut.debug_alu_operand_a, "ALU Operand A");
    check(5, dut.debug_alu_operand_b, "ALU Operand B");

    check(5, dut.debug_alu_result, "ALU Result");
    check(0, dut.debug_zero,       "Zero Flag");

    check(0, dut.debug_memory_write_data, "Memory Data");

    check(5, dut.debug_writeback, "Writeback");

    check(5, dut.debug_x1, "x1");

      
    // Instruction 1 : addi x1,x0,5
      

    dut.reset = 0;

    tick(dut, trace, sim_time);

    // Now PC = 4
    check(0x00000004, dut.debug_pc, "PC after Inst1");
    check(0x00A00113, dut.debug_instruction, "Instruction @4");

    // Decoder for addi x2,x0,10
    check(0x13, dut.debug_opcode, "Opcode");
    check(2,    dut.debug_rd,     "RD");
    check(0,    dut.debug_rs1,    "RS1");
    check(10,   dut.debug_rs2,    "RS2 Field");
    check(0,    dut.debug_funct3, "Funct3");
    check(0,    dut.debug_funct7, "Funct7");


    check(10, dut.debug_immediate, "Immediate");

    check(0, dut.debug_rs1_data,      "RS1 Data");
    check(0, dut.debug_rs2_data,      "RS2 Data");

    check(0, dut.debug_alu_operand_a, "ALU Operand A");
    check(10, dut.debug_alu_operand_b, "ALU Operand B");

    check(10, dut.debug_alu_result, "ALU Result");
    check(0, dut.debug_zero,       "Zero Flag");

    check(0, dut.debug_memory_write_data, "Memory Data");

    check(10, dut.debug_writeback, "Writeback");

    check(5,  dut.debug_x1, "x1");
    check(10, dut.debug_x2, "x2");
      
    // Instruction 2 : addi x2,x0,10
      

    tick(dut, trace, sim_time);

    check(0x00000008, dut.debug_pc, "PC after Inst2");
    check(0x002081B3, dut.debug_instruction, "Instruction @8");

    // Decoder of Instruction 2

    check(0x33, dut.debug_opcode, "Opcode");
    check(3,    dut.debug_rd,     "RD");
    check(1,    dut.debug_rs1,    "RS1");
    check(2,    dut.debug_rs2,    "RS2");
    check(0,    dut.debug_funct3, "Funct3");
    check(0,    dut.debug_funct7, "Funct7");

    check(0, dut.debug_rs1_data,      "RS1 Data");
    check(0, dut.debug_rs2_data,      "RS2 Data");

    check(0, dut.debug_alu_operand_a, "ALU Operand A");
    check(0, dut.debug_alu_operand_b, "ALU Operand B");

    check(0, dut.debug_alu_result, "ALU Result");
    check(1, dut.debug_zero,       "Zero Flag");

    check(0, dut.debug_memory_write_data, "Memory Data");

    check(0, dut.debug_writeback, "Writeback");
        
    // Instruction 3 : add x3,x1,x2
      

    tick(dut, trace, sim_time);

    check(0x0000000C, dut.debug_pc, "PC after Inst3");
    check(0x40310233, dut.debug_instruction, "Instruction @C");

    // Decoder

    check(0x33, dut.debug_opcode, "Opcode");
    check(4,    dut.debug_rd,     "RD");
    check(2,    dut.debug_rs1,    "RS1");
    check(3,    dut.debug_rs2,    "RS2");
    check(0,    dut.debug_funct3, "Funct3");
    check(0x20, dut.debug_funct7, "Funct7");

    check(0, dut.debug_rs1_data, "RS1 Data");
    check(0, dut.debug_rs2_data, "RS2 Data");

    check(0, dut.debug_alu_operand_a, "ALU Operand A");
    check(0, dut.debug_alu_operand_b, "ALU Operand B");

    check(0, dut.debug_alu_result, "ALU Result");
    check(1, dut.debug_zero,       "Zero Flag");

    check(0, dut.debug_memory_write_data, "Memory Data");

    check(5,  dut.debug_x1, "x1");
    check(10, dut.debug_x2, "x2");
    check(15, dut.debug_x3, "x3");
      
    // Instruction 4 : sub x4,x2,x3
      

    tick(dut, trace, sim_time);
    check(0x00000010, dut.debug_pc, "PC after Inst4");
    check(0x00000000, dut.debug_instruction, "Instruction @10");

    check(0, dut.debug_rs1_data, "RS1 Data");
    check(0, dut.debug_rs2_data, "RS2 Data");

    check(0, dut.debug_alu_result, "ALU Result");
    check(1, dut.debug_zero,       "Zero Flag");

    check(0, dut.debug_memory_write_data, "Memory Data");

    check(0, dut.debug_writeback, "Writeback");

    check(5,  dut.debug_x1, "x1");
    check(10, dut.debug_x2, "x2");
    check(15, dut.debug_x3, "x3");
    check(10, dut.debug_x4, "x4");

    // Display fetched instruction
      

    std::cout
        << "\nCurrent Instruction : 0x"
        << std::hex
        << dut.debug_instruction
        << std::dec
        << std::endl;

      

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

    std::cout << "RS1 Data      : " << dut.debug_rs1_data << '\n';
    std::cout << "RS2 Data      : " << dut.debug_rs2_data << '\n';

    std::cout << "ALU Operand A : " << dut.debug_alu_operand_a << '\n';
    std::cout << "ALU Operand B : " << dut.debug_alu_operand_b << '\n';


    std::cout << "PC          : " << std::hex << dut.debug_pc << '\n';
    std::cout << "Instruction : " << dut.debug_instruction << '\n';
    std::cout << "ALU Result  : " << dut.debug_alu_result << '\n';
    std::cout << "Write Back  : " << dut.debug_writeback << '\n';
    std::cout << "Reg Write   : " << static_cast<int>(dut.debug_reg_write)<< '\n';
    std::cout << "Zero        : " << static_cast<int>(dut.debug_zero) << '\n';

    return failed;
}