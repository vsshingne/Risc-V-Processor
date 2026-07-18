VERILATOR = verilator
CXX = g++

TOP = immediate_generator

RTL = rtl/immediate/immediate_generator.sv

TB = tb/immediate/immediate_generator_tb.cpp
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