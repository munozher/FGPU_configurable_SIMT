# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/hector/Documents/demo/FGPU/vivado/xilinx_contest/FGPU_configurable_SIMT/vitis_proj/zedboard_custom/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/hector/Documents/demo/FGPU/vivado/xilinx_contest/FGPU_configurable_SIMT/vitis_proj/zedboard_custom/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {zedboard_custom}\
-hw {/home/hector/Documents/demo/FGPU/vivado/xilinx_contest/FGPU_configurable_SIMT/fgpu_sys/FGPU_bd_wrapper.xsa}\
-proc {ps7_cortexa9_0} -os {standalone} -fsbl-target {psu_cortexa53_0} -out {/home/hector/Documents/demo/FGPU/vivado/xilinx_contest/FGPU_configurable_SIMT/vitis_proj}

platform write
platform generate -domains 
platform active {zedboard_custom}
platform generate
