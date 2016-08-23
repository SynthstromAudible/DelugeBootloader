/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/******************************************************************************
* File Name     : GNU_e2studio-QSPI_SpSb.ld
* Device(s)     : RZ/A1H RSK+RZA1H
* H/W Platform  : RSK+RZA1H CPU Board
* Description   : Linker file for projects that require to run from 
*               : Single Port Single bit SPI multi I/O device ROM 
******************************************************************************/
/******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 21.10.2014 1.00
******************************************************************************/
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(start)

/* Base Address RAM Memory Table 10-Mbyte on-chip RAM */  
/* Total RAM available 10112K */
MEMORY 
{
	/* Internal RAM address range H'2000_0000 to H'2001_FFFF is configured as data retention RAM */
	/* Write access to this address range has to be enabled by writing to registers SYSCR1 and SYSCR2 */
	RAM0L (rwx)   : ORIGIN = 0x20020000, LENGTH = 0x000E0000  /* Page 0 Lower bank ( 896KB) H'20020000 to H'200FFFFF */
	RAM1L (rwx)   : ORIGIN = 0x20100000, LENGTH = 0x00100000  /* Page 1 Upper bank (1024KB) H'20100000 to H'201FFFFF */
	RAM2L (rwx)   : ORIGIN = 0x20200000, LENGTH = 0x00100000  /* Page 2 Upper bank (1024KB) H'20200000 to H'202FFFFF */
	RAM3L (rwx)   : ORIGIN = 0x20300000, LENGTH = 0x00100000  /* Page 3 Upper bank (1024KB) H'20300000 to H'203FFFFF */
	RAM4L (rwx)   : ORIGIN = 0x20400000, LENGTH = 0x00100000  /* Page 4 Upper bank (1024KB) H'20400000 to H'204FFFFF */
	RAM0U (rwx)   : ORIGIN = 0x20500000, LENGTH = 0x00100000  /* Page 0 Upper bank (1024KB) H'20500000 to H'205FFFFF */
	RAM1U (rwx)   : ORIGIN = 0x20600000, LENGTH = 0x00100000  /* Page 1 Upper bank (1024KB) H'20600000 to H'206FFFFF */
	RAM2U (rwx)   : ORIGIN = 0x20700000, LENGTH = 0x00100000  /* Page 2 Upper bank (1024KB) H'20700000 to H'207FFFFF */
	RAM3U (rwx)   : ORIGIN = 0x20800000, LENGTH = 0x00100000  /* Page 3 Upper bank (1024KB) H'20800000 to H'208FFFFF */
	RAM4U (rwx)   : ORIGIN = 0x20900000, LENGTH = 0x00100000  /* Page 4 Upper bank (1024KB) H'20900000 to H'209FFFFF */
	SDRAM (rwx)   : ORIGIN = 0x48000000, LENGTH = 0x02000000  /* SDRAM 32MB attached to CS2 H'08000000 to H'09FFFFFF */

	RAM012L   (rwx) : ORIGIN = 0x20020000, LENGTH = 0x002E0000
	RAM01234L (rwx) : ORIGIN = 0x20020000, LENGTH = 0x007E0000
	RAM012U   (rwx) : ORIGIN = 0x20500000, LENGTH = 0x00300000

	ROM   (rw)    : ORIGIN = 0x18000000, LENGTH = 0x02000000  /* SPI ROM attached to H'18000000 to H'19FFFFFF */
	VRAM  (rw)    : ORIGIN = 0x60300000, LENGTH = 0x00700000  
}

PROGRAM_STACK_SIZE    = 0x8000;      /* Application stack                  */
IRQ_STACK_SIZE	      = 0x2000;      /* IRQ mode stack                     */
FIQ_STACK_SIZE	      = 0x2000;      /* FRQ mode stack                     */
SVC_STACK_SIZE	      = 0x2000;      /* SVC mode stack                     */
ABT_STACK_SIZE	      = 0x2000;      /* ABT mode stack                     */
TTB_SIZE              = 0x8000;      /* Level-1 Translation Table for MMU  */

LOAD_MODULE1          = 0x18000000;  /* QSPI Boot Location */
LOAD_MODULE2          = 0x18000300;  /* 0x00000400 */
LOAD_MODULE3          = 0x18000800;  /* 0x00004D00  ROM Area (0x18000400-0x18003FFF) */
EXEC_BASE             = 0x20020000;  /* The location for executable code (copied to RAM) */
STACK_BASE            = 0x202E8000; /* Stacks and TTB. 0x202E8000 is the latest possible                     */


SECTIONS
{

    /* Initial code section no execution address is specified so the code will run
    from the load address which is the start of QSPI defined for QSPI boot.*/
    .reset LOAD_MODULE1  :  AT (LOAD_MODULE1)
    {
        /* The start object file begins with the vector table, and so must be located here. */
        *start.o (.text)
        *start.o (.rodata)
        *start.o (.data)
        /* Reallocate the vector base register to the start of the boot loader. */
        *vbar_init.o (.text)
        *vbar_init.o (.rodata)
        *vbar_init.o (.data) 
        
 		/* Power ON Reset. */
		*resetprg.o (.text)
        *resetprg.o (.rodata)
        *resetprg.o (.data)
        
        
                /* Any following code must be aligned to a 4 byte boundary. */
        . = ALIGN(0x4);
        
        __code_reset_handler_start__ = .;
		PROVIDE(__code_reset_handler_start__ = .);

		*reset_handler.o (.text)
		PROVIDE(_code_reset_handler_end__ = .);
        /* The following data must be aligned to a 8 byte boundary. */
        . = ALIGN(0x8);
		*(.rodata.compiler_specific\asmreset_handler.o)
        
        
        /* The QSPI re-configuration needs to be copied to RAM by this code (single SPI)*/
        /* The section that gets copied is LoadModule 2 */
        /* (__code_spibsc_init_1_start__ to __code_spibsc_init_1_end__*/
        *init_spibsc_init1_section.o (.text)
        *init_spibsc_init1_section.o (.rodata)
        *init_spibsc_init1_section.o (.data)
        PROVIDE(load1_end = .);
	}

    /* Load Module 2, Executes from "EXEC_BASE" and physically stored (before copy) at 
       the address specified by LOAD_MODULE2. */

	.load2 EXEC_BASE : AT (LOAD_MODULE2)
	{
        /* Any following code must be aligned to a 4 byte boundary. */
        . = ALIGN(0x4);

        /* The load of this code is completed by the built in boot code, at slow speed. */
        /* This short code section runs first to reconfigure the SPI to high speed. */
        /* The following location is available in the program to be copied by init_init1.*/

        /* relocated address of section used in init_spibsc_init1_section */
        __code_spibsc_init_1_src__ = LOADADDR(.load2); 
		PROVIDE(code_spibsc_init_1_src__ = LOADADDR(.load2));	

        __code_spibsc_init_1_start__ = .;
        PROVIDE(code_spibsc_init_1_start__ = .);

        /* Code to copy section 2 to RAM should be placed here */
		*init_spibsc_init2_section.o (.text)
		*init_spibsc_init2_section.o (.rodata)
		*init_spibsc_init2_section.o (.data)
		
        /* The QSPI re-configuration needs to be copied to RAM by this code (single SPI)*/
        /* The section that gets copied is LoadModule 2 */
        /* (__code_spibsc_init_1_start__ to __code_spibsc_init_1_end__*/
        /* Reconfiguration Functions. To speed up the SPI only.*/
		*spibsc_init1.o (.text)
		*spibsc_init1.o (.rodata)
		*spibsc_init1.o (.data)
		
        /* Code for accessing the IO registers. This is used by both loaded sections. */
		*rza_io_regrw.o (.text)
		*rza_io_regrw.o (.rodata)
		*rza_io_regrw.o (.data)

        /* The following location is available in the program to be copied by init_init1.*/
        __code_spibsc_init_1_end__ = .;
		PROVIDE(__code_spibsc_init_1_end__ = .);

        /* Any following code must be aligned to a 4 byte boundary. */
		. = ALIGN(0x4);
        PROVIDE(load2_end = .); 
	} 
	
    /* Load Module 3, Executes following the end of "EXEC_BASE" and Load Module 2 and
       physically stored (before copy) at the address specified by LOAD_MODULE3. */
	.load3 load2_end :  AT (LOAD_MODULE3)
	{
        /* Any following code must be aligned to a 4 byte boundary. */
        . = ALIGN(0x4);
        
        /* The load of this code is completed by the Module 2 code, at high speed. */
        /* This code section runs to reconfigure the SPI to the final start DQSPI. */
        /* The code then looks for valid user code. */
        
        /* relocated address of section used in init_spibsc_init2_section */
        __code_spibsc_init_2_src__ = LOADADDR(.load3); 
		PROVIDE(code_spibsc_init_2_src__ = LOADADDR(.load3));	

        /* Make this location available in the program to be copied by init_init2.*/
        __code_spibsc_init_2_start__ = .;
        PROVIDE(__code_spibsc_init_2_start__ = .);



		/* All other .text, .data and .rodata goes here, as this section is the final one loaded in order for the main functionality to happen */
        *(.text)


        *(.rodata)
        *(.rodata.*)
        . = ALIGN(0x8);
        *(.data)
        *(.data.*)
        *(.got.plt) *(.got)        
        . = ALIGN(0x8);

        /* The following location is available in the program to be copied by init_init2.*/
        __code_spibsc_init_2_end__ = .;
		PROVIDE(__code_spibsc_init_2_end__ = .);

        /* Any following code must be aligned to a 4 byte boundary. */
        . = ALIGN(0x4);
        PROVIDE(load3_end = .);
	}

    /* constant data section. I've emptied this out successfully - Rohan */
	.rodata load3_end : AT (LOAD_MODULE3 + SIZEOF(.load3))
    {

        PROVIDE(end = .);
        PROVIDE(rodata_end = .);
	}

    /* initialised data section */
    .bss rodata_end () :
    {
        _bss = .;
        PROVIDE(__bss_start__ = .);         
        *(.bss)
        *(.bss.**)
        *(COMMON)
        PROVIDE(__bss_end__ = .);
        _ebss = .;
        _end = .;
    }

    
	.program_stack STACK_BASE :
	{
		program_stack_start = .;
		.  += PROGRAM_STACK_SIZE;
		.	= ALIGN(0x4);
		program_stack_end = .;
	} >RAM012L
		
	.irq_stack :
	{
		irq_stack_start = .;
		.  += IRQ_STACK_SIZE;
		.	= ALIGN(0x4);
		irq_stack_end = .;
	} >RAM012L

	.fiq_stack :
	{
		fiq_stack_start = .;
		.  += FIQ_STACK_SIZE;
		.	= ALIGN(0x4);
		fiq_stack_end = .;
	} >RAM012L

	.svc_stack :
	{
		PROVIDE(svc_stack_start = .);
		.  += SVC_STACK_SIZE;
		.	= ALIGN(0x4);
		svc_stack_end = .;
	} >RAM012L

	.abt_stack :
	{
		abt_stack_start = .;
		.  += ABT_STACK_SIZE;
		.	= ALIGN(0x4);
		abt_stack_end = .;
	} >RAM012L
    
	.ttb_mmu1 () : ALIGN(0x4000)
	{
		ttb_mmu1_base = .;
		.  += TTB_SIZE;
		.	= ALIGN(0x4);
		ttb_mmu1_end = .;
	} >RAM012L

	VRAM_SECTION 0x60300000 () : {} > VRAM

}
