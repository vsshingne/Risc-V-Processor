# RV32I Single-Cycle Processor Core

A 32-bit single-cycle RISC-V CPU core supporting the complete RV32I Base Integer Instruction Set (40 instructions). Implemented in SystemVerilog and verified using a C++ Verilator test framework and GCC RV32I assembly test cases.

---

## Key Features

- **Standard RV32I ISA Compliance**: Implements all 40 base integer instructions including R-type, I-type (arithmetic, loads, jumps), S-type (stores), B-type (branches), and U-type (`LUI`, `AUIPC`).
- **Complete Memory Subsystem**:
  - Word (`LW`, `SW`), Halfword (`LH`, `LHU`, `SH`), and Byte (`LB`, `LBU`, `SB`) access support.
  - Sign-extension and zero-extension for loads.
  - Alignment checking and protection against unaligned halfword/word transfers.
- **Dedicated Branch & Jump Hardware**:
  - Independent `branch_comparator` evaluating signed (`BLT`, `BGE`) and unsigned (`BLTU`, `BGEU`, `BEQ`, `BNE`) comparisons without ALU resource contention.
  - Full support for `JAL` and `JALR` target calculation and link register writeback.
- **Single-Cycle Datapath**: Executes instructions in 1 clock cycle with synchronous active-high reset.
- **Clean Microarchitecture**: Decoupled control unit, immediate generator, instruction decoder, ALU, register file, and memory modules.
- **Industrial Verification Setup**: Verilator C++ simulation framework with VCD waveform tracing, automated assembly compilation pipeline, and full 20-test regression suite (`scripts/regression.sh`).

---

## Supported RV32I Instructions

| Instruction Type | Instructions Supported |
| :--- | :--- |
| **R-Type Arithmetic** | `ADD`, `SUB`, `SLL`, `SLT`, `SLTU`, `XOR`, `SRL`, `SRA`, `OR`, `AND` |
| **I-Type Arithmetic** | `ADDI`, `SLTI`, `SLTIU`, `XORI`, `ORI`, `ANDI`, `SLLI`, `SRLI`, `SRAI` |
| **I-Type Loads** | `LB`, `LH`, `LW`, `LBU`, `LHU` |
| **S-Type Stores** | `SB`, `SH`, `SW` |
| **B-Type Branches** | `BEQ`, `BNE`, `BLT`, `BGE`, `BLTU`, `BGEU` |
| **J-Type Jumps** | `JAL`, `JALR` |
| **U-Type Upper Immediates** | `LUI`, `AUIPC` |

---

## Architecture Overview

```text
               +-------------------------------------------------------+
               |                    System Reset / Clk                 |
               +---------------------------+---------------------------+
                                           |
                                           v
+------------------+         +----------------------------+
|  Program Counter |-------->|    Instruction Memory      |
+--------+---------+         +--------------+-------------+
         |                                  | Instruction (32-bit)
         |                                  v
         |                   +----------------------------+
         |                   |    Instruction Decoder     |
         |                   +--+-----+-----+------+------+
         |                      |     |     |      |
         |         +------------+     |     |      +----------------+
         |         | rs1/rs2/rd       |     | funct3/funct7/opcode   |
         |         v                  |     v                        v
         |  +--------------+          |  +---------------+    +--------------+
         |  | Register File|          |  | Immediate Gen |    | Control Unit |
         |  +---+------+---+          |  +-------+-------+    +------+-------+
         |      |      |              |          |                   |
         |      | rs1  | rs2          v          | imm               | Control Signals
         |      |      |         +----+---+      |                   | (alu_op, mem_size, etc.)
         |      |      +-------->| ALU MUX|<-----+                   v
         v      v                +----+---+                     +----+----+
      +--+------+--+                  | b                       | PC MUX  |<---+
      |  ALU A MUX |                  v                         +----+----+    |
      +-----+------+             +----+----+                         |         |
            | a                  |   ALU   |-------------------------+         |
            +------------------->+----+----+ alu_result                        |
                                      |                                        |
                   +------------------+--------------------+                   |
                   |                                       |                   |
                   v                                       v                   |
        +--------------------+                   +--------------------+        |
        |    Data Memory     |                   |  Branch Comparator |--------+
        +---------+----------+                   +--------------------+ equal, lt
                  | read_data
                  v
            +-----+-----+
            | Writeback |=====> Writeback Data -> Register File (rd)
            |    MUX    |
            +-----------+
```

---

## Directory Structure

```text
.
├── Makefile                # Master build, compilation, and execution script
├── README.md               # Project documentation and guide
├── LICENSE                 # MIT License
├── .gitignore              # Git ignore rules for build artifacts
├── docs/                   # Detailed architectural & verification docs
│   ├── architecture.md     # Hardware specification & datapath analysis
│   └── verification.md     # Verilator & assembly testing strategy
├── images/                 # Architecture diagrams and waveform captures
├── rtl/                    # SystemVerilog RTL Source Files
│   ├── alu/                # Arithmetic Logic Unit
│   ├── common/             # Package definitions (riscv_pkg.sv)
│   ├── comparator/         # Branch comparison logic
│   ├── control/            # Main Control Unit decoder
│   ├── core/               # Top-level RISC-V CPU wrapper (riscv_cpu.sv)
│   ├── decoder/            # Instruction bit field extractor
│   ├── immediate/          # Sign-extended immediate generator
│   ├── memory/             # Instruction memory & Data memory
│   ├── mux/                # Datapath multiplexers (ALU, WB)
│   ├── pc/                 # Program Counter register & PC MUX
│   └── register_file/      # 32x32-bit register file with debug interface
├── software/               # Assembly test programs & build artifacts
│   ├── asm/                # RISC-V assembly source files (.S)
│   └── linker.ld           # Memory map linker script
├── tb/                     # C++ Verilator testbenches
│   ├── addi/ ...           # Testbench suites per instruction group
│   └── lb/ lbu/ lh/ ...    # Load/store unit testbenches
└── scripts/
    ├── bin_to_hex.py       # Binary to hex converter for instruction RAM
    └── regression.sh       # Automated full regression runner script
```

---

## Quick Start & Build Instructions

### Prerequisites
Ensure the following tools are installed:
- **GNU Toolchain for RISC-V**: `riscv64-unknown-elf-gcc`, `riscv64-unknown-elf-objcopy`
- **Verilator**: Version 5.0+ (`verilator`)
- **Build Utilities**: `make`, `g++`, `python3`, `bash`

### Running an Individual Test
To compile an assembly test case, build the Verilator C++ model, and run the simulation with VCD trace generation:

```bash
# Run Load Byte (LB) test
make PROGRAM=lb TEST=lb run

# Run R-Type ALU test
make PROGRAM=alu_rtype TEST=alu_rtype run
```

### Running Full Regression Suite
To run all 20 instruction and feature test suites sequentially:

```bash
bash scripts/regression.sh
```

---

## Verification Methodology

Verification is performed at two complementary levels:
1. **Assembly Unit Tests (`software/asm/`)**: Hand-crafted RISC-V assembly routines targeting edge cases (signed vs unsigned arithmetic, sign-extension, byte/halfword masking, branch taken/not-taken, jump links).
2. **Self-Checking C++ Testbenches (`tb/`)**: Verilator testbenches driving cycle-by-cycle clock ticks, monitoring top-level CPU signals and debug ports (`debug_pc`, `debug_memory_read_data`, `debug_x1`..`debug_x4`), asserting strict correctness.

---

## License

This project is licensed under the [MIT License](LICENSE).
