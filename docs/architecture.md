# RISC-V RV32I Processor Architecture Specification

## 1. Overview

The processor is a 32-bit single-cycle RISC-V core executing the RV32I instruction set architecture. It is designed around a decoupled, modular microarchitecture where every instruction completes execution and writeback in exactly one clock cycle.

---

## 2. Processor Architecture Diagram

![RISC-V CPU Block Diagram](/home/vsprime/riscv-soc/images/cpu_block_diagram.png)

---

## 3. Top-Level Core Subsystems

### Module Descriptions:

1. **`program_counter` (`rtl/pc/program_counter.sv`)**:
   32-bit register holding the current Instruction Address. Updated on every `posedge clk` when `pc_write` is active, or initialized to `0x00000000` on synchronous `reset`.

2. **`instruction_memory` (`rtl/memory/instruction_memory.sv`)**:
   256 x 32-bit ROM loaded with binary program machine code from `software/program.hex`. Asynchronously outputs 32-bit `instruction` based on word address `pc[9:2]`.

3. **`instruction_decoder` (`rtl/decoder/instruction_decoder.sv`)**:
   Extracts instruction bitfields: `opcode` (`[6:0]`), `rd` (`[11:7]`), `funct3` (`[14:12]`), `rs1` (`[19:15]`), `rs2` (`[24:20]`), `funct7` (`[31:25]`).

4. **`register_file` (`rtl/register_file/register_file.sv`)**:
   32 x 32-bit general-purpose register array. Reads on `rs1` and `rs2` are asynchronous (combinational). Writes to `rd` are synchronous on `posedge clk` when `we` is high and `rd != 0`. Register `x0` is hardwired to zero.

5. **`immediate_generator` (`rtl/immediate/immediate_generator.sv`)**:
   Generates sign-extended 32-bit immediates for all standard RISC-V types:
   - **I-Type**: `{ {20{inst[31]}}, inst[31:20] }`
   - **S-Type**: `{ {20{inst[31]}}, inst[31:25], inst[11:7] }`
   - **B-Type**: `{ {19{inst[31]}}, inst[31], inst[7], inst[30:25], inst[11:8], 1'b0 }`
   - **U-Type**: `{ inst[31:12], 12'b0 }`
   - **J-Type**: `{ {11{inst[31]}}, inst[31], inst[19:12], inst[20], inst[30:21], 1'b0 }`

6. **`control_unit` (`rtl/control/control_unit.sv`)**:
   Decodes `opcode`, `funct3`, and `funct7` into control signals governing ALU operation, immediate selection, memory size/extension, writeback source, and branch/jump routing.

7. **`alu` (`rtl/alu/alu.sv`)**:
   32-bit Arithmetic Logic Unit performing:
   `ADD`, `SUB`, `AND`, `OR`, `XOR`, `SLL`, `SRL`, `SRA` (arithmetic right shift), `SLT` (signed comparison), `SLTU` (unsigned comparison).

8. **`branch_comparator` (`rtl/comparator/branch_comparator.sv`)**:
   Dedicated comparator driving `equal`, `lt_signed`, and `lt_unsigned` flags directly from `rs1_data` and `rs2_data`.

9. **`data_memory` (`rtl/memory/data_memory.sv`)**:
   1 KB byte-addressable SRAM (`logic [7:0] memory [0:1023]`). Supports byte (`MEM_BYTE`), halfword (`MEM_HALF`), and word (`MEM_WORD`) transfers. Computes alignment status `address_aligned` and performs sign or zero extension based on `mem_unsigned`.

---

## 4. Control Matrix & Signal Decoding

| Instruction | Opcode | Funct3 | Funct7 | `reg_write` | `alu_src` | `alu_op` | `mem_read` | `mem_write` | `mem_size` | `result_src` |
| :--- | :--- | :--- | :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **ADD** | `0110011` | `000` | `0000000` | 1 | 0 | `ALU_ADD` | 0 | 0 | - | 0 |
| **SUB** | `0110011` | `000` | `0100000` | 1 | 0 | `ALU_SUB` | 0 | 0 | - | 0 |
| **ADDI** | `0010011` | `000` | - | 1 | 1 | `ALU_ADD` | 0 | 0 | - | 0 |
| **LW** | `0000011` | `010` | - | 1 | 1 | `ALU_ADD` | 1 | 0 | `MEM_WORD` | 1 |
| **LB** | `0000011` | `000` | - | 1 | 1 | `ALU_ADD` | 1 | 0 | `MEM_BYTE` | 1 |
| **LH** | `0000011` | `001` | - | 1 | 1 | `ALU_ADD` | 1 | 0 | `MEM_HALF` | 1 |
| **SW** | `0100011` | `010` | - | 0 | 1 | `ALU_ADD` | 0 | 1 | `MEM_WORD` | - |
| **SB** | `0100011` | `000` | - | 0 | 1 | `ALU_ADD` | 0 | 1 | `MEM_BYTE` | - |
| **SH** | `0100011` | `001` | - | 0 | 1 | `ALU_ADD` | 0 | 1 | `MEM_HALF` | - |
| **BEQ** | `1100011` | `000` | - | 0 | 0 | `ALU_SUB` | 0 | 0 | - | - |
| **JAL** | `1101111` | - | - | 1 | - | `ALU_ADD` | 0 | 0 | - | 0 (`PC+4`) |
| **JALR** | `1100111` | `000` | - | 1 | 1 | `ALU_ADD` | 0 | 0 | - | 0 (`PC+4`) |
