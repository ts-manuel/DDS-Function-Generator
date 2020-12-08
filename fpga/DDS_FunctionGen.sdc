## Generated SDC file "DDS_FunctionGen.out.sdc"

## Copyright (C) 2017  Intel Corporation. All rights reserved.
## Your use of Intel Corporation's design tools, logic functions 
## and other software and tools, and its AMPP partner logic 
## functions, and any output files from any of the foregoing 
## (including device programming or simulation files), and any 
## associated documentation or information are expressly subject 
## to the terms and conditions of the Intel Program License 
## Subscription Agreement, the Intel Quartus Prime License Agreement,
## the Intel FPGA IP License Agreement, or other applicable license
## agreement, including, without limitation, that your use is for
## the sole purpose of programming logic devices manufactured by
## Intel and sold by Intel or its authorized distributors.  Please
## refer to the applicable agreement for further details.


## VENDOR  "Altera"
## PROGRAM "Quartus Prime"
## VERSION "Version 17.1.0 Build 590 10/25/2017 SJ Lite Edition"

## DATE    "Tue Feb 27 11:13:19 2018"

##
## DEVICE  "10M08SAE144I7G"
##


#**************************************************************
# Time Information
#**************************************************************

set_time_format -unit ns -decimal_places 3



#**************************************************************
# Create Clock
#**************************************************************
create_clock -name i_clk_10Mhz -period 100.000 -waveform { 0.000 50.000 } [get_pins {i_clk_10Mhz~input|i i_clk_10Mhz~input|o}]
create_clock -name onchip_flash_clock -period 20.000 [get_registers {NiosII_Processor:CPU|altera_onchip_flash:flash|altera_onchip_flash_avmm_data_controller:avmm_data_controller|flash_se_neg_reg}]

#**************************************************************
# Create Generated Clock
#**************************************************************
#derive_pll_clocks
create_generated_clock -source {PLL0|altpll_component|auto_generated|pll1|inclk[0]} -multiply_by 20 -duty_cycle 50.00 -name {PLL0|altpll_component|auto_generated|pll1|clk[0]} {PLL0|altpll_component|auto_generated|pll1|clk[0]}
create_generated_clock -source {PLL0|altpll_component|auto_generated|pll1|inclk[0]} -multiply_by 5 -duty_cycle 50.00 -name {PLL0|altpll_component|auto_generated|pll1|clk[1]} {PLL0|altpll_component|auto_generated|pll1|clk[1]}
create_generated_clock -source {PLL0|altpll_component|auto_generated|pll1|inclk[0]} -divide_by 5000 -duty_cycle 50.00 -name {PLL0|altpll_component|auto_generated|pll1|clk[2]} {PLL0|altpll_component|auto_generated|pll1|clk[2]}



#**************************************************************
# Set Clock Latency
#**************************************************************



#**************************************************************
# Set Clock Uncertainty
#**************************************************************
derive_clock_uncertainty


#**************************************************************
# Set Input Delay
#**************************************************************



#**************************************************************
# Set Output Delay
#**************************************************************
set_output_delay -clock { PLL0|altpll_component|auto_generated|pll1|clk[0] } 40 [get_ports {o_DDS0_DAC[0] o_DDS0_DAC[1] o_DDS0_DAC[2] o_DDS0_DAC[3] o_DDS0_DAC[4] o_DDS0_DAC[5] o_DDS0_DAC[6] o_DDS0_DAC[7] o_DDS0_DAC[8] o_DDS0_DAC[9] o_DDS0_DAC[10] o_DDS0_DAC[11] o_DDS1_DAC[0] o_DDS1_DAC[1] o_DDS1_DAC[2] o_DDS1_DAC[3] o_DDS1_DAC[4] o_DDS1_DAC[5] o_DDS1_DAC[6] o_DDS1_DAC[7] o_DDS1_DAC[8] o_DDS1_DAC[9] o_DDS1_DAC[10] o_DDS1_DAC[11]}]


#**************************************************************
# Set Clock Groups
#**************************************************************

set_clock_groups -asynchronous -group [get_clocks {altera_reserved_tck}] 
set_clock_groups -asynchronous -group [get_clocks {altera_reserved_tck}] 


#**************************************************************
# Set False Path
#**************************************************************

set_false_path -from [get_registers {*|alt_jtag_atlantic:*|jupdate}] -to [get_registers {*|alt_jtag_atlantic:*|jupdate1*}]
set_false_path -from [get_registers {*|alt_jtag_atlantic:*|rdata[*]}] -to [get_registers {*|alt_jtag_atlantic*|td_shift[*]}]
set_false_path -from [get_registers {*|alt_jtag_atlantic:*|read}] -to [get_registers {*|alt_jtag_atlantic:*|read1*}]
set_false_path -from [get_registers {*|alt_jtag_atlantic:*|read_req}] 
set_false_path -from [get_registers {*|alt_jtag_atlantic:*|rvalid}] -to [get_registers {*|alt_jtag_atlantic*|td_shift[*]}]
set_false_path -from [get_registers {*|t_dav}] -to [get_registers {*|alt_jtag_atlantic:*|tck_t_dav}]
set_false_path -from [get_registers {*|alt_jtag_atlantic:*|user_saw_rvalid}] -to [get_registers {*|alt_jtag_atlantic:*|rvalid0*}]
set_false_path -from [get_registers {*|alt_jtag_atlantic:*|wdata[*]}] -to [get_registers *]
set_false_path -from [get_registers {*|alt_jtag_atlantic:*|write}] -to [get_registers {*|alt_jtag_atlantic:*|write1*}]
set_false_path -from [get_registers {*|alt_jtag_atlantic:*|write_stalled}] -to [get_registers {*|alt_jtag_atlantic:*|t_ena*}]
set_false_path -from [get_registers {*|alt_jtag_atlantic:*|write_stalled}] -to [get_registers {*|alt_jtag_atlantic:*|t_pause*}]
set_false_path -from [get_registers {*|alt_jtag_atlantic:*|write_valid}] 
set_false_path -to [get_keepers {*altera_std_synchronizer:*|din_s1}]
set_false_path -to [get_pins -nocase -compatibility_mode {*|alt_rst_sync_uq1|altera_reset_synchronizer_int_chain*|clrn}]
set_false_path -from [get_keepers {*NiosII_Processor_NIOS_CPU_cpu:*|NiosII_Processor_NIOS_CPU_cpu_nios2_oci:the_NiosII_Processor_NIOS_CPU_cpu_nios2_oci|NiosII_Processor_NIOS_CPU_cpu_nios2_oci_break:the_NiosII_Processor_NIOS_CPU_cpu_nios2_oci_break|break_readreg*}] -to [get_keepers {*NiosII_Processor_NIOS_CPU_cpu:*|NiosII_Processor_NIOS_CPU_cpu_nios2_oci:the_NiosII_Processor_NIOS_CPU_cpu_nios2_oci|NiosII_Processor_NIOS_CPU_cpu_debug_slave_wrapper:the_NiosII_Processor_NIOS_CPU_cpu_debug_slave_wrapper|NiosII_Processor_NIOS_CPU_cpu_debug_slave_tck:the_NiosII_Processor_NIOS_CPU_cpu_debug_slave_tck|*sr*}]
set_false_path -from [get_keepers {*NiosII_Processor_NIOS_CPU_cpu:*|NiosII_Processor_NIOS_CPU_cpu_nios2_oci:the_NiosII_Processor_NIOS_CPU_cpu_nios2_oci|NiosII_Processor_NIOS_CPU_cpu_nios2_oci_debug:the_NiosII_Processor_NIOS_CPU_cpu_nios2_oci_debug|*resetlatch}] -to [get_keepers {*NiosII_Processor_NIOS_CPU_cpu:*|NiosII_Processor_NIOS_CPU_cpu_nios2_oci:the_NiosII_Processor_NIOS_CPU_cpu_nios2_oci|NiosII_Processor_NIOS_CPU_cpu_debug_slave_wrapper:the_NiosII_Processor_NIOS_CPU_cpu_debug_slave_wrapper|NiosII_Processor_NIOS_CPU_cpu_debug_slave_tck:the_NiosII_Processor_NIOS_CPU_cpu_debug_slave_tck|*sr[33]}]
set_false_path -from [get_keepers {*NiosII_Processor_NIOS_CPU_cpu:*|NiosII_Processor_NIOS_CPU_cpu_nios2_oci:the_NiosII_Processor_NIOS_CPU_cpu_nios2_oci|NiosII_Processor_NIOS_CPU_cpu_nios2_oci_debug:the_NiosII_Processor_NIOS_CPU_cpu_nios2_oci_debug|monitor_ready}] -to [get_keepers {*NiosII_Processor_NIOS_CPU_cpu:*|NiosII_Processor_NIOS_CPU_cpu_nios2_oci:the_NiosII_Processor_NIOS_CPU_cpu_nios2_oci|NiosII_Processor_NIOS_CPU_cpu_debug_slave_wrapper:the_NiosII_Processor_NIOS_CPU_cpu_debug_slave_wrapper|NiosII_Processor_NIOS_CPU_cpu_debug_slave_tck:the_NiosII_Processor_NIOS_CPU_cpu_debug_slave_tck|*sr[0]}]
set_false_path -from [get_keepers {*NiosII_Processor_NIOS_CPU_cpu:*|NiosII_Processor_NIOS_CPU_cpu_nios2_oci:the_NiosII_Processor_NIOS_CPU_cpu_nios2_oci|NiosII_Processor_NIOS_CPU_cpu_nios2_oci_debug:the_NiosII_Processor_NIOS_CPU_cpu_nios2_oci_debug|monitor_error}] -to [get_keepers {*NiosII_Processor_NIOS_CPU_cpu:*|NiosII_Processor_NIOS_CPU_cpu_nios2_oci:the_NiosII_Processor_NIOS_CPU_cpu_nios2_oci|NiosII_Processor_NIOS_CPU_cpu_debug_slave_wrapper:the_NiosII_Processor_NIOS_CPU_cpu_debug_slave_wrapper|NiosII_Processor_NIOS_CPU_cpu_debug_slave_tck:the_NiosII_Processor_NIOS_CPU_cpu_debug_slave_tck|*sr[34]}]
set_false_path -from [get_keepers {*NiosII_Processor_NIOS_CPU_cpu:*|NiosII_Processor_NIOS_CPU_cpu_nios2_oci:the_NiosII_Processor_NIOS_CPU_cpu_nios2_oci|NiosII_Processor_NIOS_CPU_cpu_nios2_ocimem:the_NiosII_Processor_NIOS_CPU_cpu_nios2_ocimem|*MonDReg*}] -to [get_keepers {*NiosII_Processor_NIOS_CPU_cpu:*|NiosII_Processor_NIOS_CPU_cpu_nios2_oci:the_NiosII_Processor_NIOS_CPU_cpu_nios2_oci|NiosII_Processor_NIOS_CPU_cpu_debug_slave_wrapper:the_NiosII_Processor_NIOS_CPU_cpu_debug_slave_wrapper|NiosII_Processor_NIOS_CPU_cpu_debug_slave_tck:the_NiosII_Processor_NIOS_CPU_cpu_debug_slave_tck|*sr*}]
set_false_path -from [get_keepers {*NiosII_Processor_NIOS_CPU_cpu:*|NiosII_Processor_NIOS_CPU_cpu_nios2_oci:the_NiosII_Processor_NIOS_CPU_cpu_nios2_oci|NiosII_Processor_NIOS_CPU_cpu_debug_slave_wrapper:the_NiosII_Processor_NIOS_CPU_cpu_debug_slave_wrapper|NiosII_Processor_NIOS_CPU_cpu_debug_slave_tck:the_NiosII_Processor_NIOS_CPU_cpu_debug_slave_tck|*sr*}] -to [get_keepers {*NiosII_Processor_NIOS_CPU_cpu:*|NiosII_Processor_NIOS_CPU_cpu_nios2_oci:the_NiosII_Processor_NIOS_CPU_cpu_nios2_oci|NiosII_Processor_NIOS_CPU_cpu_debug_slave_wrapper:the_NiosII_Processor_NIOS_CPU_cpu_debug_slave_wrapper|NiosII_Processor_NIOS_CPU_cpu_debug_slave_sysclk:the_NiosII_Processor_NIOS_CPU_cpu_debug_slave_sysclk|*jdo*}]
set_false_path -from [get_keepers {sld_hub:*|irf_reg*}] -to [get_keepers {*NiosII_Processor_NIOS_CPU_cpu:*|NiosII_Processor_NIOS_CPU_cpu_nios2_oci:the_NiosII_Processor_NIOS_CPU_cpu_nios2_oci|NiosII_Processor_NIOS_CPU_cpu_debug_slave_wrapper:the_NiosII_Processor_NIOS_CPU_cpu_debug_slave_wrapper|NiosII_Processor_NIOS_CPU_cpu_debug_slave_sysclk:the_NiosII_Processor_NIOS_CPU_cpu_debug_slave_sysclk|ir*}]
set_false_path -from [get_keepers {sld_hub:*|sld_shadow_jsm:shadow_jsm|state[1]}] -to [get_keepers {*NiosII_Processor_NIOS_CPU_cpu:*|NiosII_Processor_NIOS_CPU_cpu_nios2_oci:the_NiosII_Processor_NIOS_CPU_cpu_nios2_oci|NiosII_Processor_NIOS_CPU_cpu_nios2_oci_debug:the_NiosII_Processor_NIOS_CPU_cpu_nios2_oci_debug|monitor_go}]
set_false_path -to [get_registers {*|flash_busy_reg}]
set_false_path -to [get_registers {*|flash_busy_clear_reg}]


#**************************************************************
# Set Multicycle Path
#**************************************************************



#**************************************************************
# Set Maximum Delay
#**************************************************************



#**************************************************************
# Set Minimum Delay
#**************************************************************



#**************************************************************
# Set Input Transition
#**************************************************************

