# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: /home/hector/Documents/demo/FGPU/vivado/xilinx_contest/FGPU_configurable_SIMT/vitis_proj/cnn_system/_ide/scripts/debugger_cnn-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source /home/hector/Documents/demo/FGPU/vivado/xilinx_contest/FGPU_configurable_SIMT/vitis_proj/cnn_system/_ide/scripts/debugger_cnn-default.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent Zed 210248783527" && level==0 && jtag_device_ctx=="jsn-Zed-210248783527-23727093-0"}
fpga -file /home/hector/Documents/demo/FGPU/vivado/xilinx_contest/FGPU_configurable_SIMT/vitis_proj/cnn/_ide/bitstream/FGPU_bd_wrapper.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw /home/hector/Documents/demo/FGPU/vivado/xilinx_contest/FGPU_configurable_SIMT/vitis_proj/zedboard_custom/export/zedboard_custom/hw/FGPU_bd_wrapper.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source /home/hector/Documents/demo/FGPU/vivado/xilinx_contest/FGPU_configurable_SIMT/vitis_proj/cnn/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow /home/hector/Documents/demo/FGPU/vivado/xilinx_contest/FGPU_configurable_SIMT/vitis_proj/cnn/Debug/cnn.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A9*#0"}
con
