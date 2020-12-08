/*
 * linker.x - Linker script
 *
 * Machine generated for CPU 'NIOS_CPU' in SOPC Builder design 'NiosII_Processor'
 * SOPC Builder design path: ../../qsys/NiosII_Processor.sopcinfo
 *
 * Generated: Tue Dec 08 15:15:54 CET 2020
 */

/*
 * DO NOT MODIFY THIS FILE
 *
 * Changing this file will have subtle consequences
 * which will almost certainly lead to a nonfunctioning
 * system. If you do modify this file, be aware that your
 * changes will be overwritten and lost when this file
 * is generated again.
 *
 * DO NOT MODIFY THIS FILE
 */

/*
 * License Agreement
 *
 * Copyright (c) 2008
 * Altera Corporation, San Jose, California, USA.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * This agreement shall be governed in all respects by the laws of the State
 * of California and by the laws of the United States of America.
 */

MEMORY
{
    RAM_24K : ORIGIN = 0x0, LENGTH = 24000
    LOOKUP_RAM_RAM_DDS0_AM : ORIGIN = 0x9000, LENGTH = 2048
    LOOKUP_RAM_RAM_DDS0_FM : ORIGIN = 0x9800, LENGTH = 2048
    LOOKUP_RAM_RAM_DDS0 : ORIGIN = 0xa000, LENGTH = 2048
    LOOKUP_RAM_RAM_DDS1_AM : ORIGIN = 0xa800, LENGTH = 2048
    LOOKUP_RAM_RAM_DDS1_FM : ORIGIN = 0xb000, LENGTH = 2048
    LOOKUP_RAM_RAM_DDS1 : ORIGIN = 0xb800, LENGTH = 2048
    reset : ORIGIN = 0x40000, LENGTH = 32
    FLASH_data : ORIGIN = 0x40020, LENGTH = 176096
}

/* Define symbols for each memory base-address */
__alt_mem_RAM_24K = 0x0;
__alt_mem_LOOKUP_RAM_RAM_DDS0_AM = 0x9000;
__alt_mem_LOOKUP_RAM_RAM_DDS0_FM = 0x9800;
__alt_mem_LOOKUP_RAM_RAM_DDS0 = 0xa000;
__alt_mem_LOOKUP_RAM_RAM_DDS1_AM = 0xa800;
__alt_mem_LOOKUP_RAM_RAM_DDS1_FM = 0xb000;
__alt_mem_LOOKUP_RAM_RAM_DDS1 = 0xb800;
__alt_mem_FLASH_data = 0x40000;

OUTPUT_FORMAT( "elf32-littlenios2",
               "elf32-littlenios2",
               "elf32-littlenios2" )
OUTPUT_ARCH( nios2 )
ENTRY( _start )

/*
 * The alt_load() facility is enabled. This typically happens when there isn't
 * an external bootloader (e.g. flash bootloader).
 * The LMA (aka physical address) of each loaded section is
 * set to the .text memory device.
 * The HAL alt_load() routine called from crt0 copies sections from
 * the .text memory to RAM as needed.
 */

SECTIONS
{

    /*
     * Output sections associated with reset and exceptions (they have to be first)
     */

    .entry :
    {
        KEEP (*(.entry))
    } > reset

    .exceptions :
    {
        PROVIDE (__ram_exceptions_start = ABSOLUTE(.));
        . = ALIGN(0x20);
        KEEP (*(.irq));
        KEEP (*(.exceptions.entry.label));
        KEEP (*(.exceptions.entry.user));
        KEEP (*(.exceptions.entry.ecc_fatal));
        KEEP (*(.exceptions.entry));
        KEEP (*(.exceptions.irqtest.user));
        KEEP (*(.exceptions.irqtest));
        KEEP (*(.exceptions.irqhandler.user));
        KEEP (*(.exceptions.irqhandler));
        KEEP (*(.exceptions.irqreturn.user));
        KEEP (*(.exceptions.irqreturn));
        KEEP (*(.exceptions.notirq.label));
        KEEP (*(.exceptions.notirq.user));
        KEEP (*(.exceptions.notirq));
        KEEP (*(.exceptions.soft.user));
        KEEP (*(.exceptions.soft));
        KEEP (*(.exceptions.unknown.user));
        KEEP (*(.exceptions.unknown));
        KEEP (*(.exceptions.exit.label));
        KEEP (*(.exceptions.exit.user));
        KEEP (*(.exceptions.exit));
        KEEP (*(.exceptions));
        PROVIDE (__ram_exceptions_end = ABSOLUTE(.));
    } > FLASH_data

    PROVIDE (__flash_exceptions_start = LOADADDR(.exceptions));

    .ram_dds0 :
    {
        PROVIDE (_alt_partition_ram_dds0_start = ABSOLUTE(.));
        *(.ram_dds0 .ram_dds0.*)
        . = ALIGN(4);
        PROVIDE (_alt_partition_ram_dds0_end = ABSOLUTE(.));
    } > LOOKUP_RAM_RAM_DDS0

    .ram_dds0_am :
    {
        PROVIDE (_alt_partition_ram_dds0_am_start = ABSOLUTE(.));
        *(.ram_dds0_am .ram_dds0_am.*)
        . = ALIGN(4);
        PROVIDE (_alt_partition_ram_dds0_am_end = ABSOLUTE(.));
    } > LOOKUP_RAM_RAM_DDS0_AM

    .ram_dds0_fm :
    {
        PROVIDE (_alt_partition_ram_dds0_fm_start = ABSOLUTE(.));
        *(.ram_dds0_fm .ram_dds0_fm.*)
        . = ALIGN(4);
        PROVIDE (_alt_partition_ram_dds0_fm_end = ABSOLUTE(.));
    } > LOOKUP_RAM_RAM_DDS0_FM

    .ram_dds1 :
    {
        PROVIDE (_alt_partition_ram_dds1_start = ABSOLUTE(.));
        *(.ram_dds1 .ram_dds1.*)
        . = ALIGN(4);
        PROVIDE (_alt_partition_ram_dds1_end = ABSOLUTE(.));
    } > LOOKUP_RAM_RAM_DDS1

    .ram_dds1_am :
    {
        PROVIDE (_alt_partition_ram_dds1_am_start = ABSOLUTE(.));
        *(.ram_dds1_am .ram_dds1_am.*)
        . = ALIGN(4);
        PROVIDE (_alt_partition_ram_dds1_am_end = ABSOLUTE(.));
    } > LOOKUP_RAM_RAM_DDS1_AM

    .ram_dds1_fm :
    {
        PROVIDE (_alt_partition_ram_dds1_fm_start = ABSOLUTE(.));
        *(.ram_dds1_fm .ram_dds1_fm.*)
        . = ALIGN(4);
        PROVIDE (_alt_partition_ram_dds1_fm_end = ABSOLUTE(.));
    } > LOOKUP_RAM_RAM_DDS1_FM

    .text :
    {
        /*
         * All code sections are merged into the text output section, along with
         * the read only data sections.
         *
         */

        PROVIDE (stext = ABSOLUTE(.));

        *(.interp)
        *(.hash)
        *(.dynsym)
        *(.dynstr)
        *(.gnu.version)
        *(.gnu.version_d)
        *(.gnu.version_r)
        *(.rel.init)
        *(.rela.init)
        *(.rel.text .rel.text.* .rel.gnu.linkonce.t.*)
        *(.rela.text .rela.text.* .rela.gnu.linkonce.t.*)
        *(.rel.fini)
        *(.rela.fini)
        *(.rel.rodata .rel.rodata.* .rel.gnu.linkonce.r.*)
        *(.rela.rodata .rela.rodata.* .rela.gnu.linkonce.r.*)
        *(.rel.data .rel.data.* .rel.gnu.linkonce.d.*)
        *(.rela.data .rela.data.* .rela.gnu.linkonce.d.*)
        *(.rel.tdata .rel.tdata.* .rel.gnu.linkonce.td.*)
        *(.rela.tdata .rela.tdata.* .rela.gnu.linkonce.td.*)
        *(.rel.tbss .rel.tbss.* .rel.gnu.linkonce.tb.*)
        *(.rela.tbss .rela.tbss.* .rela.gnu.linkonce.tb.*)
        *(.rel.ctors)
        *(.rela.ctors)
        *(.rel.dtors)
        *(.rela.dtors)
        *(.rel.got)
        *(.rela.got)
        *(.rel.sdata .rel.sdata.* .rel.gnu.linkonce.s.*)
        *(.rela.sdata .rela.sdata.* .rela.gnu.linkonce.s.*)
        *(.rel.sbss .rel.sbss.* .rel.gnu.linkonce.sb.*)
        *(.rela.sbss .rela.sbss.* .rela.gnu.linkonce.sb.*)
        *(.rel.sdata2 .rel.sdata2.* .rel.gnu.linkonce.s2.*)
        *(.rela.sdata2 .rela.sdata2.* .rela.gnu.linkonce.s2.*)
        *(.rel.sbss2 .rel.sbss2.* .rel.gnu.linkonce.sb2.*)
        *(.rela.sbss2 .rela.sbss2.* .rela.gnu.linkonce.sb2.*)
        *(.rel.bss .rel.bss.* .rel.gnu.linkonce.b.*)
        *(.rela.bss .rela.bss.* .rela.gnu.linkonce.b.*)
        *(.rel.plt)
        *(.rela.plt)
        *(.rel.dyn)

        KEEP (*(.init))
        *(.plt)
        *(.text .stub .text.* .gnu.linkonce.t.*)

        /* .gnu.warning sections are handled specially by elf32.em.  */

        *(.gnu.warning.*)
        KEEP (*(.fini))
        PROVIDE (__etext = ABSOLUTE(.));
        PROVIDE (_etext = ABSOLUTE(.));
        PROVIDE (etext = ABSOLUTE(.));

        *(.eh_frame_hdr)
        /* Ensure the __preinit_array_start label is properly aligned.  We
           could instead move the label definition inside the section, but
           the linker would then create the section even if it turns out to
           be empty, which isn't pretty.  */
        . = ALIGN(4);
        PROVIDE (__preinit_array_start = ABSOLUTE(.));
        *(.preinit_array)
        PROVIDE (__preinit_array_end = ABSOLUTE(.));
        PROVIDE (__init_array_start = ABSOLUTE(.));
        *(.init_array)
        PROVIDE (__init_array_end = ABSOLUTE(.));
        PROVIDE (__fini_array_start = ABSOLUTE(.));
        *(.fini_array)
        PROVIDE (__fini_array_end = ABSOLUTE(.));
        SORT(CONSTRUCTORS)
        KEEP (*(.eh_frame))
        *(.gcc_except_table .gcc_except_table.*)
        *(.dynamic)
        PROVIDE (__CTOR_LIST__ = ABSOLUTE(.));
        KEEP (*(.ctors))
        KEEP (*(SORT(.ctors.*)))
        PROVIDE (__CTOR_END__ = ABSOLUTE(.));
        PROVIDE (__DTOR_LIST__ = ABSOLUTE(.));
        KEEP (*(.dtors))
        KEEP (*(SORT(.dtors.*)))
        PROVIDE (__DTOR_END__ = ABSOLUTE(.));
        KEEP (*(.jcr))
        . = ALIGN(4);
    } > FLASH_data = 0x3a880100 /* NOP instruction (always in big-endian byte ordering) */

    /*
     *
     * This section's LMA is set to the .text region.
     * crt0 will copy to this section's specified mapped region virtual memory address (VMA)
     *
     */

    .rodata : AT ( LOADADDR (.text) + SIZEOF (.text) )
    {
        PROVIDE (__ram_rodata_start = ABSOLUTE(.));
        . = ALIGN(4);
        *(.rodata .rodata.* .gnu.linkonce.r.*)
        *(.rodata1)
        . = ALIGN(4);
        PROVIDE (__ram_rodata_end = ABSOLUTE(.));
    } > RAM_24K

    PROVIDE (__flash_rodata_start = LOADADDR(.rodata));

    /*
     *
     * This section's LMA is set to the .text region.
     * crt0 will copy to this section's specified mapped region virtual memory address (VMA)
     *
     */

    .rwdata : AT ( LOADADDR (.rodata) + SIZEOF (.rodata) )
    {
        PROVIDE (__ram_rwdata_start = ABSOLUTE(.));
        . = ALIGN(4);
        *(.got.plt) *(.got)
        *(.data1)
        *(.data .data.* .gnu.linkonce.d.*)

        _gp = ABSOLUTE(. + 0x8000);
        PROVIDE(gp = _gp);

        *(.rwdata .rwdata.*)
        *(.sdata .sdata.* .gnu.linkonce.s.*)
        *(.sdata2 .sdata2.* .gnu.linkonce.s2.*)

        . = ALIGN(4);
        _edata = ABSOLUTE(.);
        PROVIDE (edata = ABSOLUTE(.));
        PROVIDE (__ram_rwdata_end = ABSOLUTE(.));
    } > RAM_24K

    PROVIDE (__flash_rwdata_start = LOADADDR(.rwdata));

    .bss :
    {
        __bss_start = ABSOLUTE(.);
        PROVIDE (__sbss_start = ABSOLUTE(.));
        PROVIDE (___sbss_start = ABSOLUTE(.));

        *(.dynsbss)
        *(.sbss .sbss.* .gnu.linkonce.sb.*)
        *(.sbss2 .sbss2.* .gnu.linkonce.sb2.*)
        *(.scommon)

        PROVIDE (__sbss_end = ABSOLUTE(.));
        PROVIDE (___sbss_end = ABSOLUTE(.));

        *(.dynbss)
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)

        . = ALIGN(4);
        __bss_end = ABSOLUTE(.);
    } > RAM_24K

    /*
     *
     * One output section mapped to the associated memory device for each of
     * the available memory devices. These are not used by default, but can
     * be used by user applications by using the .section directive.
     *
     * The output section used for the heap is treated in a special way,
     * i.e. the symbols "end" and "_end" are added to point to the heap start.
     *
     * Because alt_load() is enabled, these sections have
     * their LMA set to be loaded into the .text memory region.
     * However, the alt_load() code will NOT automatically copy
     * these sections into their mapped memory region.
     *
     */

    /*
     *
     * This section's LMA is set to the .text region.
     * crt0 will copy to this section's specified mapped region virtual memory address (VMA)
     *
     */

    .RAM_24K : AT ( LOADADDR (.rwdata) + SIZEOF (.rwdata) )
    {
        PROVIDE (_alt_partition_RAM_24K_start = ABSOLUTE(.));
        *(.RAM_24K .RAM_24K. RAM_24K.*)
        . = ALIGN(4);
        PROVIDE (_alt_partition_RAM_24K_end = ABSOLUTE(.));
        _end = ABSOLUTE(.);
        end = ABSOLUTE(.);
        __alt_stack_base = ABSOLUTE(.);
    } > RAM_24K

    PROVIDE (_alt_partition_RAM_24K_load_addr = LOADADDR(.RAM_24K));

    /*
     *
     * This section's LMA is set to the .text region.
     * crt0 will copy to this section's specified mapped region virtual memory address (VMA)
     *
     */

    .LOOKUP_RAM_RAM_DDS0_AM : AT ( LOADADDR (.RAM_24K) + SIZEOF (.RAM_24K) )
    {
        PROVIDE (_alt_partition_LOOKUP_RAM_RAM_DDS0_AM_start = ABSOLUTE(.));
        *(.LOOKUP_RAM_RAM_DDS0_AM .LOOKUP_RAM_RAM_DDS0_AM. LOOKUP_RAM_RAM_DDS0_AM.*)
        . = ALIGN(4);
        PROVIDE (_alt_partition_LOOKUP_RAM_RAM_DDS0_AM_end = ABSOLUTE(.));
    } > LOOKUP_RAM_RAM_DDS0_AM

    PROVIDE (_alt_partition_LOOKUP_RAM_RAM_DDS0_AM_load_addr = LOADADDR(.LOOKUP_RAM_RAM_DDS0_AM));

    /*
     *
     * This section's LMA is set to the .text region.
     * crt0 will copy to this section's specified mapped region virtual memory address (VMA)
     *
     */

    .LOOKUP_RAM_RAM_DDS0_FM : AT ( LOADADDR (.LOOKUP_RAM_RAM_DDS0_AM) + SIZEOF (.LOOKUP_RAM_RAM_DDS0_AM) )
    {
        PROVIDE (_alt_partition_LOOKUP_RAM_RAM_DDS0_FM_start = ABSOLUTE(.));
        *(.LOOKUP_RAM_RAM_DDS0_FM .LOOKUP_RAM_RAM_DDS0_FM. LOOKUP_RAM_RAM_DDS0_FM.*)
        . = ALIGN(4);
        PROVIDE (_alt_partition_LOOKUP_RAM_RAM_DDS0_FM_end = ABSOLUTE(.));
    } > LOOKUP_RAM_RAM_DDS0_FM

    PROVIDE (_alt_partition_LOOKUP_RAM_RAM_DDS0_FM_load_addr = LOADADDR(.LOOKUP_RAM_RAM_DDS0_FM));

    /*
     *
     * This section's LMA is set to the .text region.
     * crt0 will copy to this section's specified mapped region virtual memory address (VMA)
     *
     */

    .LOOKUP_RAM_RAM_DDS0 : AT ( LOADADDR (.LOOKUP_RAM_RAM_DDS0_FM) + SIZEOF (.LOOKUP_RAM_RAM_DDS0_FM) )
    {
        PROVIDE (_alt_partition_LOOKUP_RAM_RAM_DDS0_start = ABSOLUTE(.));
        *(.LOOKUP_RAM_RAM_DDS0 .LOOKUP_RAM_RAM_DDS0. LOOKUP_RAM_RAM_DDS0.*)
        . = ALIGN(4);
        PROVIDE (_alt_partition_LOOKUP_RAM_RAM_DDS0_end = ABSOLUTE(.));
    } > LOOKUP_RAM_RAM_DDS0

    PROVIDE (_alt_partition_LOOKUP_RAM_RAM_DDS0_load_addr = LOADADDR(.LOOKUP_RAM_RAM_DDS0));

    /*
     *
     * This section's LMA is set to the .text region.
     * crt0 will copy to this section's specified mapped region virtual memory address (VMA)
     *
     */

    .LOOKUP_RAM_RAM_DDS1_AM : AT ( LOADADDR (.LOOKUP_RAM_RAM_DDS0) + SIZEOF (.LOOKUP_RAM_RAM_DDS0) )
    {
        PROVIDE (_alt_partition_LOOKUP_RAM_RAM_DDS1_AM_start = ABSOLUTE(.));
        *(.LOOKUP_RAM_RAM_DDS1_AM .LOOKUP_RAM_RAM_DDS1_AM. LOOKUP_RAM_RAM_DDS1_AM.*)
        . = ALIGN(4);
        PROVIDE (_alt_partition_LOOKUP_RAM_RAM_DDS1_AM_end = ABSOLUTE(.));
    } > LOOKUP_RAM_RAM_DDS1_AM

    PROVIDE (_alt_partition_LOOKUP_RAM_RAM_DDS1_AM_load_addr = LOADADDR(.LOOKUP_RAM_RAM_DDS1_AM));

    /*
     *
     * This section's LMA is set to the .text region.
     * crt0 will copy to this section's specified mapped region virtual memory address (VMA)
     *
     */

    .LOOKUP_RAM_RAM_DDS1_FM : AT ( LOADADDR (.LOOKUP_RAM_RAM_DDS1_AM) + SIZEOF (.LOOKUP_RAM_RAM_DDS1_AM) )
    {
        PROVIDE (_alt_partition_LOOKUP_RAM_RAM_DDS1_FM_start = ABSOLUTE(.));
        *(.LOOKUP_RAM_RAM_DDS1_FM .LOOKUP_RAM_RAM_DDS1_FM. LOOKUP_RAM_RAM_DDS1_FM.*)
        . = ALIGN(4);
        PROVIDE (_alt_partition_LOOKUP_RAM_RAM_DDS1_FM_end = ABSOLUTE(.));
    } > LOOKUP_RAM_RAM_DDS1_FM

    PROVIDE (_alt_partition_LOOKUP_RAM_RAM_DDS1_FM_load_addr = LOADADDR(.LOOKUP_RAM_RAM_DDS1_FM));

    /*
     *
     * This section's LMA is set to the .text region.
     * crt0 will copy to this section's specified mapped region virtual memory address (VMA)
     *
     */

    .LOOKUP_RAM_RAM_DDS1 : AT ( LOADADDR (.LOOKUP_RAM_RAM_DDS1_FM) + SIZEOF (.LOOKUP_RAM_RAM_DDS1_FM) )
    {
        PROVIDE (_alt_partition_LOOKUP_RAM_RAM_DDS1_start = ABSOLUTE(.));
        *(.LOOKUP_RAM_RAM_DDS1 .LOOKUP_RAM_RAM_DDS1. LOOKUP_RAM_RAM_DDS1.*)
        . = ALIGN(4);
        PROVIDE (_alt_partition_LOOKUP_RAM_RAM_DDS1_end = ABSOLUTE(.));
    } > LOOKUP_RAM_RAM_DDS1

    PROVIDE (_alt_partition_LOOKUP_RAM_RAM_DDS1_load_addr = LOADADDR(.LOOKUP_RAM_RAM_DDS1));

    /*
     *
     * This section's LMA is set to the .text region.
     * crt0 will copy to this section's specified mapped region virtual memory address (VMA)
     *
     */

    .FLASH_data LOADADDR (.LOOKUP_RAM_RAM_DDS1) + SIZEOF (.LOOKUP_RAM_RAM_DDS1) : AT ( LOADADDR (.LOOKUP_RAM_RAM_DDS1) + SIZEOF (.LOOKUP_RAM_RAM_DDS1) )
    {
        PROVIDE (_alt_partition_FLASH_data_start = ABSOLUTE(.));
        *(.FLASH_data .FLASH_data. FLASH_data.*)
        . = ALIGN(4);
        PROVIDE (_alt_partition_FLASH_data_end = ABSOLUTE(.));
    } > FLASH_data

    PROVIDE (_alt_partition_FLASH_data_load_addr = LOADADDR(.FLASH_data));

    /*
     * Stabs debugging sections.
     *
     */

    .stab          0 : { *(.stab) }
    .stabstr       0 : { *(.stabstr) }
    .stab.excl     0 : { *(.stab.excl) }
    .stab.exclstr  0 : { *(.stab.exclstr) }
    .stab.index    0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment       0 : { *(.comment) }
    /* DWARF debug sections.
       Symbols in the DWARF debugging sections are relative to the beginning
       of the section so we begin them at 0.  */
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }

    /* Altera debug extensions */
    .debug_alt_sim_info 0 : { *(.debug_alt_sim_info) }
}

/* provide a pointer for the stack */

/*
 * Don't override this, override the __alt_stack_* symbols instead.
 */
__alt_data_end = 0x5dc0;

/*
 * The next two symbols define the location of the default stack.  You can
 * override them to move the stack to a different memory.
 */
PROVIDE( __alt_stack_pointer = __alt_data_end );
PROVIDE( __alt_stack_limit   = __alt_stack_base );

/*
 * This symbol controls where the start of the heap is.  If the stack is
 * contiguous with the heap then the stack will contract as memory is
 * allocated to the heap.
 * Override this symbol to put the heap in a different memory.
 */
PROVIDE( __alt_heap_start    = end );
PROVIDE( __alt_heap_limit    = 0x5dc0 );
