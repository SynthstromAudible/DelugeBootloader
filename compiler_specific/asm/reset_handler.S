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
*******************************************************************************/
/*******************************************************************************
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/******************************************************************************
* File Name     : reset_handler.S
* Device(s)     : RZ/A1H RSK+RZA1H
* Tool-Chain    : GNUARM-NONEv14.02-EABI
* H/W Platform  : RSK+RZA1H CPU Board
* Description   : Called by reset vector (start.S/mirrorstart.S)
*               : As such its the startpoint for this software
******************************************************************************/
/******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 21.10.2014 1.00
******************************************************************************/

    .global reset_handler
    .global undefined_handler
    .global svc_handler
    .global prefetch_handler
    .global abort_handler
    .global reserved_handler
    .global irq_handler
    .global fiq_handler

    .text

/* Standard definitions of mode bits and interrupt flags in PSRs */
    .equ    USR_MODE , 0x10
    .equ    FIQ_MODE , 0x11
    .equ    IRQ_MODE , 0x12
    .equ    SVC_MODE , 0x13
    .equ    ABT_MODE , 0x17
    .equ    UND_MODE , 0x1b
    .equ    SYS_MODE , 0x1f
    .equ    Thum_bit , 0x20                           /* CPSR/SPSR Thumb bit */

/* ========================================================================= */
/* Entry point for the Reset handler */
/* ========================================================================= */
//;******************************************************************************
//; Function Name : reset_handler
//; Description   : This function is the assembler function executed after reset
//;               : cancellation. After initial setting for the stack pointer or
//;               : the MMU and reset cancellation, executes initial setting for
//;               : the minimum required peripheral functions. Calls the __main
//;               : of the standard library function to execute the main function.
//;******************************************************************************
reset_handler:
/* ========================================================================= */
/* Multi-core startup (future proofing boot code)                            */
/* Check core, if not core 0  put to sleep.                                  */
/* ========================================================================= */
        MRC     p15, 0, r0, c0, c0, 5
        ANDS    r0, r0, #3
goToSleep:
        WFINE
        BNE     goToSleep

/* ========================================================================= */
//; Clock Setting
/* ========================================================================= */
//    ; Set standby_mode_en of Power Control Register
    LDR  r0, =0x3FFFFF80
    LDR  r1, =0x00000001
    STR  r1, [r0]
    LDR  r0, [r0]                    /*;;; dummy read*/

//    ; FRQCR = 0x1035  PLL(x30), I:G:B:P1:P0 = 30:20:10:5:5/2
    LDR  r0, =0xFCFE0010
    LDR  r1, =0x1035
    STRH r1, [r0]
    LDR  r0, [r0]                    /*;;; dummy read*/

    ; FRQCR2 = 0x0001
    LDR  r0, =0xFCFE0014
    LDR  r1, =0x0001
    STRH r1, [r0]
    LDR  r0, [r0]                    /*;;; dummy read*/

/* ========================================================================= */
//; Set SCTLR High Vectors
//;      0 = Normal (Low address) Vectors
//;      1 = High (High address) Vectors
/* ========================================================================= */
    MRC  p15, 0, r0, c1, c0, 0  //;;; Read CP15 System Control register (SCTLR)
    BIC  r0, r0, #(0x1 << 13)   //;;; Clear V bit 13 to set Low Vectors
    MCR  p15, 0, r0, c1, c0, 0  //;;; Write CP15 System Control register
/* ========================================================================= */
/* Disable cache and MMU in case it was left enabled from an earlier         */
/* This does not need to be done from a cold reset                           */
/*****************************************************************************/
    MRC  p15, 0, r0, c1, c0, 0 /* Read CP15 SCTLR                            */
    BIC  r0, r0, #(0x1 << 13)  /* Set Low Vectors                            */
    BIC  r0, r0, #(0x1 << 12)  /* Clear I bit 12 to disable I Cache          */
    BIC  r0, r0, #(0x1 <<  2)  /* Clear C bit  2 to disable D Cache          */
    BIC  r0, r0, #0x1          /* Clear M bit  0 to disable MMU              */
    MCR  p15, 0, r0, c1, c0, 0 /* Write value back to CP15 SCR               */

/* ========================================================================= */
/*    Reset Vbar to this programs vectors                                    */
/* ========================================================================= */
    BL   VbarInit

/* ========================================================================= */
/*  Setting up Stack Area handled by .ld file                                 */
/* ========================================================================= */
/* SVC Mode(Default)                                                         */
    CPS  #SVC_MODE
    LDR  sp, =svc_stack_end

/* ========================================================================= */
/* Branch to C library entry point                                           */
/* ========================================================================= */
    LDR  r12,=PowerON_Reset
    BX   r12                              /* Branch to C library entry point */

reset_handler_end:
    B    reset_handler_end         //  ;;; Looping in this address
/* ========================================================================= */
/* Other Handler                                                             */
/* ========================================================================= */
undefined_handler:
    B    undefined_handler

svc_handler:
    B    svc_handler

prefetch_handler:
    B    prefetch_handler

abort_handler:
    B    abort_handler

reserved_handler:
    B    reserved_handler
.end

