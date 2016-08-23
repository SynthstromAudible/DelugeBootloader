;/*******************************************************************************
;* DISCLAIMER
;* This software is supplied by Renesas Electronics Corporation and is only
;* intended for use with Renesas products. No other uses are authorized. This
;* software is owned by Renesas Electronics Corporation and is protected under
;* all applicable laws, including copyright laws.
;* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
;* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
;* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
;* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
;* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
;* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
;* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
;* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
;* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
;* Renesas reserves the right, without notice, to make changes to this software
;* and to discontinue the availability of this software. By using this software,
;* you agree to the additional terms and conditions found by accessing the
;* following link:
;* http://www.renesas.com/disclaimer
;*
;* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
;*******************************************************************************/
/*******************************************************************************
* File Name     : vbar_init.S
* Device(s)     : RZ/A1H RSK+RZA1H
* Tool-Chain    : GNUARM-NONEv14.02-EABI
* H/W Platform  : RSK+RZA1H CPU Board
* Description   : Vector base address initialize
*******************************************************************************/
/*******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 21.10.2014 1.00
*******************************************************************************/

/*;==================================================================*/
/*; This code provides basic global enable for Cortex-A9 cache.*/
/*; It also enables branch prediction*/
/*; This code must be run from a privileged mode*/
/*;==================================================================*/
    .text
    .code 32
    .global VbarInit

/*;==========================================================================*/
/*; VbarInit*/
/*;==========================================================================*/
    .func VbarInit
VbarInit:
/* ========================================================================== */
/* Set Vector Base Address Register to point to application's vector table    */
/* ========================================================================== */
    LDR r0, = start
    MCR p15, 0, r0, c12, c0, 0

    BX      lr
    .endfunc

