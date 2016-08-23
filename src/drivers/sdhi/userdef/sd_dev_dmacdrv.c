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
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name    : sd_dev_dmacdrv.c
* $Rev: $
* $Date::                           $
* Device(s)    : RZ/A1H
* Tool-Chain   : 
*              : 
* OS           : 
* H/W Platform : 
* Description  : RZ/A1H Sample Program - DMAC sample program (Main)
* Operation    : 
* Limitations  : 
*******************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdio.h>
#include "r_typedefs.h"
#include "iodefine.h"
#include "rza_io_regrw.h"
#include "dev_drv.h"                /* Device Driver common header */
#include "devdrv_intc.h"            /* INTC Driver Header */
#include "sd_dev_dmacdrv.h"

/******************************************************************************
Typedef definitions
******************************************************************************/


/******************************************************************************
Macro definitions
******************************************************************************/
#define DMAC_INDEFINE   (255)       /* �s��l */

typedef enum dmac_peri_req_reg_type
{
    DMAC_REQ_MID,
    DMAC_REQ_RID,
    DMAC_REQ_AM,
    DMAC_REQ_LVL,
    DMAC_REQ_REQD
} dmac_peri_req_reg_type_t;

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/


/******************************************************************************
Private global variables and functions
******************************************************************************/
/* ==== Prototype declaration ==== */

/* ==== Global variable ==== */
static const uint8_t sd_dmac_peri_req_init_table[4][5] =
{
/*   MID,RID,AM,LVL,REQD                                                            */
    {  48, 1, 2, 1, 1             },    /* SDHI_0 Tx                                */
    {  48, 2, 2, 1, 0             },    /* SDHI_0 Rx                                */
    {  49, 1, 2, 1, 1             },    /* SDHI_1 Tx                                */
    {  49, 2, 2, 1, 0             },    /* SDHI_1 Rx                                */
};

/******************************************************************************
* Function Name: sd_DMAC1_PeriReqInit
* Description  : 
* Arguments    : 
* Return Value : 
******************************************************************************/
void sd_DMAC1_PeriReqInit(const dmac_transinfo_t * trans_info, uint32_t dmamode, uint32_t continuation, 
                              uint32_t request_factor, uint32_t req_direction)
{
    if (DMAC_MODE_REGISTER == dmamode)
    {
        /* ==== Next0 register set ==== */
        DMAC.N0SA_1 = trans_info->src_addr;
        DMAC.N0DA_1 = trans_info->dst_addr;
        DMAC.N0TB_1 = trans_info->count;

        rza_io_reg_write_32(&DMAC.CHCFG_1,
                            trans_info->daddr_dir,
                            DMAC1_CHCFG_n_DAD_SHIFT,
                            DMAC1_CHCFG_n_DAD);
        rza_io_reg_write_32(&DMAC.CHCFG_1,
                            trans_info->saddr_dir,
                            DMAC1_CHCFG_n_SAD_SHIFT,
                            DMAC1_CHCFG_n_SAD);
        rza_io_reg_write_32(&DMAC.CHCFG_1,
                            trans_info->dst_size,
                            DMAC1_CHCFG_n_DDS_SHIFT,
                            DMAC1_CHCFG_n_DDS);
        rza_io_reg_write_32(&DMAC.CHCFG_1,
                            trans_info->src_size,
                            DMAC1_CHCFG_n_SDS_SHIFT,
                            DMAC1_CHCFG_n_SDS);

        rza_io_reg_write_32(&DMAC.CHCFG_1,
                            0,
                            DMAC1_CHCFG_n_DMS_SHIFT,
                            DMAC1_CHCFG_n_DMS);
        rza_io_reg_write_32(&DMAC.CHCFG_1,
                            0,
                            DMAC1_CHCFG_n_RSEL_SHIFT,
                            DMAC1_CHCFG_n_RSEL);
        rza_io_reg_write_32(&DMAC.CHCFG_1,
                            0,
                            DMAC1_CHCFG_n_SBE_SHIFT,
                            DMAC1_CHCFG_n_SBE);
        rza_io_reg_write_32(&DMAC.CHCFG_1,
                            0,
                            DMAC1_CHCFG_n_DEM_SHIFT,
                            DMAC1_CHCFG_n_DEM);

        if (DMAC_SAMPLE_CONTINUATION == continuation)
        {
            rza_io_reg_write_32(&DMAC.CHCFG_1,
                                1,
                                DMAC1_CHCFG_n_REN_SHIFT,
                                DMAC1_CHCFG_n_REN);
            rza_io_reg_write_32(&DMAC.CHCFG_1,
                                1,
                                DMAC1_CHCFG_n_RSW_SHIFT,
                                DMAC1_CHCFG_n_RSW);
        }
        else
        {
            rza_io_reg_write_32(&DMAC.CHCFG_1,
                                0,
                                DMAC1_CHCFG_n_REN_SHIFT,
                                DMAC1_CHCFG_n_REN);
            rza_io_reg_write_32(&DMAC.CHCFG_1,
                                0,
                                DMAC1_CHCFG_n_RSW_SHIFT,
                                DMAC1_CHCFG_n_RSW);
        }

        rza_io_reg_write_32(&DMAC.CHCFG_1,
                            0,
                            DMAC1_CHCFG_n_TM_SHIFT,
                            DMAC1_CHCFG_n_TM);
        rza_io_reg_write_32(&DMAC.CHCFG_1,
                            1,
                            DMAC1_CHCFG_n_SEL_SHIFT,
                            DMAC1_CHCFG_n_SEL);
        rza_io_reg_write_32(&DMAC.CHCFG_1,
                            1,
                            DMAC1_CHCFG_n_HIEN_SHIFT,
                            DMAC1_CHCFG_n_HIEN);
        rza_io_reg_write_32(&DMAC.CHCFG_1,
                            0,
                            DMAC1_CHCFG_n_LOEN_SHIFT,
                            DMAC1_CHCFG_n_LOEN);

        rza_io_reg_write_32(&DMAC.CHCFG_1,
                            sd_dmac_peri_req_init_table[request_factor][DMAC_REQ_AM],
                            DMAC1_CHCFG_n_AM_SHIFT,
                            DMAC1_CHCFG_n_AM);
        rza_io_reg_write_32(&DMAC.CHCFG_1,
                            sd_dmac_peri_req_init_table[request_factor][DMAC_REQ_LVL],
                            DMAC1_CHCFG_n_LVL_SHIFT,
                            DMAC1_CHCFG_n_LVL);
        if (sd_dmac_peri_req_init_table[request_factor][DMAC_REQ_REQD] != DMAC_INDEFINE)
        {
            rza_io_reg_write_32(&DMAC.CHCFG_1,
                                sd_dmac_peri_req_init_table[request_factor][DMAC_REQ_REQD],
                                DMAC1_CHCFG_n_REQD_SHIFT,
                                DMAC1_CHCFG_n_REQD);
        }
        else
        {
            rza_io_reg_write_32(&DMAC.CHCFG_1,
                                req_direction,
                                DMAC1_CHCFG_n_REQD_SHIFT,
                                DMAC1_CHCFG_n_REQD);
        }
        rza_io_reg_write_32(&DMAC.DMARS0,
                            sd_dmac_peri_req_init_table[request_factor][DMAC_REQ_RID],
                            DMAC01_DMARS_CH1_RID_SHIFT,
                            DMAC01_DMARS_CH1_RID);
        rza_io_reg_write_32(&DMAC.DMARS0,
                            sd_dmac_peri_req_init_table[request_factor][DMAC_REQ_MID],
                            DMAC01_DMARS_CH1_MID_SHIFT,
                            DMAC01_DMARS_CH1_MID);

        rza_io_reg_write_32(&DMAC.DMARS7,
                            1,
                            DMAC07_DCTRL_0_7_PR_SHIFT,
                            DMAC07_DCTRL_0_7_PR);
    }
}

/******************************************************************************
* Function Name: sd_DMAC1_Open
* Description  : 
* Arguments    : 
* Return Value : 
******************************************************************************/
int32_t sd_DMAC1_Open(uint32_t req)
{
    int32_t ret;
    volatile uint8_t  dummy;

    if ((0 == rza_io_reg_read_32(&DMAC.CHSTAT_1,
                                DMAC1_CHSTAT_n_EN_SHIFT,
                                DMAC1_CHSTAT_n_EN)) && 
        (0 == rza_io_reg_read_32(&DMAC.CHSTAT_1,
                                DMAC1_CHSTAT_n_TACT_SHIFT,
                                DMAC1_CHSTAT_n_TACT)))
    {
        rza_io_reg_write_32(&DMAC.CHCTRL_1,
                            1,
                            DMAC1_CHCTRL_n_SWRST_SHIFT,
                            DMAC1_CHCTRL_n_SWRST);
        dummy = rza_io_reg_read_32(&DMAC.CHCTRL_1,
                                DMAC1_CHCTRL_n_SWRST_SHIFT,
                                DMAC1_CHCTRL_n_SWRST);
        rza_io_reg_write_32(&DMAC.CHCTRL_1,
                            1,
                            DMAC1_CHCTRL_n_SETEN_SHIFT,
                            DMAC1_CHCTRL_n_SETEN);

        if (DMAC_REQ_MODE_SOFT == req)
        {
            rza_io_reg_write_32(&DMAC.CHCTRL_1,
                                1,
                                DMAC1_CHCTRL_n_STG_SHIFT,
                                DMAC1_CHCTRL_n_STG);
        }
        ret = 0;
    }
    else
    {
        ret = -1;
    }

    return ret;
}

/******************************************************************************
* Function Name: sd_DMAC1_Close
* Description  : 
* Arguments    : 
* Return Value : 
******************************************************************************/
void sd_DMAC1_Close(uint32_t * remain)
{

    rza_io_reg_write_32(&DMAC.CHCTRL_1,
                        1,
                        DMAC1_CHCTRL_n_CLREN_SHIFT,
                        DMAC1_CHCTRL_n_CLREN);

    while (1 == rza_io_reg_read_32(&DMAC.CHSTAT_1,
                                DMAC1_CHSTAT_n_TACT_SHIFT,
                                DMAC1_CHSTAT_n_TACT))
    {
        /* Wait stop */
    }

    while (1 == rza_io_reg_read_32(&DMAC.CHSTAT_1,
                                DMAC1_CHSTAT_n_EN_SHIFT,
                                DMAC1_CHSTAT_n_EN))
    {
        /* Wait stop */
    }
    *remain = DMAC.CRTB_1;
}

/******************************************************************************
* Function Name: sd_DMAC1_Load_Set
* Description  : 
* Arguments    : 
* Return Value : 
******************************************************************************/
void sd_DMAC1_Load_Set(uint32_t src_addr, uint32_t dst_addr, uint32_t count)
{
    uint8_t reg_set;

    reg_set = rza_io_reg_read_32(&DMAC.CHSTAT_1,
                                DMAC1_CHSTAT_n_SR_SHIFT,
                                DMAC1_CHSTAT_n_SR);

    if (0 == reg_set)
    {
        /* ---- Next0 Register Set ---- */
        DMAC.N0SA_1 = src_addr;
        DMAC.N0DA_1 = dst_addr;
        DMAC.N0TB_1 = count;
    }
    else
    {
        /* ---- Next1 Register Set ---- */
        DMAC.N1SA_1 = src_addr;
        DMAC.N1DA_1 = dst_addr;
        DMAC.N1TB_1 = count;
     }
}

/******************************************************************************
* Function Name: sd_DMAC1_Reload_Set
* Description  : 
* Arguments    : 
* Return Value : 
******************************************************************************/
void sd_DMAC1_Reload_Set(uint32_t src_addr, uint32_t dst_addr, uint32_t count)
{
    uint8_t reg_set;

    reg_set = rza_io_reg_read_32(&DMAC.CHSTAT_1,
                                DMAC1_CHSTAT_n_SR_SHIFT,
                                DMAC1_CHSTAT_n_SR);

    if (1 == rza_io_reg_read_32(&DMAC.CHCFG_1,
                                DMAC1_CHCFG_n_RSW_SHIFT,
                                DMAC1_CHCFG_n_RSW))
    {
        reg_set ^= 1;
    }

    if (0 == reg_set)
    {
        /* ---- Next0 Register Set ---- */
        DMAC.N0SA_1 = src_addr;
        DMAC.N0DA_1 = dst_addr;
        DMAC.N0TB_1 = count;
    }
    else
    {
        /* ---- Next1 Register Set ---- */
        DMAC.N1SA_1 = src_addr;
        DMAC.N1DA_1 = dst_addr;
        DMAC.N1TB_1 = count;
     }
}

/******************************************************************************
* Function Name: sd_DMAC1_Get_Endflag
* Description  : 
* Arguments    : 
* Return Value : 
******************************************************************************/
int32_t sd_DMAC1_Get_Endflag(void)
{
    if( rza_io_reg_read_32(&DMAC.CHSTAT_1,
                                DMAC1_CHSTAT_n_TC_SHIFT,
                                DMAC1_CHSTAT_n_TC) == 0 ){
        /* not yet */
        return 0;
    }

    /* finish */
    return 1;
}

/******************************************************************************
* Function Name: sd_DMAC1_Clear_Interrupt
* Description  : 
* Arguments    : 
* Return Value : 
******************************************************************************/
void sd_DMAC1_Clear_Interrupt(void)
{
    while ( rza_io_reg_read_32(&DMAC.CHSTAT_1,
                                DMAC1_CHSTAT_n_TC_SHIFT,
                                DMAC1_CHSTAT_n_TC) == 0 ){
        /* Wait stop */
    }

    rza_io_reg_write_32(&DMAC.CHCTRL_1,
                        1,
                        DMAC1_CHCTRL_n_CLREND_SHIFT,
                        DMAC1_CHCTRL_n_CLREND);
}








/******************************************************************************
* Function Name: sd_DMAC2_PeriReqInit
* Description  : 
* Arguments    : 
* Return Value : 
******************************************************************************/
void sd_DMAC2_PeriReqInit(const dmac_transinfo_t * trans_info, uint32_t dmamode, uint32_t continuation, 
                              uint32_t request_factor, uint32_t req_direction)
{
    if (DMAC_MODE_REGISTER == dmamode)
    {
        /* ==== Next0 register set ==== */
        DMAC.N0SA_2 = trans_info->src_addr;
        DMAC.N0DA_2 = trans_info->dst_addr;
        DMAC.N0TB_2 = trans_info->count;

        rza_io_reg_write_32(&DMAC.CHCFG_2,
                            trans_info->daddr_dir,
                            DMAC2_CHCFG_n_DAD_SHIFT,
                            DMAC2_CHCFG_n_DAD);
        rza_io_reg_write_32(&DMAC.CHCFG_2,
                            trans_info->saddr_dir,
                            DMAC2_CHCFG_n_SAD_SHIFT,
                            DMAC2_CHCFG_n_SAD);
        rza_io_reg_write_32(&DMAC.CHCFG_2,
                            trans_info->dst_size,
                            DMAC2_CHCFG_n_DDS_SHIFT,
                            DMAC2_CHCFG_n_DDS);
        rza_io_reg_write_32(&DMAC.CHCFG_2,
                            trans_info->src_size,
                            DMAC2_CHCFG_n_SDS_SHIFT,
                            DMAC2_CHCFG_n_SDS);

        rza_io_reg_write_32(&DMAC.CHCFG_2,
                            0,
                            DMAC2_CHCFG_n_DMS_SHIFT,
                            DMAC2_CHCFG_n_DMS);
        rza_io_reg_write_32(&DMAC.CHCFG_2,
                            0,
                            DMAC2_CHCFG_n_RSEL_SHIFT,
                            DMAC2_CHCFG_n_RSEL);
        rza_io_reg_write_32(&DMAC.CHCFG_2,
                            0,
                            DMAC2_CHCFG_n_SBE_SHIFT,
                            DMAC2_CHCFG_n_SBE);
        rza_io_reg_write_32(&DMAC.CHCFG_2,
                            0,
                            DMAC2_CHCFG_n_DEM_SHIFT,
                            DMAC2_CHCFG_n_DEM);

        if (DMAC_SAMPLE_CONTINUATION == continuation)
        {
            rza_io_reg_write_32(&DMAC.CHCFG_2,
                                1,
                                DMAC2_CHCFG_n_REN_SHIFT,
                                DMAC2_CHCFG_n_REN);
            rza_io_reg_write_32(&DMAC.CHCFG_2,
                                1,
                                DMAC2_CHCFG_n_RSW_SHIFT,
                                DMAC2_CHCFG_n_RSW);
        }
        else
        {
            rza_io_reg_write_32(&DMAC.CHCFG_2,
                                0,
                                DMAC2_CHCFG_n_REN_SHIFT,
                                DMAC2_CHCFG_n_REN);
            rza_io_reg_write_32(&DMAC.CHCFG_2,
                                0,
                                DMAC2_CHCFG_n_RSW_SHIFT,
                                DMAC2_CHCFG_n_RSW);
        }

        rza_io_reg_write_32(&DMAC.CHCFG_2,
                            0,
                            DMAC2_CHCFG_n_TM_SHIFT,
                            DMAC2_CHCFG_n_TM);
        rza_io_reg_write_32(&DMAC.CHCFG_2,
                            2,
                            DMAC2_CHCFG_n_SEL_SHIFT,
                            DMAC2_CHCFG_n_SEL);
        rza_io_reg_write_32(&DMAC.CHCFG_2,
                            1,
                            DMAC2_CHCFG_n_HIEN_SHIFT,
                            DMAC2_CHCFG_n_HIEN);
        rza_io_reg_write_32(&DMAC.CHCFG_2,
                            0,
                            DMAC2_CHCFG_n_LOEN_SHIFT,
                            DMAC2_CHCFG_n_LOEN);

        rza_io_reg_write_32(&DMAC.CHCFG_2,
                            sd_dmac_peri_req_init_table[request_factor][DMAC_REQ_AM],
                            DMAC2_CHCFG_n_AM_SHIFT,
                            DMAC2_CHCFG_n_AM);
        rza_io_reg_write_32(&DMAC.CHCFG_2,
                            sd_dmac_peri_req_init_table[request_factor][DMAC_REQ_LVL],
                            DMAC2_CHCFG_n_LVL_SHIFT,
                            DMAC2_CHCFG_n_LVL);
        if (sd_dmac_peri_req_init_table[request_factor][DMAC_REQ_REQD] != DMAC_INDEFINE)
        {
            rza_io_reg_write_32(&DMAC.CHCFG_2,
                                sd_dmac_peri_req_init_table[request_factor][DMAC_REQ_REQD],
                                DMAC2_CHCFG_n_REQD_SHIFT,
                                DMAC2_CHCFG_n_REQD);
        }
        else
        {
            rza_io_reg_write_32(&DMAC.CHCFG_2,
                                req_direction,
                                DMAC2_CHCFG_n_REQD_SHIFT,
                                DMAC2_CHCFG_n_REQD);
        }
        rza_io_reg_write_32(&DMAC.DMARS1,
                            sd_dmac_peri_req_init_table[request_factor][DMAC_REQ_RID],
                            DMAC23_DMARS_CH2_RID_SHIFT,
                            DMAC23_DMARS_CH2_RID);
        rza_io_reg_write_32(&DMAC.DMARS1,
                            sd_dmac_peri_req_init_table[request_factor][DMAC_REQ_MID],
                            DMAC23_DMARS_CH2_MID_SHIFT,
                            DMAC23_DMARS_CH2_MID);

        rza_io_reg_write_32(&DMAC.DMARS7,
                            1,
                            DMAC07_DCTRL_0_7_PR_SHIFT,
                            DMAC07_DCTRL_0_7_PR);
    }
}

/******************************************************************************
* Function Name: sd_DMAC2_Open
* Description  : 
* Arguments    : 
* Return Value : 
******************************************************************************/
int32_t sd_DMAC2_Open(uint32_t req)
{
    int32_t ret;
    volatile uint8_t  dummy;

    if ((0 == rza_io_reg_read_32(&DMAC.CHSTAT_2,
                                DMAC2_CHSTAT_n_EN_SHIFT,
                                DMAC2_CHSTAT_n_EN)) && 
        (0 == rza_io_reg_read_32(&DMAC.CHSTAT_2,
                                DMAC2_CHSTAT_n_TACT_SHIFT,
                                DMAC2_CHSTAT_n_TACT)))
    {
        rza_io_reg_write_32(&DMAC.CHCTRL_2,
                            1,
                            DMAC2_CHCTRL_n_SWRST_SHIFT,
                            DMAC2_CHCTRL_n_SWRST);
        dummy = rza_io_reg_read_32(&DMAC.CHCTRL_2,
                                DMAC2_CHCTRL_n_SWRST_SHIFT,
                                DMAC2_CHCTRL_n_SWRST);
        rza_io_reg_write_32(&DMAC.CHCTRL_2,
                            1,
                            DMAC2_CHCTRL_n_SETEN_SHIFT,
                            DMAC2_CHCTRL_n_SETEN);

        if (DMAC_REQ_MODE_SOFT == req)
        {
            rza_io_reg_write_32(&DMAC.CHCTRL_2,
                                1,
                                DMAC2_CHCTRL_n_STG_SHIFT,
                                DMAC2_CHCTRL_n_STG);
        }

        ret = 0;
    }
    else
    {
        ret = -1;
    }

    return ret;
}

/******************************************************************************
* Function Name: sd_DMAC2_Close
* Description  : 
* Arguments    : 
* Return Value : 
******************************************************************************/
void sd_DMAC2_Close(uint32_t * remain)
{

    rza_io_reg_write_32(&DMAC.CHCTRL_2,
                        1,
                        DMAC2_CHCTRL_n_CLREN_SHIFT,
                        DMAC2_CHCTRL_n_CLREN);

    while (1 == rza_io_reg_read_32(&DMAC.CHSTAT_2,
                                DMAC2_CHSTAT_n_TACT_SHIFT,
                                DMAC2_CHSTAT_n_TACT))
    {
        /* Wait stop */
    }

    while (1 == rza_io_reg_read_32(&DMAC.CHSTAT_2,
                                DMAC2_CHSTAT_n_EN_SHIFT,
                                DMAC2_CHSTAT_n_EN))
    {
        /* Wait stop */
    }
    *remain = DMAC.CRTB_2;
}

/******************************************************************************
* Function Name: sd_DMAC2_Load_Set
* Description  : 
* Arguments    : 
* Return Value : 
******************************************************************************/
void sd_DMAC2_Load_Set(uint32_t src_addr, uint32_t dst_addr, uint32_t count)
{
    uint8_t reg_set;

    reg_set = rza_io_reg_read_32(&DMAC.CHSTAT_2,
                                DMAC2_CHSTAT_n_SR_SHIFT,
                                DMAC2_CHSTAT_n_SR);

    if (0 == reg_set)
    {
        /* ---- Next0 Register Set ---- */
        DMAC.N0SA_2 = src_addr;
        DMAC.N0DA_2 = dst_addr;
        DMAC.N0TB_2 = count;
    }
    else
    {
        /* ---- Next1 Register Set ---- */
        DMAC.N1SA_2 = src_addr;
        DMAC.N1DA_2 = dst_addr;
        DMAC.N1TB_2 = count;
     }
}

/******************************************************************************
* Function Name: sd_DMAC2_Reload_Set
* Description  : 
* Arguments    : 
* Return Value : 
******************************************************************************/
void sd_DMAC2_Reload_Set(uint32_t src_addr, uint32_t dst_addr, uint32_t count)
{
    uint8_t reg_set;

    reg_set = rza_io_reg_read_32(&DMAC.CHSTAT_2,
                                DMAC2_CHSTAT_n_SR_SHIFT,
                                DMAC2_CHSTAT_n_SR);

    if (1 == rza_io_reg_read_32(&DMAC.CHCFG_2,
                                DMAC2_CHCFG_n_RSW_SHIFT,
                                DMAC2_CHCFG_n_RSW))
    {
        reg_set ^= 1;
    }

    if (0 == reg_set)
    {
        /* ---- Next0 Register Set ---- */
        DMAC.N0SA_2 = src_addr;
        DMAC.N0DA_2 = dst_addr;
        DMAC.N0TB_2 = count;
    }
    else
    {
        /* ---- Next1 Register Set ---- */
        DMAC.N1SA_2 = src_addr;
        DMAC.N1DA_2 = dst_addr;
        DMAC.N1TB_2 = count;
     }
}

/******************************************************************************
* Function Name: sd_DMAC2_Get_Endflag
* Description  : 
* Arguments    : 
* Return Value : 
******************************************************************************/
int32_t sd_DMAC2_Get_Endflag(void)
{
    if( rza_io_reg_read_32(&DMAC.CHSTAT_2,
                                DMAC2_CHSTAT_n_TC_SHIFT,
                                DMAC2_CHSTAT_n_TC) == 0 ){
        /* not yet */
        return 0;
    }

    /* finish */
    return 1;
}

/******************************************************************************
* Function Name: sd_DMAC2_Clear_Interrupt
* Description  : 
* Arguments    : 
* Return Value : 
******************************************************************************/
void sd_DMAC2_Clear_Interrupt(void)
{
    while ( rza_io_reg_read_32(&DMAC.CHSTAT_2,
                                DMAC2_CHSTAT_n_TC_SHIFT,
                                DMAC2_CHSTAT_n_TC) == 0 ){
        /* wait stop */
    }

    rza_io_reg_write_32(&DMAC.CHCTRL_2,
                        1,
                        DMAC2_CHCTRL_n_CLREND_SHIFT,
                        DMAC2_CHCTRL_n_CLREND);
}

/* End of File */

