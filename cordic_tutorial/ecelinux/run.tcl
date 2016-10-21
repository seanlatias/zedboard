#=============================================================================
# run.tcl 
#=============================================================================
# @brief: A Tcl script for synthesizing the digit recongnition design.

# Project name
set hls_prj cordic.prj

# Open/reset the project
open_project ${hls_prj} -reset

# Top function of the design is "dut"
set_top dut

# Add design and testbench files
add_files cordic.cpp
add_files -tb cordic_test.cpp

open_solution "solution1"
# Use Zynq device
set_part {xc7z020clg484-1}

# Target clock period is 10ns
create_clock -period 10

############################################

# Simulate the C++ design
csim_design
# Synthesize the design
csynth_design
# Co-simulate the design
#cosim_design
exit
