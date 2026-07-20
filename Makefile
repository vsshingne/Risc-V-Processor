  
# RISC-V Single Cycle CPU Makefile
  

  
# Program Selection
  

PROGRAM ?= arithmetic

  
# Directories
  

RTL_DIR      := rtl
TB_DIR       := tb/core
BUILD_DIR    := obj_dir
WAVE_DIR     := waveforms

SOFT_DIR     := software
ASM_DIR      := $(SOFT_DIR)/asm
BUILD_SOFT   := $(SOFT_DIR)/build

SCRIPT_DIR   := scripts

  
# Software Files
  

ASM  := $(ASM_DIR)/$(PROGRAM).S
ELF  := $(BUILD_SOFT)/$(PROGRAM).elf
BIN  := $(BUILD_SOFT)/$(PROGRAM).bin
LST  := $(BUILD_SOFT)/$(PROGRAM).lst

HEX  := $(SOFT_DIR)/program.hex

LINKER := $(SOFT_DIR)/linker.ld

  
# Verilator
  

VERILATOR = verilator

TOP = riscv_cpu

CPP = $(TB_DIR)/riscv_cpu_tb_execution.cpp

RTL = \
rtl/common/riscv_pkg.sv \
rtl/pc/program_counter.sv \
rtl/memory/instruction_memory.sv \
rtl/decoder/instruction_decoder.sv \
rtl/control/control_unit.sv \
rtl/immediate/immediate_generator.sv \
rtl/register_file/register_file.sv \
rtl/mux/alu_mux.sv \
rtl/alu/alu.sv \
rtl/memory/data_memory.sv \
rtl/mux/wb_mux.sv \
rtl/core/riscv_cpu.sv

  
# Default Target
  

all: $(ELF)

  
# Software Build
  

$(ELF): $(ASM) $(LINKER)
	@mkdir -p $(BUILD_SOFT)

	riscv64-unknown-elf-gcc \
	-march=rv32i \
	-mabi=ilp32 \
	-nostdlib \
	-T $(LINKER) \
	$(ASM) \
	-o $(ELF)

$(BIN): $(ELF)

	riscv64-unknown-elf-objcopy \
	-O binary \
	$(ELF) \
	$(BIN)

$(LST): $(ELF)

	riscv64-unknown-elf-objdump -d $(ELF) > $(LST)

$(HEX): $(BIN)

	python3 $(SCRIPT_DIR)/bin_to_hex.py \
	$(BIN) \
	$(HEX)

  
# Verilator Build
  

compile: $(HEX)

	$(VERILATOR) \
	--cc \
	--exe \
	--trace \
	--build \
	--top-module $(TOP) \
	$(RTL) \
	$(CPP)

  
# Run Simulation
  

run: $(HEX) compile

	./obj_dir/V$(TOP)

  
# Utility Targets
  

disasm: $(LST)

	@cat $(LST)

hex: $(HEX)

	@cat $(HEX)

  
# Clean
  

clean:

	rm -rf obj_dir
	rm -rf $(BUILD_SOFT)/*
	rm -f $(HEX)
	rm -f $(WAVE_DIR)/*.vcd

  
# Help
  

help:

	@echo ""
	@echo "Usage:"
	@echo ""
	@echo "make run"
	@echo "make PROGRAM=arithmetic run"
	@echo "make PROGRAM=lw run"
	@echo "make PROGRAM=branch run"
	@echo ""
	@echo "make disasm PROGRAM=arithmetic"
	@echo "make hex PROGRAM=arithmetic"
	@echo "make clean"
	@echo ""