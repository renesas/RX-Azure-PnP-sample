/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : azurertos_object_init.h
 * Version      : 1.0
 * Description  : declare libraries includes, global Object variables used for other files, and function prototypes
 **********************************************************************************************************************/

 #ifndef AZURERTOS_OBJECT_INIT_H_
 #define AZURERTOS_OBJECT_INIT_H_

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include "tx_api.h"

/***********************************************************************************************************************
 * ThreadX object control blocks definitions
 **********************************************************************************************************************/
extern TX_THREAD               sensor_thread;
extern TX_THREAD               zmod_thread;
extern TX_THREAD               ob1203_thread;

/***********************************************************************************************************************
 * Stacks definitions
 **********************************************************************************************************************/
extern UCHAR               sensor_thread_memory_stack[1024];
extern UCHAR               zmod_thread_memory_stack[1024];
extern UCHAR               ob1203_thread_memory_stack[1024];
/***********************************************************************************************************************
 * Function prototypes definition
 **********************************************************************************************************************/
extern void        sensor_thread_entry(ULONG entry_input);
extern void        zmod_thread_entry(ULONG entry_input);
extern void        ob1203_thread_entry(ULONG entry_input);

#endif /* AZURERTOS_OBJECT_INIT_H_ */
