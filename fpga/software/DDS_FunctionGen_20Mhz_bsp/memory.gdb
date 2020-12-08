# memory.gdb - GDB memory region definitions
#
# Machine generated for CPU 'NIOS_CPU' in SOPC Builder design 'NiosII_Processor'
# SOPC Builder design path: ../../qsys/NiosII_Processor.sopcinfo
#
# Generated: Tue Dec 08 15:15:54 CET 2020

# DO NOT MODIFY THIS FILE
#
# Changing this file will have subtle consequences
# which will almost certainly lead to a nonfunctioning
# system. If you do modify this file, be aware that your
# changes will be overwritten and lost when this file
# is generated again.
#
# DO NOT MODIFY THIS FILE

# License Agreement
#
# Copyright (c) 2008
# Altera Corporation, San Jose, California, USA.
# All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#
# This agreement shall be governed in all respects by the laws of the State
# of California and by the laws of the United States of America.

# Define memory regions for each memory connected to the CPU.
# The cache attribute is specified which improves GDB performance
# by allowing GDB to cache memory contents on the host.

# RAM_24K
memory 0x0 0x5dc0 cache

# LOOKUP_RAM_RAM_DDS0_AM
memory 0x9000 0x9800 cache

# LOOKUP_RAM_RAM_DDS0_FM
memory 0x9800 0xa000 cache

# LOOKUP_RAM_RAM_DDS0
memory 0xa000 0xa800 cache

# LOOKUP_RAM_RAM_DDS1_AM
memory 0xa800 0xb000 cache

# LOOKUP_RAM_RAM_DDS1_FM
memory 0xb000 0xb800 cache

# LOOKUP_RAM_RAM_DDS1
memory 0xb800 0xc000 cache

# FLASH_data
memory 0x40000 0x6b000 cache
