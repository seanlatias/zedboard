#=============================================================================
# run_unroll.tcl 
#=============================================================================
# @brief: A Tcl script for synthesizing the unrolled digit recongnition design.

# Project name
set hls_prj digitrec.prj

# Open/reset the project
open_project ${hls_prj} -reset

# Top function of the design is "dut"
set_top dut

# Add design and testbench files
add_files digitrec.cpp
add_files -tb digitrec_test.cpp
add_files -tb data

open_solution "solution1"
# Use Zynq device
set_part {xc7z020clg484-1}

# Target clock period is 10ns
create_clock -period 10

### You can insert your own directives here ###
set_directive_inline -off update_knn
set_directive_array_partition -type complete -dim 1 digitrec knn_set
set_directive_unroll digitrec/L10

############################################

# Simulate the C++ design
#csim_design
# Synthesize the design
csynth_design
# Co-simulate the design
#cosim_design
exit
