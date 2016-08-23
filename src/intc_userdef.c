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
* File Name     : intc_userdef.c
* Device(s)     : RZ/A1H (R7S721001)
* Tool-Chain    : GNUARM-NONEv14.02-EABI
* H/W Platform  : RSK+RZA1H CPU Board
* Description   : Interrupt controller use file
*******************************************************************************/
/*******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 21.10.2014 1.00
*******************************************************************************/

/*******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
/* Default  type definition header */
#include "typedefine.h"
/* definition of I/O register */
#include "iodefine.h"
/* INTC Driver Header */
#include "intc.h"
/* RSK user switches header */
//#include "rsk_switches.h"
/* Interchangeable compiler specific header */
//#include "compiler_settings.h"

/*******************************************************************************
Typedef definitions
*******************************************************************************/


/*******************************************************************************
Macro definitions
*******************************************************************************/
/* Defines the NMI bit in the user switch variable */
#define NMIF_BIT	(0x0002)

/*******************************************************************************
Private global variables and functions
*******************************************************************************/
/* ====  Prototype function ==== */
static void userdef_intc_dummy_interrupt (uint32_t int_sense);

/* ==== Global variable ==== */
/* Interrupt handler table */
static void (* intc_func_table[INTC_ID_TOTAL])(uint32_t int_sense) =
{
    userdef_intc_dummy_interrupt,        /* 0   : SW0           */
    userdef_intc_dummy_interrupt,        /* 1   : SW1           */
    userdef_intc_dummy_interrupt,        /* 2   : SW2           */
    userdef_intc_dummy_interrupt,        /* 3   : SW3           */
    userdef_intc_dummy_interrupt,        /* 4   : SW4           */
    userdef_intc_dummy_interrupt,        /* 5   : SW5           */
    userdef_intc_dummy_interrupt,        /* 6   : SW6           */
    userdef_intc_dummy_interrupt,        /* 7   : SW7           */
    userdef_intc_dummy_interrupt,        /* 8   : SW8           */
    userdef_intc_dummy_interrupt,        /* 9   : SW9           */
    userdef_intc_dummy_interrupt,        /* 10  : SW10          */
    userdef_intc_dummy_interrupt,        /* 11  : SW11          */
    userdef_intc_dummy_interrupt,        /* 12  : SW12          */
    userdef_intc_dummy_interrupt,        /* 13  : SW13          */
    userdef_intc_dummy_interrupt,        /* 14  : SW14          */
    userdef_intc_dummy_interrupt,        /* 15  : SW15          */
    userdef_intc_dummy_interrupt,        /* 16  : PMUIRQ0       */
    userdef_intc_dummy_interrupt,        /* 17  : COMMRX0       */
    userdef_intc_dummy_interrupt,        /* 18  : COMMTX0       */
    userdef_intc_dummy_interrupt,        /* 19  : CTIIRQ0       */
    userdef_intc_dummy_interrupt,        /* 20  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 21  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 22  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 23  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 24  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 25  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 26  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 27  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 28  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 29  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 30  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 31  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 32  : IRQ0          */
    userdef_intc_dummy_interrupt,        /* 33  : IRQ1          */
    userdef_intc_dummy_interrupt,        /* 34  : IRQ2          */
    userdef_intc_dummy_interrupt,        /* 35  : IRQ3          */
    userdef_intc_dummy_interrupt,        /* 36  : IRQ4          */
    userdef_intc_dummy_interrupt,        /* 37  : IRQ5          */
    userdef_intc_dummy_interrupt,        /* 38  : IRQ6          */
    userdef_intc_dummy_interrupt,        /* 39  : IRQ7          */
    userdef_intc_dummy_interrupt,        /* 40  : PL310ERR      */
    userdef_intc_dummy_interrupt,        /* 41  : DMAINT0       */
    userdef_intc_dummy_interrupt,        /* 42  : DMAINT1       */
    userdef_intc_dummy_interrupt,        /* 43  : DMAINT2       */
    userdef_intc_dummy_interrupt,        /* 44  : DMAINT3       */
    userdef_intc_dummy_interrupt,        /* 45  : DMAINT4       */
    userdef_intc_dummy_interrupt,        /* 46  : DMAINT5       */
    userdef_intc_dummy_interrupt,        /* 47  : DMAINT6       */
    userdef_intc_dummy_interrupt,        /* 48  : DMAINT7       */
    userdef_intc_dummy_interrupt,        /* 49  : DMAINT8       */
    userdef_intc_dummy_interrupt,        /* 50  : DMAINT9       */
    userdef_intc_dummy_interrupt,        /* 51  : DMAINT10      */
    userdef_intc_dummy_interrupt,        /* 52  : DMAINT11      */
    userdef_intc_dummy_interrupt,        /* 53  : DMAINT12      */
    userdef_intc_dummy_interrupt,        /* 54  : DMAINT13      */
    userdef_intc_dummy_interrupt,        /* 55  : DMAINT14      */
    userdef_intc_dummy_interrupt,        /* 56  : DMAINT15      */
    userdef_intc_dummy_interrupt,        /* 57  : DMAERR        */
    userdef_intc_dummy_interrupt,        /* 58  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 59  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 60  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 61  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 62  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 63  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 64  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 65  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 66  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 67  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 68  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 69  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 70  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 71  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 72  : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 73  : USBI0         */
    userdef_intc_dummy_interrupt,        /* 74  : USBI1         */
    userdef_intc_dummy_interrupt,        /* 75  : S0_VI_VSYNC0  */
    userdef_intc_dummy_interrupt,        /* 76  : S0_LO_VSYNC0  */
    userdef_intc_dummy_interrupt,        /* 77  : S0_VSYNCERR0  */
    userdef_intc_dummy_interrupt,        /* 78  : GR3_VLINE0    */
    userdef_intc_dummy_interrupt,        /* 79  : S0_VFIELD0    */
    userdef_intc_dummy_interrupt,        /* 80  : IV1_VBUFERR0  */
    userdef_intc_dummy_interrupt,        /* 81  : IV3_VBUFERR0  */
    userdef_intc_dummy_interrupt,        /* 82  : IV5_VBUFERR0  */
    userdef_intc_dummy_interrupt,        /* 83  : IV6_VBUFERR0  */
    userdef_intc_dummy_interrupt,        /* 84  : S0_WLINE0     */
    userdef_intc_dummy_interrupt,        /* 85  : S1_VI_VSYNC0  */
    userdef_intc_dummy_interrupt,        /* 86  : S1_LO_VSYNC0  */
    userdef_intc_dummy_interrupt,        /* 87  : S1_VSYNCERR0  */
    userdef_intc_dummy_interrupt,        /* 88  : S1_VFIELD0    */
    userdef_intc_dummy_interrupt,        /* 89  : IV2_VBUFERR0  */
    userdef_intc_dummy_interrupt,        /* 90  : IV4_VBUFERR0  */
    userdef_intc_dummy_interrupt,        /* 91  : S1_WLINE0     */
    userdef_intc_dummy_interrupt,        /* 92  : OIR_VI_VSYNC0 */
    userdef_intc_dummy_interrupt,        /* 93  : OIR_LO_VSYNC0 */
    userdef_intc_dummy_interrupt,        /* 94  : OIR_VSYNCERR0 */
    userdef_intc_dummy_interrupt,        /* 95  : OIR_VFIELD0   */
    userdef_intc_dummy_interrupt,        /* 96  : IV7_VBUFERR0  */
    userdef_intc_dummy_interrupt,        /* 97  : IV8_VBUFERR0  */
    userdef_intc_dummy_interrupt,        /* 98  : OIR_WLINE0    */
    userdef_intc_dummy_interrupt,        /* 99  : S0_VI_VSYNC1  */
    userdef_intc_dummy_interrupt,        /* 100 : S0_LO_VSYNC1  */
    userdef_intc_dummy_interrupt,        /* 101 : S0_VSYNCERR1  */
    userdef_intc_dummy_interrupt,        /* 102 : GR3_VLINE1    */
    userdef_intc_dummy_interrupt,        /* 103 : S0_VFIELD1    */
    userdef_intc_dummy_interrupt,        /* 104 : IV1_VBUFERR1  */
    userdef_intc_dummy_interrupt,        /* 105 : IV3_VBUFERR1  */
    userdef_intc_dummy_interrupt,        /* 106 : IV5_VBUFERR1  */
    userdef_intc_dummy_interrupt,        /* 107 : IV6_VBUFERR1  */
    userdef_intc_dummy_interrupt,        /* 108 : S0_WLINE1     */
    userdef_intc_dummy_interrupt,        /* 109 : S1_VI_VSYNC1  */
    userdef_intc_dummy_interrupt,        /* 110 : S1_LO_VSYNC1  */
    userdef_intc_dummy_interrupt,        /* 111 : S1_VSYNCERR1  */
    userdef_intc_dummy_interrupt,        /* 112 : S1_VFIELD1    */
    userdef_intc_dummy_interrupt,        /* 113 : IV2_VBUFERR1  */
    userdef_intc_dummy_interrupt,        /* 114 : IV4_VBUFERR1  */
    userdef_intc_dummy_interrupt,        /* 115 : S1_WLINE1     */
    userdef_intc_dummy_interrupt,        /* 116 : OIR_VI_VSYNC1 */
    userdef_intc_dummy_interrupt,        /* 117 : OIR_LO_VSYNC1 */
    userdef_intc_dummy_interrupt,        /* 118 : OIR_VLINE1    */
    userdef_intc_dummy_interrupt,        /* 119 : OIR_VFIELD1   */
    userdef_intc_dummy_interrupt,        /* 120 : IV7_VBUFERR1  */
    userdef_intc_dummy_interrupt,        /* 121 : IV8_VBUFERR1  */
    userdef_intc_dummy_interrupt,        /* 122 : OIR_WLINE1    */
    userdef_intc_dummy_interrupt,        /* 123 : IMRDI         */
    userdef_intc_dummy_interrupt,        /* 124 : IMR2I0        */
    userdef_intc_dummy_interrupt,        /* 125 : IMR2I1        */
    userdef_intc_dummy_interrupt,        /* 126 : JEDI          */
    userdef_intc_dummy_interrupt,        /* 127 : JDTI          */
    userdef_intc_dummy_interrupt,        /* 128 : CMP0          */
    userdef_intc_dummy_interrupt,        /* 129 : CMP1          */
    userdef_intc_dummy_interrupt,        /* 130 : INT0          */
    userdef_intc_dummy_interrupt,        /* 131 : INT1          */
    userdef_intc_dummy_interrupt,        /* 132 : INT2          */
    userdef_intc_dummy_interrupt,        /* 133 : INT3          */
    userdef_intc_dummy_interrupt,        /* 134 : OSTM0TINT     */
    userdef_intc_dummy_interrupt,        /* 135 : OSTM1TINT     */
    userdef_intc_dummy_interrupt,        /* 136 : CMI           */
    userdef_intc_dummy_interrupt,        /* 137 : WTOUT         */
    userdef_intc_dummy_interrupt,        /* 138 : ITI           */
    userdef_intc_dummy_interrupt,        /* 139 : TGI0A         */
    userdef_intc_dummy_interrupt,        /* 140 : TGI0B         */
    userdef_intc_dummy_interrupt,        /* 141 : TGI0C         */
    userdef_intc_dummy_interrupt,        /* 142 : TGI0D         */
    userdef_intc_dummy_interrupt,        /* 143 : TGI0V         */
    userdef_intc_dummy_interrupt,        /* 144 : TGI0E         */
    userdef_intc_dummy_interrupt,        /* 145 : TGI0F         */
    userdef_intc_dummy_interrupt,        /* 146 : TGI1A         */
    userdef_intc_dummy_interrupt,        /* 147 : TGI1B         */
    userdef_intc_dummy_interrupt,        /* 148 : TGI1V         */
    userdef_intc_dummy_interrupt,        /* 149 : TGI1U         */
    userdef_intc_dummy_interrupt,        /* 150 : TGI2A         */
    userdef_intc_dummy_interrupt,        /* 151 : TGI2B         */
    userdef_intc_dummy_interrupt,        /* 152 : TGI2V         */
    userdef_intc_dummy_interrupt,        /* 153 : TGI2U         */
    userdef_intc_dummy_interrupt,        /* 154 : TGI3A         */
    userdef_intc_dummy_interrupt,        /* 155 : TGI3B         */
    userdef_intc_dummy_interrupt,        /* 156 : TGI3C         */
    userdef_intc_dummy_interrupt,        /* 157 : TGI3D         */
    userdef_intc_dummy_interrupt,        /* 158 : TGI3V         */
    userdef_intc_dummy_interrupt,        /* 159 : TGI4A         */
    userdef_intc_dummy_interrupt,        /* 160 : TGI4B         */
    userdef_intc_dummy_interrupt,        /* 161 : TGI4C         */
    userdef_intc_dummy_interrupt,        /* 162 : TGI4D         */
    userdef_intc_dummy_interrupt,        /* 163 : TGI4V         */
    userdef_intc_dummy_interrupt,        /* 164 : CMI1          */
    userdef_intc_dummy_interrupt,        /* 165 : CMI2          */
    userdef_intc_dummy_interrupt,        /* 166 : SGDEI0        */
    userdef_intc_dummy_interrupt,        /* 167 : SGDEI1        */
    userdef_intc_dummy_interrupt,        /* 168 : SGDEI2        */
    userdef_intc_dummy_interrupt,        /* 169 : SGDEI3        */
    userdef_intc_dummy_interrupt,        /* 170 : ADI           */
    userdef_intc_dummy_interrupt,        /* 171 : LMTI          */
    userdef_intc_dummy_interrupt,        /* 172 : SSII0         */
    userdef_intc_dummy_interrupt,        /* 173 : SSIRXI0       */
    userdef_intc_dummy_interrupt,        /* 174 : SSITXI0       */
    userdef_intc_dummy_interrupt,        /* 175 : SSII1         */
    userdef_intc_dummy_interrupt,        /* 176 : SSIRXI1       */
    userdef_intc_dummy_interrupt,        /* 177 : SSITXI1       */
    userdef_intc_dummy_interrupt,        /* 178 : SSII2         */
    userdef_intc_dummy_interrupt,        /* 179 : SSIRTI2       */
    userdef_intc_dummy_interrupt,        /* 180 : SSII3         */
    userdef_intc_dummy_interrupt,        /* 181 : SSIRXI3       */
    userdef_intc_dummy_interrupt,        /* 182 : SSITXI3       */
    userdef_intc_dummy_interrupt,        /* 183 : SSII4         */
    userdef_intc_dummy_interrupt,        /* 184 : SSIRTI4       */
    userdef_intc_dummy_interrupt,        /* 185 : SSII5         */
    userdef_intc_dummy_interrupt,        /* 186 : SSIRXI5       */
    userdef_intc_dummy_interrupt,        /* 187 : SSITXI5       */
    userdef_intc_dummy_interrupt,        /* 188 : SPDIFI        */
    userdef_intc_dummy_interrupt,        /* 189 : INTIICTEI0    */
    userdef_intc_dummy_interrupt,        /* 190 : INTIICRI0     */
    userdef_intc_dummy_interrupt,        /* 191 : INTIICTI0     */
    userdef_intc_dummy_interrupt,        /* 192 : INTIICSPI0    */
    userdef_intc_dummy_interrupt,        /* 193 : INTIICSTI0    */
    userdef_intc_dummy_interrupt,        /* 194 : INTIICNAKI0   */
    userdef_intc_dummy_interrupt,        /* 195 : INTIICALI0    */
    userdef_intc_dummy_interrupt,        /* 196 : INTIICTMOI0   */
    userdef_intc_dummy_interrupt,        /* 197 : INTIICTEI1    */
    userdef_intc_dummy_interrupt,        /* 198 : INTIICRI1     */
    userdef_intc_dummy_interrupt,        /* 199 : INTIICTI1     */
    userdef_intc_dummy_interrupt,        /* 200 : INTIICSPI1    */
    userdef_intc_dummy_interrupt,        /* 201 : INTIICSTI1    */
    userdef_intc_dummy_interrupt,        /* 202 : INTIICNAKI1   */
    userdef_intc_dummy_interrupt,        /* 203 : INTIICALI1    */
    userdef_intc_dummy_interrupt,        /* 204 : INTIICTMOI1   */
    userdef_intc_dummy_interrupt,        /* 205 : INTIICTEI2    */
    userdef_intc_dummy_interrupt,        /* 206 : INTIICRI2     */
    userdef_intc_dummy_interrupt,        /* 207 : INTIICTI2     */
    userdef_intc_dummy_interrupt,        /* 208 : INTIICSPI2    */
    userdef_intc_dummy_interrupt,        /* 209 : INTIICSTI2    */
    userdef_intc_dummy_interrupt,        /* 210 : INTIICNAKI2   */
    userdef_intc_dummy_interrupt,        /* 211 : INTIICALI2    */
    userdef_intc_dummy_interrupt,        /* 212 : INTIICTMOI2   */
    userdef_intc_dummy_interrupt,        /* 213 : INTIICTEI3    */
    userdef_intc_dummy_interrupt,        /* 214 : INTIICRI3     */
    userdef_intc_dummy_interrupt,        /* 215 : INTIICTI3     */
    userdef_intc_dummy_interrupt,        /* 216 : INTIICSPI3    */
    userdef_intc_dummy_interrupt,        /* 217 : INTIICSTI3    */
    userdef_intc_dummy_interrupt,        /* 218 : INTIICNAKI3   */
    userdef_intc_dummy_interrupt,        /* 219 : INTIICALI3    */
    userdef_intc_dummy_interrupt,        /* 220 : INTIICTMOI3   */
    userdef_intc_dummy_interrupt,        /* 221 : BRI0          */
    userdef_intc_dummy_interrupt,        /* 222 : ERI0          */
    userdef_intc_dummy_interrupt,        /* 223 : RXI0          */
    userdef_intc_dummy_interrupt,        /* 224 : TXI0          */
    userdef_intc_dummy_interrupt,        /* 225 : BRI1          */
    userdef_intc_dummy_interrupt,        /* 226 : ERI1          */
    userdef_intc_dummy_interrupt,        /* 227 : RXI1          */
    userdef_intc_dummy_interrupt,        /* 228 : TXI1          */
    userdef_intc_dummy_interrupt,        /* 229 : BRI2          */
    userdef_intc_dummy_interrupt,        /* 230 : ERI2          */
    userdef_intc_dummy_interrupt,        /* 231 : RXI2          */
    userdef_intc_dummy_interrupt,        /* 232 : TXI2          */
    userdef_intc_dummy_interrupt,        /* 233 : BRI3          */
    userdef_intc_dummy_interrupt,        /* 234 : ERI3          */
    userdef_intc_dummy_interrupt,        /* 235 : RXI3          */
    userdef_intc_dummy_interrupt,        /* 236 : TXI3          */
    userdef_intc_dummy_interrupt,        /* 237 : BRI4          */
    userdef_intc_dummy_interrupt,        /* 238 : ERI4          */
    userdef_intc_dummy_interrupt,        /* 239 : RXI4          */
    userdef_intc_dummy_interrupt,        /* 240 : TXI4          */
    userdef_intc_dummy_interrupt,        /* 241 : BRI5          */
    userdef_intc_dummy_interrupt,        /* 242 : ERI5          */
    userdef_intc_dummy_interrupt,        /* 243 : RXI5          */
    userdef_intc_dummy_interrupt,        /* 244 : TXI5          */
    userdef_intc_dummy_interrupt,        /* 245 : BRI6          */
    userdef_intc_dummy_interrupt,        /* 246 : ERI6          */
    userdef_intc_dummy_interrupt,        /* 247 : RXI6          */
    userdef_intc_dummy_interrupt,        /* 248 : TXI6          */
    userdef_intc_dummy_interrupt,        /* 249 : BRI7          */
    userdef_intc_dummy_interrupt,        /* 250 : ERI7          */
    userdef_intc_dummy_interrupt,        /* 251 : RXI7          */
    userdef_intc_dummy_interrupt,        /* 252 : TXI7          */
    userdef_intc_dummy_interrupt,        /* 253 : INTRCANGERR   */
    userdef_intc_dummy_interrupt,        /* 254 : INTRCANGRECC  */
    userdef_intc_dummy_interrupt,        /* 255 : INTRCAN0REC   */
    userdef_intc_dummy_interrupt,        /* 256 : INTRCAN0ERR   */
    userdef_intc_dummy_interrupt,        /* 257 : INTRCAN0TRX   */
    userdef_intc_dummy_interrupt,        /* 258 : INTRCAN1REC   */
    userdef_intc_dummy_interrupt,        /* 259 : INTRCAN1ERR   */
    userdef_intc_dummy_interrupt,        /* 260 : INTRCAN1TRX   */
    userdef_intc_dummy_interrupt,        /* 261 : INTRCAN2REC   */
    userdef_intc_dummy_interrupt,        /* 262 : INTRCAN2ERR   */
    userdef_intc_dummy_interrupt,        /* 263 : INTRCAN2TRX   */
    userdef_intc_dummy_interrupt,        /* 264 : INTRCAN3REC   */
    userdef_intc_dummy_interrupt,        /* 265 : INTRCAN3ERR   */
    userdef_intc_dummy_interrupt,        /* 266 : INTRCAN3TRX   */
    userdef_intc_dummy_interrupt,        /* 267 : INTRCAN4REC   */
    userdef_intc_dummy_interrupt,        /* 268 : INTRCAN4ERR   */
    userdef_intc_dummy_interrupt,        /* 269 : INTRCAN4TRX   */
    userdef_intc_dummy_interrupt,        /* 270 : SPEI0         */
    userdef_intc_dummy_interrupt,        /* 271 : SPRI0         */
    userdef_intc_dummy_interrupt,        /* 272 : SPTI0         */
    userdef_intc_dummy_interrupt,        /* 273 : SPEI1         */
    userdef_intc_dummy_interrupt,        /* 274 : SPRI1         */
    userdef_intc_dummy_interrupt,        /* 275 : SPTI1         */
    userdef_intc_dummy_interrupt,        /* 276 : SPEI2         */
    userdef_intc_dummy_interrupt,        /* 277 : SPRI2         */
    userdef_intc_dummy_interrupt,        /* 278 : SPTI2         */
    userdef_intc_dummy_interrupt,        /* 279 : SPEI3         */
    userdef_intc_dummy_interrupt,        /* 280 : SPRI3         */
    userdef_intc_dummy_interrupt,        /* 281 : SPTI3         */
    userdef_intc_dummy_interrupt,        /* 282 : SPEI4         */
    userdef_intc_dummy_interrupt,        /* 283 : SPRI4         */
    userdef_intc_dummy_interrupt,        /* 284 : SPTI4         */
    userdef_intc_dummy_interrupt,        /* 285 : IEBBTD        */
    userdef_intc_dummy_interrupt,        /* 286 : IEBBTERR      */
    userdef_intc_dummy_interrupt,        /* 287 : IEBBTSTA      */
    userdef_intc_dummy_interrupt,        /* 288 : IEBBTV        */
    userdef_intc_dummy_interrupt,        /* 289 : ISY           */
    userdef_intc_dummy_interrupt,        /* 290 : IERR          */
    userdef_intc_dummy_interrupt,        /* 291 : ITARG         */
    userdef_intc_dummy_interrupt,        /* 292 : ISEC          */
    userdef_intc_dummy_interrupt,        /* 293 : IBUF          */
    userdef_intc_dummy_interrupt,        /* 294 : IREADY        */
    userdef_intc_dummy_interrupt,        /* 295 : FLSTE         */
    userdef_intc_dummy_interrupt,        /* 296 : FLTENDI       */
    userdef_intc_dummy_interrupt,        /* 297 : FLTREQ0I      */
    userdef_intc_dummy_interrupt,        /* 298 : FLTREQ1I      */
    userdef_intc_dummy_interrupt,        /* 299 : MMC0          */
    userdef_intc_dummy_interrupt,        /* 300 : MMC1          */
    userdef_intc_dummy_interrupt,        /* 301 : MMC2          */
    userdef_intc_dummy_interrupt,        /* 302 : SDHI0_3       */
    userdef_intc_dummy_interrupt,        /* 303 : SDHI0_0       */
    userdef_intc_dummy_interrupt,        /* 304 : SDHI0_1       */
    userdef_intc_dummy_interrupt,        /* 305 : SDHI1_3       */
    userdef_intc_dummy_interrupt,        /* 306 : SDHI1_0       */
    userdef_intc_dummy_interrupt,        /* 307 : SDHI1_1       */
    userdef_intc_dummy_interrupt,        /* 308 : ARM           */
    userdef_intc_dummy_interrupt,        /* 309 : PRD           */
    userdef_intc_dummy_interrupt,        /* 310 : CUP           */
    userdef_intc_dummy_interrupt,        /* 311 : SCUAI0        */
    userdef_intc_dummy_interrupt,        /* 312 : SCUAI1        */
    userdef_intc_dummy_interrupt,        /* 313 : SCUFDI0       */
    userdef_intc_dummy_interrupt,        /* 314 : SCUFDI1       */
    userdef_intc_dummy_interrupt,        /* 315 : SCUFDI2       */
    userdef_intc_dummy_interrupt,        /* 316 : SCUFDI3       */
    userdef_intc_dummy_interrupt,        /* 317 : SCUFUI0       */
    userdef_intc_dummy_interrupt,        /* 318 : SCUFUI1       */
    userdef_intc_dummy_interrupt,        /* 319 : SCUFUI2       */
    userdef_intc_dummy_interrupt,        /* 320 : SCUFUI3       */
    userdef_intc_dummy_interrupt,        /* 321 : SCUDVI0       */
    userdef_intc_dummy_interrupt,        /* 322 : SCUDVI1       */
    userdef_intc_dummy_interrupt,        /* 323 : SCUDVI2       */
    userdef_intc_dummy_interrupt,        /* 324 : SCUDVI3       */
    userdef_intc_dummy_interrupt,        /* 325 : MLB_CINT      */
    userdef_intc_dummy_interrupt,        /* 326 : MLB_SINT      */
    userdef_intc_dummy_interrupt,        /* 327 : DRC0          */
    userdef_intc_dummy_interrupt,        /* 328 : DRC1          */
    userdef_intc_dummy_interrupt,        /* 329 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 330 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 331 : LINI0_INT_T   */
    userdef_intc_dummy_interrupt,        /* 332 : LINI0_INT_R   */
    userdef_intc_dummy_interrupt,        /* 333 : LINI0_INT_S   */
    userdef_intc_dummy_interrupt,        /* 334 : LINI0_INT_M   */
    userdef_intc_dummy_interrupt,        /* 335 : LINI1_INT_T   */
    userdef_intc_dummy_interrupt,        /* 336 : LINI1_INT_R   */
    userdef_intc_dummy_interrupt,        /* 337 : LINI1_INT_S   */
    userdef_intc_dummy_interrupt,        /* 338 : LINI1_INT_M   */
    userdef_intc_dummy_interrupt,        /* 339 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 340 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 341 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 342 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 343 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 344 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 345 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 346 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 347 : ERI0          */
    userdef_intc_dummy_interrupt,        /* 348 : RXI0          */
    userdef_intc_dummy_interrupt,        /* 349 : TXI0          */
    userdef_intc_dummy_interrupt,        /* 350 : TEI0          */
    userdef_intc_dummy_interrupt,        /* 351 : ERI1          */
    userdef_intc_dummy_interrupt,        /* 352 : RXI1          */
    userdef_intc_dummy_interrupt,        /* 353 : TXI1          */
    userdef_intc_dummy_interrupt,        /* 354 : TEI1          */
    userdef_intc_dummy_interrupt,        /* 355 : AVBI_DATA     */
    userdef_intc_dummy_interrupt,        /* 356 : AVBI_ERROR    */
    userdef_intc_dummy_interrupt,        /* 357 : AVBI_MANAGE   */
    userdef_intc_dummy_interrupt,        /* 358 : AVBI_MAC      */
    userdef_intc_dummy_interrupt,        /* 359 : ETHERI        */
    userdef_intc_dummy_interrupt,        /* 360 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 361 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 362 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 363 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 364 : CEUI          */
    userdef_intc_dummy_interrupt,        /* 365 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 366 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 367 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 368 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 369 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 370 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 371 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 372 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 373 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 374 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 375 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 376 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 377 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 378 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 379 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 380 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 381 : H2XMLB_ERRINT */
    userdef_intc_dummy_interrupt,        /* 382 : H2XIC1_ERRINT */
    userdef_intc_dummy_interrupt,        /* 383 : X2HPERI1_ERRINT*/
    userdef_intc_dummy_interrupt,        /* 384 : X2HPERI2_ERRINT*/
    userdef_intc_dummy_interrupt,        /* 385 : X2HPERI34_ERRINT*/
    userdef_intc_dummy_interrupt,        /* 386 : X2HPERI5_ERRINT*/
    userdef_intc_dummy_interrupt,        /* 387 : X2HPERI67_ERRINT*/
    userdef_intc_dummy_interrupt,        /* 388 : X2HDBGR_ERRINT*/
    userdef_intc_dummy_interrupt,        /* 389 : X2HBSC_ERRINT */
    userdef_intc_dummy_interrupt,        /* 390 : X2HSPI1_ERRINT*/
    userdef_intc_dummy_interrupt,        /* 391 : X2HSPI2_ERRINT*/
    userdef_intc_dummy_interrupt,        /* 392 : PRRI          */
    userdef_intc_dummy_interrupt,        /* 393 : IFEI0         */
    userdef_intc_dummy_interrupt,        /* 394 : OFFI0         */
    userdef_intc_dummy_interrupt,        /* 395 : PFVEI0        */
    userdef_intc_dummy_interrupt,        /* 396 : IFEI1         */
    userdef_intc_dummy_interrupt,        /* 397 : OFFI1         */
    userdef_intc_dummy_interrupt,        /* 398 : PFVEI1        */
    userdef_intc_dummy_interrupt,        /* 399 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 400 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 401 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 402 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 403 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 404 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 405 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 406 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 407 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 408 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 409 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 410 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 411 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 412 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 413 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 414 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 415 : <reserved>    */
    userdef_intc_dummy_interrupt,        /* 416 : TINT0         */
    userdef_intc_dummy_interrupt,        /* 417 : TINT1         */
    userdef_intc_dummy_interrupt,        /* 418 : TINT2         */
    userdef_intc_dummy_interrupt,        /* 419 : TINT3         */
    userdef_intc_dummy_interrupt,        /* 420 : TINT4         */
    userdef_intc_dummy_interrupt,        /* 421 : TINT5         */
    userdef_intc_dummy_interrupt,        /* 422 : TINT6         */
    userdef_intc_dummy_interrupt,        /* 423 : TINT7         */
    userdef_intc_dummy_interrupt,        /* 424 : TINT8         */
    userdef_intc_dummy_interrupt,        /* 425 : TINT9         */
    userdef_intc_dummy_interrupt,        /* 426 : TINT10        */
    userdef_intc_dummy_interrupt,        /* 427 : TINT11        */
    userdef_intc_dummy_interrupt,        /* 428 : TINT12        */
    userdef_intc_dummy_interrupt,        /* 429 : TINT13        */
    userdef_intc_dummy_interrupt,        /* 430 : TINT14        */
    userdef_intc_dummy_interrupt,        /* 431 : TINT15        */
    userdef_intc_dummy_interrupt,        /* 432 : TINT16        */
    userdef_intc_dummy_interrupt,        /* 433 : TINT17        */
    userdef_intc_dummy_interrupt,        /* 434 : TINT18        */
    userdef_intc_dummy_interrupt,        /* 435 : TINT19        */
    userdef_intc_dummy_interrupt,        /* 436 : TINT20        */
    userdef_intc_dummy_interrupt,        /* 437 : TINT21        */
    userdef_intc_dummy_interrupt,        /* 438 : TINT22        */
    userdef_intc_dummy_interrupt,        /* 439 : TINT23        */
    userdef_intc_dummy_interrupt,        /* 440 : TINT24        */
    userdef_intc_dummy_interrupt,        /* 441 : TINT25        */
    userdef_intc_dummy_interrupt,        /* 442 : TINT26        */
    userdef_intc_dummy_interrupt,        /* 443 : TINT27        */
    userdef_intc_dummy_interrupt,        /* 444 : TINT28        */
    userdef_intc_dummy_interrupt,        /* 445 : TINT29        */
    userdef_intc_dummy_interrupt,        /* 446 : TINT30        */
    userdef_intc_dummy_interrupt,        /* 447 : TINT31        */
    userdef_intc_dummy_interrupt,        /* 448 : TINT32        */
    userdef_intc_dummy_interrupt,        /* 449 : TINT33        */
    userdef_intc_dummy_interrupt,        /* 450 : TINT34        */
    userdef_intc_dummy_interrupt,        /* 451 : TINT35        */
    userdef_intc_dummy_interrupt,        /* 452 : TINT36        */
    userdef_intc_dummy_interrupt,        /* 453 : TINT37        */
    userdef_intc_dummy_interrupt,        /* 454 : TINT38        */
    userdef_intc_dummy_interrupt,        /* 455 : TINT39        */
    userdef_intc_dummy_interrupt,        /* 456 : TINT40        */
    userdef_intc_dummy_interrupt,        /* 457 : TINT41        */
    userdef_intc_dummy_interrupt,        /* 458 : TINT42        */
    userdef_intc_dummy_interrupt,        /* 459 : TINT43        */
    userdef_intc_dummy_interrupt,        /* 460 : TINT44        */
    userdef_intc_dummy_interrupt,        /* 461 : TINT45        */
    userdef_intc_dummy_interrupt,        /* 462 : TINT46        */
    userdef_intc_dummy_interrupt,        /* 463 : TINT47        */
    userdef_intc_dummy_interrupt,        /* 464 : TINT48        */
    userdef_intc_dummy_interrupt,        /* 465 : TINT49        */
    userdef_intc_dummy_interrupt,        /* 466 : TINT50        */
    userdef_intc_dummy_interrupt,        /* 467 : TINT51        */
    userdef_intc_dummy_interrupt,        /* 468 : TINT52        */
    userdef_intc_dummy_interrupt,        /* 469 : TINT53        */
    userdef_intc_dummy_interrupt,        /* 470 : TINT54        */
    userdef_intc_dummy_interrupt,        /* 471 : TINT55        */
    userdef_intc_dummy_interrupt,        /* 472 : TINT56        */
    userdef_intc_dummy_interrupt,        /* 473 : TINT57        */
    userdef_intc_dummy_interrupt,        /* 474 : TINT58        */
    userdef_intc_dummy_interrupt,        /* 475 : TINT59        */
    userdef_intc_dummy_interrupt,        /* 476 : TINT60        */
    userdef_intc_dummy_interrupt,        /* 477 : TINT61        */
    userdef_intc_dummy_interrupt,        /* 478 : TINT62        */
    userdef_intc_dummy_interrupt,        /* 479 : TINT63        */
    userdef_intc_dummy_interrupt,        /* 480 : TINT64        */
    userdef_intc_dummy_interrupt,        /* 481 : TINT65        */
    userdef_intc_dummy_interrupt,        /* 482 : TINT66        */
    userdef_intc_dummy_interrupt,        /* 483 : TINT67        */
    userdef_intc_dummy_interrupt,        /* 484 : TINT68        */
    userdef_intc_dummy_interrupt,        /* 485 : TINT69        */
    userdef_intc_dummy_interrupt,        /* 486 : TINT70        */
    userdef_intc_dummy_interrupt,        /* 487 : TINT71        */
    userdef_intc_dummy_interrupt,        /* 488 : TINT72        */
    userdef_intc_dummy_interrupt,        /* 489 : TINT73        */
    userdef_intc_dummy_interrupt,        /* 490 : TINT74        */
    userdef_intc_dummy_interrupt,        /* 491 : TINT75        */
    userdef_intc_dummy_interrupt,        /* 492 : TINT76        */
    userdef_intc_dummy_interrupt,        /* 493 : TINT77        */
    userdef_intc_dummy_interrupt,        /* 494 : TINT78        */
    userdef_intc_dummy_interrupt,        /* 495 : TINT79        */
    userdef_intc_dummy_interrupt,        /* 496 : TINT80        */
    userdef_intc_dummy_interrupt,        /* 497 : TINT81        */
    userdef_intc_dummy_interrupt,        /* 498 : TINT82        */
    userdef_intc_dummy_interrupt,        /* 499 : TINT83        */
    userdef_intc_dummy_interrupt,        /* 500 : TINT84        */
    userdef_intc_dummy_interrupt,        /* 501 : TINT85        */
    userdef_intc_dummy_interrupt,        /* 502 : TINT86        */
    userdef_intc_dummy_interrupt,        /* 503 : TINT87        */
    userdef_intc_dummy_interrupt,        /* 504 : TINT88        */
    userdef_intc_dummy_interrupt,        /* 505 : TINT89        */
    userdef_intc_dummy_interrupt,        /* 506 : TINT90        */
    userdef_intc_dummy_interrupt,        /* 507 : TINT91        */
    userdef_intc_dummy_interrupt,        /* 508 : TINT92        */
    userdef_intc_dummy_interrupt,        /* 509 : TINT93        */
    userdef_intc_dummy_interrupt,        /* 510 : TINT94        */
    userdef_intc_dummy_interrupt,        /* 511 : TINT95        */
    userdef_intc_dummy_interrupt,        /* 512 : TINT96        */
    userdef_intc_dummy_interrupt,        /* 513 : TINT97        */
    userdef_intc_dummy_interrupt,        /* 514 : TINT98        */
    userdef_intc_dummy_interrupt,        /* 515 : TINT99        */
    userdef_intc_dummy_interrupt,        /* 516 : TINT100       */
    userdef_intc_dummy_interrupt,        /* 517 : TINT101       */
    userdef_intc_dummy_interrupt,        /* 518 : TINT102       */
    userdef_intc_dummy_interrupt,        /* 519 : TINT103       */
    userdef_intc_dummy_interrupt,        /* 520 : TINT104       */
    userdef_intc_dummy_interrupt,        /* 521 : TINT105       */
    userdef_intc_dummy_interrupt,        /* 522 : TINT106       */
    userdef_intc_dummy_interrupt,        /* 523 : TINT107       */
    userdef_intc_dummy_interrupt,        /* 524 : TINT108       */
    userdef_intc_dummy_interrupt,        /* 525 : TINT109       */
    userdef_intc_dummy_interrupt,        /* 526 : TINT110       */
    userdef_intc_dummy_interrupt,        /* 527 : TINT111       */
    userdef_intc_dummy_interrupt,        /* 528 : TINT112       */
    userdef_intc_dummy_interrupt,        /* 529 : TINT113       */
    userdef_intc_dummy_interrupt,        /* 530 : TINT114       */
    userdef_intc_dummy_interrupt,        /* 531 : TINT115       */
    userdef_intc_dummy_interrupt,        /* 532 : TINT116       */
    userdef_intc_dummy_interrupt,        /* 533 : TINT117       */
    userdef_intc_dummy_interrupt,        /* 534 : TINT118       */
    userdef_intc_dummy_interrupt,        /* 535 : TINT119       */
    userdef_intc_dummy_interrupt,        /* 536 : TINT120       */
    userdef_intc_dummy_interrupt,        /* 537 : TINT121       */
    userdef_intc_dummy_interrupt,        /* 538 : TINT122       */
    userdef_intc_dummy_interrupt,        /* 539 : TINT123       */
    userdef_intc_dummy_interrupt,        /* 540 : TINT124       */
    userdef_intc_dummy_interrupt,        /* 541 : TINT125       */
    userdef_intc_dummy_interrupt,        /* 542 : TINT126       */
    userdef_intc_dummy_interrupt,        /* 543 : TINT127       */
    userdef_intc_dummy_interrupt,        /* 544 : TINT128       */
    userdef_intc_dummy_interrupt,        /* 545 : TINT129       */
    userdef_intc_dummy_interrupt,        /* 546 : TINT130       */
    userdef_intc_dummy_interrupt,        /* 547 : TINT131       */
    userdef_intc_dummy_interrupt,        /* 548 : TINT132       */
    userdef_intc_dummy_interrupt,        /* 549 : TINT133       */
    userdef_intc_dummy_interrupt,        /* 550 : TINT134       */
    userdef_intc_dummy_interrupt,        /* 551 : TINT135       */
    userdef_intc_dummy_interrupt,        /* 552 : TINT136       */
    userdef_intc_dummy_interrupt,        /* 553 : TINT137       */
    userdef_intc_dummy_interrupt,        /* 554 : TINT138       */
    userdef_intc_dummy_interrupt,        /* 555 : TINT139       */
    userdef_intc_dummy_interrupt,        /* 556 : TINT140       */
    userdef_intc_dummy_interrupt,        /* 557 : TINT141       */
    userdef_intc_dummy_interrupt,        /* 558 : TINT142       */
    userdef_intc_dummy_interrupt,        /* 559 : TINT143       */
    userdef_intc_dummy_interrupt,        /* 560 : TINT144       */
    userdef_intc_dummy_interrupt,        /* 561 : TINT145       */
    userdef_intc_dummy_interrupt,        /* 562 : TINT146       */
    userdef_intc_dummy_interrupt,        /* 563 : TINT147       */
    userdef_intc_dummy_interrupt,        /* 564 : TINT148       */
    userdef_intc_dummy_interrupt,        /* 565 : TINT149       */
    userdef_intc_dummy_interrupt,        /* 566 : TINT150       */
    userdef_intc_dummy_interrupt,        /* 567 : TINT151       */
    userdef_intc_dummy_interrupt,        /* 568 : TINT152       */
    userdef_intc_dummy_interrupt,        /* 569 : TINT153       */
    userdef_intc_dummy_interrupt,        /* 570 : TINT154       */
    userdef_intc_dummy_interrupt,        /* 571 : TINT155       */
    userdef_intc_dummy_interrupt,        /* 572 : TINT156       */
    userdef_intc_dummy_interrupt,        /* 573 : TINT157       */
    userdef_intc_dummy_interrupt,        /* 574 : TINT158       */
    userdef_intc_dummy_interrupt,        /* 575 : TINT159       */
    userdef_intc_dummy_interrupt,        /* 576 : TINT160       */
    userdef_intc_dummy_interrupt,        /* 577 : TINT161       */
    userdef_intc_dummy_interrupt,        /* 578 : TINT162       */
    userdef_intc_dummy_interrupt,        /* 579 : TINT163       */
    userdef_intc_dummy_interrupt,        /* 580 : TINT164       */
    userdef_intc_dummy_interrupt,        /* 581 : TINT165       */
    userdef_intc_dummy_interrupt,        /* 582 : TINT166       */
    userdef_intc_dummy_interrupt,        /* 583 : TINT167       */
    userdef_intc_dummy_interrupt,        /* 584 : TINT168       */
    userdef_intc_dummy_interrupt,        /* 585 : TINT169       */
    userdef_intc_dummy_interrupt,        /* 586 : TINT170       */
};


/*******************************************************************************
* Function Name: userdef_intc_regist_int_func
* Description  : This function is the user-defined function called by the 
*              : R_INTC_RegistIntFunc. Write the processing to register the 
*              : specified function to the element specified by the int_id in 
*              : INTC interrupt handler function table intc_func_table[].
* Arguments    : uint16_t int_id         : Interrupt ID
*              : void (* func)(uint32_t) : Function to be registered to INTC
*              :                         : interrupt handler table
* Return Value : none
*******************************************************************************/
void userdef_intc_regist_int_func (uint16_t int_id,
		                                     void (* func)(uint32_t int_sense))
{
    intc_func_table[int_id] = func;
}

/*******************************************************************************
* Function Name: userdef_intc_undef_id
* Description  : This function is the user-defined function called by the 
*              : INTC_Handler_Interrupt. Write the processing to be executed
*              : when an unsupported interrupt ID is received. 
* Arguments    : uint16_t int_id    : Interrupt ID
* Return Value : none
*******************************************************************************/
void userdef_intc_undef_id (uint16_t int_id)
{
    /* Execute processing when receiving interrupt ID which is not supported */
    while (1)
    {
        /* Do Nothing */
        __asm__("nop");
    }
}

/*******************************************************************************
* Function Name: userdef_intc_handler_exe
* Description  : This function is the user-defined function called by the 
*              : INTC_Handler_Interrupt. Write the handler processing which 
*              : corresponds to the INTC interrupt source ID specified by the 
*              : argument int_id to be executed. In this sample code, the
*              : function registered as an element of the int_id in the INTC
*              : interrupt handler function table intc_func_table[] is executed.
* Arguments    : uint16_t int_id    : Interrupt ID
*              : uint32_t int_sense : Interrupt detection
*              :                    :   INTC_LEVEL_SENSITIVE : Level sense
*              :                    :   INTC_EDGE_TRIGGER    : Edge trigger
* Return Value : none
*******************************************************************************/
void userdef_intc_handler_exe (uint16_t int_id, uint32_t int_sense)
{
	intc_func_table[int_id](int_sense);
}

/*******************************************************************************
* Function Name: userdef_fiq_handler_exe
* Description  : This function is the user-defined function called by 
*              : the FiqHandler_Interrupt.
* Arguments    : none
* Return Value : none
******************************************************************************/
void userdef_fiq_handler_exe (void)
{
	/*
	uint16_t dummy_read = 0;

	// Set the NMI status bit in the user flag
    g_switch_press_flg |= NMI_SET_FLG;

    // Clearing the status flag requires a dummy read
    dummy_read = INTC.ICR0;

    // Clear the NMI interrupt flag
    if (NMIF_BIT == (dummy_read & NMIF_BIT))
	{
    	INTC.ICR0 &= 0xFFFD;
	}
    */
}


/*******************************************************************************
* Function Name: userdef_intc_dummy_interrupt
* Description  : This function is the dummy processing for interrupt handler.
*              : In this sample code, this function is registered by default in
*              : all interrupts in the INTC interrupt handler table
*              : intc_func_table[].
* Arguments    : uint32_t int_sense : Interrupt detection
*              :                    :   INTC_LEVEL_SENSITIVE : Level sense
*              :                    :   INTC_EDGE_TRIGGER    : Edge trigger
* Return Value : none
*******************************************************************************/
static void userdef_intc_dummy_interrupt (uint32_t int_sense)
{
    /* Execute processing of non-registered interrupt ID */
}


/* END of File */

