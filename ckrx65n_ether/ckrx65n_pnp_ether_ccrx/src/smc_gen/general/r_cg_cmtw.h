/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name        : r_cg_cmtw.h
* Version          : 1.2.111
* Device(s)        : R5F565NEHxFB
* Description      : General header file for CMTW peripheral.
***********************************************************************************************************************/

#ifndef CMTW_H
#define CMTW_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    Timer Start Register (CMWSTR)
*/
#define _0000_CMTW_CMWSTR_COUNTER_STOP            (0x0000U) /* Stop counter count */
#define _0001_CMTW_CMWSTR_COUNTER_START           (0x0001U) /* Start counter count */

/*
    Timer Control Register (CMWCR)
*/
/* Clock select (CKS[1:0]) */
#define _0000_CMTW_CMWCR_CLOCK_PCLK8              (0x0000U) /* PCLK/8 */
#define _0001_CMTW_CMWCR_CLOCK_PCLK32             (0x0001U) /* PCLK/32 */
#define _0002_CMTW_CMWCR_CLOCK_PCLK128            (0x0002U) /* PCLK/128 */
#define _0003_CMTW_CMWCR_CLOCK_PCLK512            (0x0003U) /* PCLK/512 */
/* Compare Match Interrupt Enable (CMWIE) */
#define _0000_CMTW_CMWCR_CMWIE_DISABLE            (0x0000U) /* Disable Compare Match Interrupt */
#define _0008_CMTW_CMWCR_CMWIE_ENABLE             (0x0008U) /* Enable Compare Match Interrupt */
/* Input Capture 0 Interrupt Enable (IC0IE) */
#define _0000_CMTW_CMWCR_IC0IE_DISABLE            (0x0000U) /* Disable Input Capture 0 Interrupt */
#define _0010_CMTW_CMWCR_IC0IE_ENABLE             (0x0010U) /* Enable Input Capture 0 Interrupt */
/* Input Capture 1 Interrupt Enable (IC1IE) */
#define _0000_CMTW_CMWCR_IC1IE_DISABLE            (0x0000U) /* Disable Input Capture 1 Interrupt */
#define _0020_CMTW_CMWCR_IC1IE_ENABLE             (0x0020U) /* Enable Input Capture 1 Interrupt */
/* Output Compare 0 Interrupt Enable (OC0IE) */
#define _0000_CMTW_CMWCR_OC0IE_DISABLE            (0x0000U) /* Disable Output Compare 0 Interrupt */
#define _0040_CMTW_CMWCR_OC0IE_ENABLE             (0x0040U) /* Enable Output Compare 0 Interrupt */
/* Output Compare 1 Interrupt Enable (OC1IE) */
#define _0000_CMTW_CMWCR_OC1IE_DISABLE            (0x0000U) /* Disable Output Compare 1 Interrupt */
#define _0080_CMTW_CMWCR_OC1IE_ENABLE             (0x0080U) /* Enable Output Compare 1 Interrupt */
/* Timer Counter Size (CMS) */
#define _0000_CMTW_CMWCR_COUNTER_SIZE_32          (0x0000U) /* 32 bits */
#define _0200_CMTW_CMWCR_COUNTER_SIZE_16          (0x0200U) /* 16 bits */
/* Counter Clear (CCLR[2:0]) */
#define _0000_CMTW_CMWCR_CCLR_ENABLE_CMWCOR       (0x0000U) /* CMWCNT counter cleared by CMWCOR */
#define _2000_CMTW_CMWCR_CCLR_DISABLE             (0x2000U) /* Clearing of CMWCNT counter disabled */
#define _4000_CMTW_CMWCR_CCLR_DISABLE             (0x4000U) /* Clearing of CMWCNT counter disabled */
#define _6000_CMTW_CMWCR_CCLR_DISABLE             (0x6000U) /* Clearing of CMWCNT counter disabled */
#define _8000_CMTW_CMWCR_CCLR_CMWICR0_ENABLE      (0x8000U) /* CMWCNT counter cleared by CMWICR0 */
#define _A000_CMTW_CMWCR_CCLR_CMWICR1_ENABLE      (0xA000U) /* CMWCNT counter cleared by CMWICR1 */
#define _C000_CMTW_CMWCR_CCLR_CMWOCR0_ENABLE      (0xC000U) /* CMWCNT counter cleared by CMWOCR0 */
#define _E000_CMTW_CMWCR_CCLR_CMWOCR1_ENABLE      (0xE000U) /* CMWCNT counter cleared by CMWOCR1 */

/*
    Timer I/O Control Register (CMWIOR)
*/
/* Input Compare Control 0 (IC0[1:0]) */
#define _0000_CMTW_CMWIOR_IC0_RISE                (0x0000U) /* Rising edge */
#define _0001_CMTW_CMWIOR_IC0_FALL                (0x0001U) /* Falling edge */
#define _0002_CMTW_CMWIOR_IC0_BOTH                (0x0002U) /* Both edges */
/* Input Capture Control 1 (IC1[1:0]) */
#define _0000_CMTW_CMWIOR_IC1_RISE                (0x0000U) /* Rising edge */
#define _0004_CMTW_CMWIOR_IC1_FALL                (0x0004U) /* Falling edge */
#define _0008_CMTW_CMWIOR_IC1_BOTH                (0x0008U) /* Both edges */
/* Input Capture Enable 0 (IC0E) */
#define _0000_CMTW_CMWIOR_IC0E_DISABLE            (0x0000U) /* Disable input capture of CMWICR0 */
#define _0010_CMTW_CMWIOR_IC0E_ENABLE             (0x0010U) /* Enable input capture of CMWICR0 */
/* Input Capture Enable 1 (IC1E) */
#define _0000_CMTW_CMWIOR_IC1E_DISABLE            (0x0000U) /* Disable input capture of CMWICR1 */
#define _0020_CMTW_CMWIOR_IC1E_ENABLE             (0x0020U) /* Enable input capture of CMWICR1 */
/* Output Compare Control 0 (OC0[1:0]) */
#define _0000_CMTW_CMWIOR_OC0_RETAIN              (0x0000U) /* Retains the output value */
#define _0100_CMTW_CMWIOR_OC0_OUTPUT0             (0x0100U) /* Initially outputs 0 */
#define _0200_CMTW_CMWIOR_OC0_OUTPUT1             (0x0200U) /* Initially outputs 1 */
/* Output Compare Control 1 (OC1[1:0]) */
#define _0000_CMTW_CMWIOR_OC1_RETAIN              (0x0000U) /* Retains the output value */
#define _0400_CMTW_CMWIOR_OC1_OUTPUT0             (0x0400U) /* Initially outputs 0 */
#define _0800_CMTW_CMWIOR_OC1_OUTPUT1             (0x0800U) /* Initially outputs 1 */
/* Compare Match Enable 0 (OC0E) */
#define _0000_CMTW_CMWIOR_OC0E_DISABLE            (0x0000U) /* Disable compare match using CMWOCR0 */
#define _1000_CMTW_CMWIOR_OC0E_ENABLE             (0x1000U) /* Enable compare match using CMWOCR0 */
/* Compare Match Enable 1 (OC1E) */
#define _0000_CMTW_CMWIOR_OC1E_DISABLE            (0x0000U) /* Disable compare match using CMWOCR1 */
#define _2000_CMTW_CMWIOR_OC1E_ENABLE             (0x2000U) /* Enable compare match using CMWOCR1 */
/* Compare Match Enable (CMWE) */
#define _0000_CMTW_CMWIOR_CMWE_DISABLE            (0x0000U) /* Disable compare match using CMWCOR */
#define _8000_CMTW_CMWIOR_CMWE_ENABLE             (0x8000U) /* Enable compare match using CMWCOR */

/*
    Interrupt Source Priority Register n (IPRn)
*/
/* Interrupt Priority Level Select (IPR[3:0]) */
#define _00_CMTW_PRIORITY_LEVEL0                  (0x00U) /* Level 0 (interrupt disabled) */
#define _01_CMTW_PRIORITY_LEVEL1                  (0x01U) /* Level 1 */
#define _02_CMTW_PRIORITY_LEVEL2                  (0x02U) /* Level 2 */
#define _03_CMTW_PRIORITY_LEVEL3                  (0x03U) /* Level 3 */
#define _04_CMTW_PRIORITY_LEVEL4                  (0x04U) /* Level 4 */
#define _05_CMTW_PRIORITY_LEVEL5                  (0x05U) /* Level 5 */
#define _06_CMTW_PRIORITY_LEVEL6                  (0x06U) /* Level 6 */
#define _07_CMTW_PRIORITY_LEVEL7                  (0x07U) /* Level 7 */
#define _08_CMTW_PRIORITY_LEVEL8                  (0x08U) /* Level 8 */
#define _09_CMTW_PRIORITY_LEVEL9                  (0x09U) /* Level 9 */
#define _0A_CMTW_PRIORITY_LEVEL10                 (0x0AU) /* Level 10 */
#define _0B_CMTW_PRIORITY_LEVEL11                 (0x0BU) /* Level 11 */
#define _0C_CMTW_PRIORITY_LEVEL12                 (0x0CU) /* Level 12 */
#define _0D_CMTW_PRIORITY_LEVEL13                 (0x0DU) /* Level 13 */
#define _0E_CMTW_PRIORITY_LEVEL14                 (0x0EU) /* Level 14 */
#define _0F_CMTW_PRIORITY_LEVEL15                 (0x0FU) /* Level 15 (highest) */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif

