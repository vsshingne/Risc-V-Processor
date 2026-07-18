VERILATOR = verilator
CXX = g++

TOP = data_memory

RTL = rtl/memory/data_memory.sv

TB = tb/memory/data_memory_tb.cpp
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