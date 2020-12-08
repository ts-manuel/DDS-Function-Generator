#!/bin/sh
#
# This file was automatically generated.
#
# It can be overwritten by nios2-flash-programmer-generate or nios2-flash-programmer-gui.
#

#
# Converting Binary File: E:\QuartusPrime\Progetti\DDS_FunctionGenerator_20Mhz\DDS_FunctionGenerator_20Mhz\software\DDS_FunctionGen_20Mhz\mem_init\NiosII_Processor_FLASH.hex to: "..\flash/NiosII_Processor_FLASH_FLASH_data.flash"
#
bin2flash --input="E:/QuartusPrime/Progetti/DDS_FunctionGenerator_20Mhz/DDS_FunctionGenerator_20Mhz/software/DDS_FunctionGen_20Mhz/mem_init/NiosII_Processor_FLASH.hex" --output="../flash/NiosII_Processor_FLASH_FLASH_data.flash" --location=0x0 --verbose 

#
# Programming File: "..\flash/NiosII_Processor_FLASH_FLASH_data.flash" To Device: FLASH_data
#
nios2-flash-programmer "../flash/NiosII_Processor_FLASH_FLASH_data.flash" --base=0x40000 --sidp=0x89A18 --id=0x2 --timestamp=1518260864 --device=1 --instance=0 '--cable=USB-Blaster on localhost [USB-0]' --program --verbose 

