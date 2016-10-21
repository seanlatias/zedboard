#==========================================================================
# Makefile
#==========================================================================
# @brief: A makefile the compiles and runs the digitrec program
#
# @desc: 1. "make" or "make fpga" invokes the HW accelerator
#        2. "make fpga_batch" invokes the HW accelerator using batch mode
#        3. "make clean" cleans up the directory


INC_PATH=/usr/include/vivado_hls/2015.2
CFLAGS = -I${INC_PATH}

all: fpga

fpga: host.cpp
	@echo "Compiling host program & running CORDIC accelerator ..."
	g++ -I${INC_PATH} $< -o cordic-fpga
	./cordic-fpga

fpga_batch: host_batch.cpp
	@echo "Compiling host program (batch) & running CORDIC accelerator ..."
	g++ -I${INC_PATH} $< -o cordic-fpga-batch
	./cordic-fpga-batch

clean:
	@echo "Clean up output files"
	rm -f cordic-fpga cordic-fpga-batch  *.dat
