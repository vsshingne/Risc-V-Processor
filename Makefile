VERILATOR = verilator
CXX = g++

TOP = program_counter

RTL = rtl/pc/program_counter.sv

TB = tb/pc/program_counter_tb.cpp

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