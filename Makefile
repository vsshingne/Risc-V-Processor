VERILATOR = verilator
CXX = g++

TOP = instruction_memory

RTL = rtl/memory/instruction_memory.sv

TB = tb/memory/instruction_memory_tb.cpp

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