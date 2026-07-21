#include <iostream>

#include "Vcontrol_unit.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#define GREEN "\033[32m"
#define RED   "\033[31m"
#define RESET "\033[0m"

int passed = 0;
int failed = 0;

void check(uint32_t expected,
           uint32_t actual,
           const std::string& msg)
{
    if(expected == actual)
    {
        std::cout
            << GREEN
            << "[PASS] "
            << RESET
            << msg
            << std::endl;

        passed++;
    }
    else
    {
        std::cout
            << RED
            << "[FAIL] "
            << RESET
            << msg
            << std::endl;

        failed++;
    }
}

void dump(
    Vcontrol_unit& dut,
    VerilatedVcdC* trace,
    vluint64_t& time)
{
    trace->dump(time);

    dut.eval();

    time += 5;

    trace->dump(time);

    trace->flush();

    time += 5;
}

int main(int argc,char** argv)
{
    Verilated::commandArgs(argc,argv);

    Verilated::traceEverOn(true);

    vluint64_t sim_time = 0;

    Vcontrol_unit dut;

    VerilatedVcdC* trace = new VerilatedVcdC;

    dut.trace(trace,99);

    trace->open("waveforms/control_unit.vcd");


    // ADD


    dut.opcode = 0b0110011;
    dut.funct3 = 0b000;
    dut.funct7 = 0b0000000;

    dump(dut,trace,sim_time);

    check(1,dut.reg_write,"ADD reg_write");
    check(0,dut.alu_src,"ADD alu_src");
    check(0,dut.mem_read,"ADD mem_read");
    check(0,dut.mem_write,"ADD mem_write");
    check(0,dut.branch,"ADD branch");


    // SUB


    dut.opcode = 0b0110011;
    dut.funct3 = 0b000;
    dut.funct7 = 0b0100000;

    dump(dut,trace,sim_time);

    check(1,dut.reg_write,"SUB reg_write");


    // ADDI


    dut.opcode = 0b0010011;
    dut.funct3 = 0b000;

    dump(dut,trace,sim_time);

    check(1,dut.reg_write,"ADDI reg_write");
    check(1,dut.alu_src,"ADDI alu_src");


    // LW


    dut.opcode = 0b0000011;
    dut.funct3 = 0b010;

    dump(dut,trace,sim_time);

    check(1,dut.mem_read,"LW mem_read");
    check(1,dut.result_src,"LW result_src");
    check(1,dut.reg_write,"LW reg_write");
    check(1,dut.alu_src,"LW alu_src");
    check(2,dut.mem_size,"LW mem_size");
    check(0,dut.mem_unsigned,"LW mem_unsigned");


    // SW


    dut.opcode = 0b0100011;
    dut.funct3 = 0b010;

    dump(dut,trace,sim_time);

    check(1,dut.mem_write,"SW mem_write");
    check(0,dut.reg_write,"SW reg_write");
    check(1,dut.alu_src,"SW alu_src");
    check(2,dut.mem_size,"SW mem_size");


    // LB not implemented


    dut.opcode = 0b0000011;
    dut.funct3 = 0b000;

    dump(dut,trace,sim_time);

    check(0,dut.mem_read,"LB mem_read");
    check(0,dut.reg_write,"LB reg_write");


    // SB not implemented


    dut.opcode = 0b0100011;
    dut.funct3 = 0b000;

    dump(dut,trace,sim_time);

    check(0,dut.mem_write,"SB mem_write");


    // BEQ


    dut.opcode = 0b1100011;
    dut.funct3 = 0b000;

    dump(dut,trace,sim_time);

    check(1,dut.branch,"BEQ branch");
    check(0,dut.reg_write,"BEQ reg_write");



    trace->close();

    delete trace;

    std::cout
        << "\n========================\n";

    std::cout
        << "Passed : "
        << passed
        << std::endl;

    std::cout
        << "Failed : "
        << failed
        << std::endl;

    std::cout
        << "========================\n";

    return failed;
}
