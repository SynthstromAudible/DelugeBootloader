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
* Copyright (C) 2012 - 2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/******************************************************************************
* File Name    : devdrv_ostm.h
* $Rev: 809 $
* $Date:: 2014-04-09 15:06:36 +0900#$
* Description  : OS timer driver header
******************************************************************************/
#ifndef DEVDRV_OSTM_H
#define DEVDRV_OSTM_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/


/******************************************************************************
Typedef definitions
******************************************************************************/


/******************************************************************************
Macro definitions
******************************************************************************/
/* ==== OSTM H/W specification ==== */
#define OSTM_MODE_INTERVAL  (0)     /* OSTM operating mode : Interval timer mode       */
#define OSTM_MODE_COMPARE   (1)     /* OSTM operating mode : Free running compare mode */

/******************************************************************************
Variable Externs
******************************************************************************/


/******************************************************************************
Functions Prototypes
******************************************************************************/
/* ==== API functions ==== */
int32_t R_OSTM_Init(uint32_t channel, uint32_t mode, uint32_t cycle);
int32_t R_OSTM_Open(uint32_t channel);
int32_t R_OSTM_Close(uint32_t channel, uint32_t * count);

/* ==== User-defined functions ==== */
int32_t Userdef_OSTM0_Init(uint32_t mode, uint32_t cycle);
int32_t Userdef_OSTM1_Init(uint32_t mode, uint32_t cycle);


#endif  /* DEVDRV_OSTM_H */

/* End of File */
