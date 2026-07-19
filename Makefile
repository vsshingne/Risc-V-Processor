VERILATOR = verilator
CXX = g++

TOP = riscv_cpu

RTL = \
rtl/common/riscv_pkg.sv \
rtl/pc/program_counter.sv \
rtl/memory/instruction_memory.sv \
rtl/decoder/instruction_decoder.sv \
rtl/core/riscv_cpu.sv \
rtl/immediate/immediate_generator.sv\
rtl/control/control_unit.sv \
rtl/register_file/register_file.sv \
rtl/mux/alu_mux.sv \
rtl/alu/alu.sv \
rtl/memory/data_memory.sv \
rtl/mux/wb_mux.sv 

TB = tb/core/riscv_cpu_tb.cpp
all:
	$(VERILATOR) \
		--cc \
		--exe \
		--build \
		--trace\
		--sv \
		-Wall -Wno-fatal \
		--top-module $(TOP) \
		$(RTL) \
		$(TB)

run:
	./obj_dir/V$(TOP)

clean:
	rm -rf obj_dir *.vcd

.PHONY: all run clean