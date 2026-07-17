VERILATOR = verilator
CXX = g++

TOP = instruction_fetch

RTL = \
rtl/pc/program_counter.sv \
rtl/memory/instruction_memory.sv \
rtl/ifu/instruction_fetch.sv

TB = tb/ifu/instruction_fetch_tb.cpp
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