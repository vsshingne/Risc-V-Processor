# RISC-V RV32I Processor Verification Strategy & Regression Framework

## 1. Overview

The verification environment validates the RISC-V processor using a two-tier methodology combining **directed assembly test cases** with **self-checking Verilator C++ testbenches**.

---

## 2. Test Suite Matrix

The project includes 20 comprehensive test suites:

| Suite Name | Targeted Instructions | Key Checks & Assertions |
| :--- | :--- | :--- |
| `arithmetic` | `ADD`, `SUB` | Register value updates, overflow/underflow, zero flag |
| `alu_rtype` | `SLL`, `SLT`, `SLTU`, `XOR`, `SRL`, `SRA`, `OR`, `AND` | Full R-type opcode & funct3/funct7 verification |
| `alu_itype` | `ADDI`, `SLTI`, `SLTIU`, `XORI`, `ORI`, `ANDI`, `SLLI`, `SRLI`, `SRAI` | Immediate sign-extension, shift masks |
| `lui_auipc` | `LUI`, `AUIPC` | Upper immediate placement, PC-relative calculations |
| `lw` | `LW` | Word load data retrieval & destination register update |
| `sw` | `SW` | Memory word store & readback verification |
| `lb` | `LB` | Byte load with 24-bit sign extension |
| `lbu` | `LBU` | Byte load with 24-bit zero extension |
| `lh` | `LH` | Halfword load with 16-bit sign extension & alignment check |
| `lhu` | `LHU` | Halfword load with 16-bit zero extension & alignment check |
| `sb` | `SB` | Single byte store; neighboring byte preservation check |
| `sh` | `SH` | Two-byte halfword store; neighboring halfword preservation |
| `beq` | `BEQ` | Equal branch taken vs not-taken PC update |
| `bne` | `BNE` | Not-equal branch taken vs not-taken PC update |
| `blt` | `BLT` | Signed less-than branch evaluation |
| `bge` | `BGE` | Signed greater-or-equal branch evaluation |
| `bltu` | `BLTU` | Unsigned less-than branch evaluation |
| `bgeu` | `BGEU` | Unsigned greater-or-equal branch evaluation |
| `jump` | `JAL` | Unconditional jump target calculation & link register (`PC+4`) |
| `jalr` | `JALR` | Indirect jump target calculation (`rs1 + imm & ~1`) & link register |

---

## 3. Representative Waveform Inspections

### ADD Instruction Execution Waveform
![ADD Waveform](/home/vsprime/riscv-soc/images/waveform_add.png)

### BEQ Branch Taken Execution Waveform
![Branch Waveform](/home/vsprime/riscv-soc/images/waveform_branch.png)

### LW Memory Load Execution Waveform
![LW Load Waveform](/home/vsprime/riscv-soc/images/waveform_lw.png)

### SW Memory Store Execution Waveform
![SW Store Waveform](/home/vsprime/riscv-soc/images/waveform_sw.png)

### JALR Jump and Link Register Waveform
![JALR Waveform](/home/vsprime/riscv-soc/images/waveform_jalr.png)

---

## 4. Automated Regression Execution

To execute the complete regression suite, run:

```bash
bash scripts/regression.sh
```

### Script Execution Flow:
1. Iterates over all 20 test targets.
2. Compiles assembly source `software/asm/<test>.S` using `riscv64-unknown-elf-gcc`.
3. Converts binary output to hex file `software/program.hex`.
4. Compiles the C++ testbench `tb/<test>/<test>_tb.cpp` with Verilator against the RTL top module `riscv_cpu`.
5. Executes the simulation model, validates internal signals against expected assertions, and prints pass/fail status.
6. Returns exit code `0` if all tests pass, or `1` if any test fails.
