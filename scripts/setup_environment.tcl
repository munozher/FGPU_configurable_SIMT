##############################################################################
#
# setup_environment.tcl
#
# Description: Root script that sets up environment variables and target.
#
# Author: Hector Gerardo Munoz Hernandez <hector.munozhernandez@b-tu.de>
# Contributors:
#   - Marcelo Brandalero <marcelo.brandalero@b-tu.de>
#   - Mitko Veleski <mitko.veleski@b-tu.de>
# 
# Institution: Brandenburg University of Technology Cottbus-Senftenberg (B-TU)
# Date Created: 07.04.2020
#
# Tested Under:
#   - Vivado 2017.2
#
##############################################################################


##############################################################################
########################## BEGIN DON'T TOUCH #################################
##############################################################################

# Guard clause to prevent this script from running twice
if ([info exists set_up_fgpu_environment]) {
	puts "File [file tail [info script]] has already been sourced."
	puts "To enable re-sourcing, run \"unset set_up_fgpu_environment\"."
	return
}

# These two lines must come on top and are used to determine the absolute 
# path where these scripts and the repository are located.
set path_tclscripts [file normalize "[info script]/../"]
set path_repository [file normalize "${path_tclscripts}/../../"]
cd "${path_repository}/project_vivado"

##############################################################################
############################ END DON'T TOUCH #################################
##############################################################################


##############################################################################
########### Modify the variables below according to your setup ###############
##############################################################################

# Choose one
set OS "linux"
#set OS "windows"

################################################################################
######                  Do not edit the fgpu_ip name                       #####
######You may only change the name of the project inside the else statement#####
################################################################################
if {${action} == "generate_IP"} {
    set name_project "fgpu_ip_temp"
} else {
    set name_project "fgpu_sys"
}


set path_project "${path_repository}/project_vivado/${name_project}"


# The number of threads with which to run simulation, synthesis and impl.
set num_threads 4

# Set the target board
set target_board "Ultra96"

# The target frequency for implementation
set FREQ        100

##############################################################################
### These variables below will likely be impacted by the version of Vivado ###
##############################################################################

# IP Versions
set ip_clk_wiz_v "6.0"


##############################################################################
###########                  FGPU IP Parameters               ################
##############################################################################

set FGPU_IP_NAME "FGPU"
set FGPU_IP_VERSION "2.1"
set FGPU_IP_DISPLAY_NAME "FGPU_v2_1"
set FGPU_IP_DESCRIPTION "FGPU version 2.1."

##############################################################################
########### Variables below "should" not require modification ################
##############################################################################

set name_bd     "FGPU_bd"

set path_rtl "${path_repository}/RTL/db"
	
##############################################################################
############################ Commands ########################################
##############################################################################

source ${path_tclscripts}/targets/${target_board}/setup.tcl

# Changes to the project directory if it already exists
if {[file exists ${path_project}] != 0} {
	cd ${path_project}
}

set set_up_fgpu_environment true

