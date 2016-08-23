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
* File Name     : typedefine.h
* Device(s)     : RZ/A1H (R7S721001)
* Tool-Chain    : GNUARM-NONEv14.02-EABI
* H/W Platform  : RSK+RZA1H CPU Board
* Description   : Defines exact width integer types.
*******************************************************************************/
/*******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 21.10.2014 1.00
*******************************************************************************/

/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>


/* Multiple inclusion prevention macro */
#ifndef TYPE_DEFINE_H
#define TYPE_DEFINE_H


/*******************************************************************************
Typedef definitions
*******************************************************************************/
typedef signed   char      _SBYTE;
typedef unsigned char      _UBYTE;
typedef signed   short     _SWORD;
typedef unsigned short     _UWORD;
typedef signed   int       _SINT;
typedef unsigned int       _UINT;
typedef signed   long      _SDWORD;
typedef unsigned long      _UDWORD;
typedef signed   long long _SQWORD;
typedef unsigned long long _UQWORD;

/* TYPE_DEFINE_H */
#endif

/* End of File */
