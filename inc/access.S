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
/*******************************************************************************
* File Name     : access.S
* Device(s)     : RZ/A1H (R7S721001)
* Tool-Chain    : GNUARM-NONEv14.02-EABI
* H/W Platform  : RSK+RZA1H CPU Board
* Description   : This is the code provides access to the co-processors
*******************************************************************************/
/*******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 21.10.2014 1.00
*******************************************************************************/

    .text
    .code 32

        .global enable_neon_vfp_access_priv
        .global enable_neon_vfp_access_full
        .global disable_neon_vfp_access
        .global neon_vfp_on
        .global neon_vfp_off
        .global enable_mmu
        .global disable_mmu
        .global VbarInit
        .global SetLowVectors
        .global SetHighVectors

/* ========================================================================== */
/* Enable MMU                                                                 */
/* Leaving the caches disabled.                                               */
/* ========================================================================== */
    .func enable_mmu
enable_mmu:
    MRC  p15, 0, r0, c1, c0, 0     /* Read CP15 System Control register (SCTLR) */
    BIC  r0, r0, #(0x1 << 12)             /* Clear I bit 12 to disable I Cache */
    BIC  r0, r0, #(0x1 <<  2)             /* Clear C bit  2 to disable D Cache */
    BIC  r0, r0, #0x2       /* Clear A bit 1 to disable alignment fault check */
    ORR  r0, r0, #0x1      /* Set M bit 1 to enable MMU before scatter loading */
    MCR  p15, 0, r0, c1, c0, 0            /* Write CP15 System Control register */
    ISB
    BX        lr
    .endfunc
    .align 4


