# FGPU_configurable_SIMT
FGPU: A Configurable Soft-Core SIMT Accelerator (Xilinx open Hardware 2021 design contest)

Tested for Vivado 2020.2, and using a ZedBoard.
Other Vivado versions might not work.

The repository is organized as follows:

bitstream - It has a previously generated bitstream for the Zedboard

docs - The written report can be found here

fgpu_ip - Contains the IP files of the FGPU that can be imported into a Vivado block diagram

scripts - It has scripts that can auto-generate the bitstream of the project for Vivado.

vitis_proj - Contains the vitis project that showcases a CNN application.


## Getting started:

1. (Optional since a pre-generated version of the bitstream and the xsa hardware platform can be found in the bitstream folder) Open Vivado and run the main_implement.tcl script found in the scripts folder.
2. After Vivado is ready generating the bitstream, export the hardware (including the bitstream).
3. Open the vitis project provided in the vitis_proj folder, 

Optionally, you can start from scratch by:

4. creating a platform project (selecting the xsa from the bitstreams folder or from step 1)
5. Create an empty (C++) application project, go into lscript.ld and change the heap size to 0x8000000, and import all the .cpp, and .h sources from vitis_proj/cnn/src.
6. Build the project, connect a Zedboard and run it.
7. If you want to experiment with different handwritten digits, go into image.h (inside the sources) and comment / uncomment the desired value. 
Optionally, the user can use their own handwritten digit image, provided that its 28x28 pixels and its flatten into a one-dimesional 'c' array (usage of image.h as template is encouraged)
8. The result can be visualized in a serial terminal.

A video demonstration can be seen [here:][https://youtube.com]
