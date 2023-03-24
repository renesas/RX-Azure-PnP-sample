/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : menu_ns.c
 * Version      : .
 * Description  : The next steps screen display.
 *********************************************************************************************************************/

//#include <console_menu/menu_flash.h>
//#include "FreeRTOS.h"
//#include "FreeRTOSconfig.h"
//#include "semphr.h"
//#include "queue.h"
//#include "task.h"

#include "common_init.h"
#include "common_utils.h"
#include "console.h"
#include "rm_hs300x.h"
#include "console.h"
#include "menu_flash.h"
#include "r_simple_filesystem_on_dataflash_if.h"
#include "rx65n_cloudkit.h"
#include "sensorsData.h"
#include "nx_api.h"
#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x20)
#define MENU_EXIT                (32)

static char_t s_print_buffer[BUFFER_LINE_LENGTH];// = {};

#define MODULE_NAME_FLASH  "\r\n%d. DATA FLASH\r\n"
#define SUB_OPTIONS1       "\r\n %c) Info"
#define SUB_OPTIONS2       "\r\n %c) Write Certificate "
#define SUB_OPTIONS3       "\r\n %c) Write Private Key "
#define SUB_OPTIONS4       "\r\n %c) Host Name"
#define SUB_OPTIONS5       "\r\n %c) Device ID"
#define SUB_OPTIONS6       "\r\n %c) Read Flash"
//#define SUB_OPTIONS7       "\r\n %c) TCP Send performance server IP address"
#define SUB_OPTIONS7       "\r\n %c) Check credentials stored in flash memory"
#define SUB_OPTIONS8       "\r\n %c) Help"

#define MODULE_NAME_FLASH_AWS     "\r\n DATA FLASH WRITE\r\n"
#define SUB_OPTIONS               "\r\n %c) Certificate and key"

bool Key_skip = false;
bool g_cred_type_flag = false;

uint8_t *label, *data;
uint32_t label_length, data_length;
static char *message_buffer;

typedef struct menu_fn_tbl
{
    char_t * p_name; /*<! Name of Test */
    void ( * p_func)(void); /*<! Pointer to Test Function */
} st_menu_fn_tbl_t;


typedef struct st_hs_300x_sensor
{
    bool_t               hs300x_callback;
    rm_hs300x_data_t     hs300x_data;
} st_hs_300x_sensor_t;


test_fn data_flash_menu(void)
{
	fsp_err_t err = FSP_SUCCESS;
	int8_t Key_pressed = -1;
	credentials_t credentials;

   // err = flash_mem_init();
  //  assert(FSP_SUCCESS == err);

	while (MENU_EXIT_CRTL != Key_pressed)
	{

    char_t ndx = 'a';
    printf_colour ("%s%s", gp_clear_screen, gp_cursor_home);
   sprintf (s_print_buffer, OPTIONS_SELECT_FLASH);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);
    printf_colour(MODULE_NAME_FLASH, g_selected_menu);
    printf_colour(SUB_OPTIONS1, ndx++);
    printf_colour(SUB_OPTIONS2, ndx++);
    printf_colour(SUB_OPTIONS3, ndx++);
    printf_colour(SUB_OPTIONS4, ndx++);
    printf_colour(SUB_OPTIONS5, ndx++);
    printf_colour(SUB_OPTIONS6, ndx++);
    //printf_colour(SUB_OPTIONS7, ndx++);
    printf_colour(SUB_OPTIONS7, ndx++);
    printf_colour(SUB_OPTIONS8, ndx++);

    printf_colour(MENU_RETURN_INFO);

    if (Key_skip == false)
    {
    	//start_key_check();
    	Key_pressed = (int8_t)wait_for_keypress ();
    }
    	switch (Key_pressed)
        {
        	case INFO :
		    {
		    	Key_skip = false;
		    	flash_info();
		    	while (MENU_EXIT_CRTL != (Key_pressed = (int8_t)wait_for_keypress ()));
		    	Key_pressed = 0;
		    	break;
		    }
        	case WRITE_ENDPOINT ://Cer
        	{
        		Key_skip = false;
        		g_cred_type_flag = false;
        		credentials = AZ_DEVICE_CER;;
        		flash_display_menu(credentials);
        		err = aws_cert_key_write(credentials);
        		//assert(FSP_SUCCESS == err);
        		while (MENU_EXIT_CRTL != (Key_pressed = (int8_t)wait_for_keypress ()));
        	    Key_pressed = 0;
        		break;
        	}
        	case WRITE_ID_SCOPE ://Key
        	{
        		Key_skip = false;
        		g_cred_type_flag = false;
        		credentials = AZ_PVT_KEY;
        		flash_display_menu(credentials);
        		err = aws_cert_key_write (credentials);
        		//assert(FSP_SUCCESS == err);
        		while (MENU_EXIT_CRTL != (Key_pressed = (int8_t)wait_for_keypress ()));
        	    Key_pressed = 0;
        		break;
        	}
        	case WRITE_REGISTRATION_ID : //host name
        	{
        		Key_skip = false;
        		g_cred_type_flag = true;
        		credentials = AZ_HOST_N;
        		flash_display_menu(credentials);
        		err = aws_cert_key_write (credentials);
        		//assert(FSP_SUCCESS == err);
        		while (MENU_EXIT_CRTL != (Key_pressed = (int8_t)wait_for_keypress ()));
        	    Key_pressed = 0;
        		break;
        	}
        	case WRITE_DEVICE_SYMMETRIC_KEY : //devid
        	{
        		Key_skip = false;
        		g_cred_type_flag = true;
        		credentials = AZ_DEVICE_IDD;
        		flash_display_menu(credentials);
        		err = aws_cert_key_write (credentials);
        		//assert(FSP_SUCCESS == err);
        		while (MENU_EXIT_CRTL != (Key_pressed = (int8_t)wait_for_keypress ()));
        	    Key_pressed = 0;
        		break;
        	}
        	case READ_FLASH :
        	{
        	    Key_skip = false;
        	    err = flash_hp_data_read(true);
        	    //assert(FSP_SUCCESS == err);
        	    while (MENU_EXIT_CRTL != (Key_pressed = (int8_t)wait_for_keypress ()));
        	    Key_pressed = 0;
        	    break;
        	}
        	case CHECK_CREDENTIALS :
        	{
        		Key_skip = false;
        		check_credentials_stored();
        		printf_colour(MENU_RETURN_INFO);
         		while (MENU_EXIT_CRTL != (Key_pressed = (int8_t)wait_for_keypress ()));
        	    Key_pressed = 0;
        		break;
        	}
        	case HELP :
        	{
        		Key_skip = false;
        		help_menu();
         		while (MENU_EXIT_CRTL != (Key_pressed = (int8_t)wait_for_keypress ()));
        	    Key_pressed = 0;
        		break;
        	}
        	case ' ' :
        	{
        		break;
        	}
        	default :
        	{
        		printf_colour("\r\nPlease enter correct option....");
        		while ( 'a'<= (Key_pressed = (int8_t)wait_for_keypress ()) >= 'h');
        		Key_skip = true;
        	}
        }
	}
	return 0;
}


test_fn AWS_cert_key_write (void)
{

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);

	 printf_colour("....AWS....");
	 return 0;
}

void flash_info(void)
{
	static char_t s_print_buffer[BUFFER_LENGTH_SHORT];
	sprintf(s_print_buffer, "physical size = %d bytes.\n",
	R_SFD_ReadPysicalSize());
	printf_colour(s_print_buffer);
	sprintf(s_print_buffer, "allocated size = %d bytes.\n",
	R_SFD_ReadAllocatedStorageSize());
	printf_colour(s_print_buffer);
	sprintf(s_print_buffer, "free size = %d bytes.\n", R_SFD_ReadFreeSize());
	printf_colour(s_print_buffer);
    printf_colour(MENU_RETURN_INFO);
}

void help_menu(void)
{
	char_t s_print_buffer[BUFFER_LENGTH_SHORT];// = {};

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME_FLASH_HELP);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);

    printf_colour(FLASH_HELP_TAG1);
    printf_colour(FLASH_HELP_TAG2);
    printf_colour(FLASH_HELP_TAG3);
    printf_colour(FLASH_HELP_TAG4);
    printf_colour(FLASH_HELP_TAG5);
    printf_colour(FLASH_HELP_TAG6);
    printf_colour(FLASH_HELP_TAG7);

	printf_colour(MENU_RETURN_INFO);

}

void flash_display_menu(uint8_t credential_type)
{
	char_t s_print_buffer[BUFFER_LENGTH_SHORT];// = {};

	sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
	printf_colour((void*)s_print_buffer);

	if (AZ_HOST_N == credential_type)
	{
	    sprintf (s_print_buffer, FLASH_WRITE_THING_NAME);
	    printf_colour((void*)s_print_buffer);
	    sprintf (s_print_buffer, WRITE_HOST_NAME_STRING);
	    printf_colour((void*)s_print_buffer);
	}
	else if (AZ_DEVICE_IDD == credential_type)
	{
	    sprintf (s_print_buffer, FLASH_WRITE_MQTT_END_POINT);
	    printf_colour((void*)s_print_buffer);
	    sprintf (s_print_buffer, WRITE_DEVICE_ID_STRING);
	    printf_colour((void*)s_print_buffer);
	}
	else if (AZ_DEVICE_CER == credential_type)
	{
	    sprintf (s_print_buffer, FLASH_WRITE_CERT_MENU);
	    printf_colour((void*)s_print_buffer);
	    sprintf (s_print_buffer, SELECT_FILE);
	    printf_colour((void*)s_print_buffer);
	}
	else if (AZ_PVT_KEY == credential_type)
	{
		sprintf (s_print_buffer, FLASH_WRITE_KEY_MENU);
	    printf_colour((void*)s_print_buffer);
	    sprintf (s_print_buffer, SELECT_FILE);
	    printf_colour((void*)s_print_buffer);
	}



}

uint16_t buffer_pointer;
uint16_t uiInWritebufferIndex =0;

fsp_err_t aws_cert_key_write(uint8_t cert_type)
{
    fsp_err_t err = FSP_SUCCESS;
    buffer_pointer = 0;
    memset(g_write_buffer, 0, 2048);
    uiInWritebufferIndex = 0;
	g_start_read_flag = 1;
    while (1)
    {
    	        if (uiInWritebufferIndex >= 2048)
    	    	{
    	    		uiInWritebufferIndex = 0;
    	    	}

    	        if((strstr(g_write_buffer, "-----END CERTIFICATE-----")) || (strstr(g_write_buffer, "-----END RSA PRIVATE KEY-----")))
    	        {

    	             g_start_read_flag = false;
    	             break;
    	        }
    	        if ((g_cred_type_flag == true) && strchr(g_write_buffer, '\r'))
    	        {

    	        	g_start_read_flag = false;
    	        	g_cred_type_flag = false;
    	            break;
    	        }
    	        R_BSP_SoftwareDelay(1,BSP_DELAY_MILLISECS);
        //tx_thread_sleep(2);
    }

    if (AZ_DEVICE_CER == cert_type)
        {
        	char *target;
        	target = (char *)device_cert_label;
        	uint16_t current_buffer_pointer =strlen(g_write_buffer);
            //printf("\r\nBuf Len Cer: %d",current_buffer_pointer);
        	//g_write_buffer[current_buffer_pointer+1] = 0;
    		/* +1 means '\0' as a string terminator */
    		if (SFD_HANDLE_INVALID!= R_SFD_SaveObject((uint8_t*) target, strlen(target),(uint8_t*) g_write_buffer,strlen(g_write_buffer) + 1))
    		{
    			R_BSP_SoftwareDelay(100,BSP_DELAY_MILLISECS);
    			//tx_thread_sleep(100);
    			 printf_colour("stored data into dataflash correctly.\n");
    			 uiInWritebufferIndex =0;
    			 memset(g_write_buffer, 0, 2048);

    			//display_serial_terminal_putstring_with_uart(task_info->hWin_serial_terminal, sci_handle,message_buffer);
    		}
    		else
    		{
    			printf_colour("could not store data into dataflash.\n");
    			//display_serial_terminal_putstring_with_uart(task_info->hWin_serial_terminal, sci_handle,message_buffer);
    		}
        }
        else if (AZ_PVT_KEY == cert_type)
        {
        	        char *target;
        	    	target = (char *)private_key_label;
        	    	uint16_t current_buffer_pointer =strlen(g_write_buffer);
        	    	//printf("\r\nBuf Len Pvt: %d",current_buffer_pointer);
        	    	//g_write_buffer[current_buffer_pointer+1] = 0;
        			/* +1 means '\0' as a string terminator */
        			if (SFD_HANDLE_INVALID!= R_SFD_SaveObject((uint8_t*) target, strlen(target),(uint8_t*) g_write_buffer,strlen((const char *)g_write_buffer) + 1))
        			{
        				//tx_thread_sleep(100);
        				R_BSP_SoftwareDelay(100,BSP_DELAY_MILLISECS);
        				 printf_colour("stored data into dataflash correctly.\n");
        				 uiInWritebufferIndex =0;
        				 memset(g_write_buffer, 0, 2048);
        				//display_serial_terminal_putstring_with_uart(task_info->hWin_serial_terminal, sci_handle,message_buffer);
        			}
        			else
        			{
        				printf_colour("could not store data into dataflash.\n");
        				//display_serial_terminal_putstring_with_uart(task_info->hWin_serial_terminal, sci_handle,message_buffer);
        			}
        }
        else if (AZ_HOST_N == cert_type)
        {
        	if(SFD_HANDLE_INVALID != R_SFD_SaveObject((uint8_t *)hostname_label,strlen(hostname_label), (uint8_t*) g_write_buffer,strlen((const char *)g_write_buffer) + 1))
        	{
        		//tx_thread_sleep(100);
        		R_BSP_SoftwareDelay(100,BSP_DELAY_MILLISECS);
        		printf_colour("stored data into dataflash correctly.\n");
        		uiInWritebufferIndex =0;
        		memset(g_write_buffer, 0, 2048);

    		}
        	else
    		{
    			printf_colour("could not store data into dataflash.\n");

    		}
        }
        else if (AZ_DEVICE_IDD == cert_type)
        {
        	if(SFD_HANDLE_INVALID != R_SFD_SaveObject((uint8_t *)deviceID_label,strlen(deviceID_label), (uint8_t*) g_write_buffer,strlen((const char *)g_write_buffer) + 1))
        	{
        		//tx_thread_sleep(100);
        		R_BSP_SoftwareDelay(100,BSP_DELAY_MILLISECS);
    			printf_colour("stored data into dataflash correctly.\n");
    			uiInWritebufferIndex =0;
    			memset(g_write_buffer, 0, 2048);

    		}
        	else
        	{
    			printf_colour("could not store data into dataflash.\n");

    		}
        }
        printf_colour(MENU_RETURN_INFO);
        return err;
}


fsp_err_t flash_hp_data_read(bool print_data)
{
	fsp_err_t err = FSP_SUCCESS;
	uint8_t count = 0;
	char_t s_print_buffer[BUFFER_LENGTH_SHORT];// = {};
	char *ptr;
	uint8_t *tmp;
	uint8_t *readclient_private_key, *readclient_certificate,*readendpont,*readiotThing;
	uint32_t data_length;
	sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
	printf_colour((void*)s_print_buffer);

	sprintf (s_print_buffer, MODULE_NAME_FLASH_READ);
	printf_colour((void*)s_print_buffer);


		if (count == AZ_DEVICE_CER)
		{
			R_SFD_ResetScan();
			while (1)
			{
				if (SFD_SUCCESS== R_SFD_Scan(&label, &label_length, &data,&data_length))
				{
					sprintf(message_buffer, "label = ");
					printf_colour(message_buffer);
					memcpy(message_buffer, label, label_length);
					message_buffer[label_length] = 0;
					printf_colour(message_buffer);
					sprintf(message_buffer, "\n");
					printf_colour(message_buffer);
					sprintf(message_buffer, "data = ");
					printf_colour(message_buffer);
					memcpy(message_buffer, data, data_length);
					message_buffer[data_length] = 0;
					data[data_length] = 0;
					printf_colour(message_buffer);
					R_BSP_SoftwareDelay(500,BSP_DELAY_MILLISECS);
					//tx_thread_sleep(500); /* 長い文字列をSCI(UART)出力した際に最後のほうが出力されない。完了待ちの方法が不明なためとりあえず時間待ち */
					sprintf(message_buffer, "\n");
					printf_colour(message_buffer);
					sprintf(message_buffer,"data_length includes string terminator 1 byte zero) = %d",data_length);
					printf_colour(message_buffer);
					sprintf(message_buffer, "\n\n");
					printf_colour(message_buffer);
				}
				else
				{
					break;
				}
				R_BSP_SoftwareDelay(5,BSP_DELAY_MILLISECS);
				//vTaskDelay(5);
			}
		}
    printf_colour(MENU_RETURN_INFO);
	count = 0;
    return err;
}

static char read_buffer[2048]= {0};
fsp_err_t check_credentials_stored (void)
{
	fsp_err_t err = FSP_SUCCESS;
	char_t s_print_buffer[BUFFER_LENGTH_SHORT];
    uint8_t *tmp;
    uint32_t data_length;
    bool certificateChkFlag=false,privatrKeyChkFlag=false,endpointChkFlag=false,iotThingChkFlag=false;
	sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
	printf_colour((void*)s_print_buffer);

    sprintf (s_print_buffer, FLASH_CHECK_CREDENTIALS);
    printf_colour((void*)s_print_buffer);

    if(SFD_SUCCESS == R_SFD_GetObjectValue(R_SFD_FindObject((uint8_t *)hostname_label, strlen(hostname_label)),&tmp,&data_length))
    {
    	memcpy(read_buffer, tmp, data_length);
    	if (data_length>2)
    	 {
    	     printf_colour("\r\n [GREEN]Endpoint saved in data flash is verified and successful[WHITE]\r\n");
    	     memset (read_buffer, 0, strlen (read_buffer));
    	     certificateChkFlag =true;
    	 }
    	else
    	{
    		printf_colour("\r\n [RED]Endpoint saved in data flash is not saved correctly or corrupted[WHITE]\r\n");
    		certificateChkFlag =false;
    	}
    }
    else
    {
    	printf_colour("\r\n [RED]Endpoint in data flash is not saved correctly or corrupted[WHITE]\r\n");
    	certificateChkFlag =false;
    }

    if(SFD_SUCCESS == R_SFD_GetObjectValue(R_SFD_FindObject((uint8_t *)deviceID_label, strlen(deviceID_label)),&tmp,&data_length))
      {
      	memcpy(read_buffer, tmp, data_length);
      	if (data_length>2)
      	 {
      	     printf_colour("\r\n [GREEN]ID SCOPE saved in data flash is verified and successful[WHITE]\r\n");
      	     memset (read_buffer, 0, strlen (read_buffer));
      	     privatrKeyChkFlag =true;
      	 }
      	else
      	{
      		printf_colour("\r\n [RED]ID SCOPE saved in data flash is not saved correctly or corrupted[WHITE]\r\n");
      		privatrKeyChkFlag =false;
      	}
      }
    else
    {
    	printf_colour("\r\n [RED]ID SCOPE in data flash is not saved correctly or corrupted[WHITE]\r\n");
    	privatrKeyChkFlag =false;
    }

    if(SFD_SUCCESS == R_SFD_GetObjectValue(R_SFD_FindObject((uint8_t *)device_cert_label, strlen(device_cert_label)),&tmp,&data_length))
      {
          	memcpy(read_buffer, tmp, data_length);
          	if (data_length>2)
          	 {
          	     printf_colour("\r\n [GREEN]Registration ID saved in data flash is verified and successful[WHITE]\r\n");
          	     memset (read_buffer, 0, strlen (read_buffer));
          	     iotThingChkFlag =true;
          	 }
          	else
          	{
          		printf_colour("\r\n [RED]Registration ID saved in data flash is not saved correctly or corrupted[WHITE]\r\n");
          		iotThingChkFlag =false;
          	}
       }
    else
    {
    	printf_colour("\r\n [RED]Registration ID saved in data flash is not saved correctly or corrupted[WHITE]\r\n");
    	          		iotThingChkFlag =false;
    }

    if(SFD_SUCCESS == R_SFD_GetObjectValue(R_SFD_FindObject((uint8_t *)private_key_label, strlen(private_key_label)),&tmp,&data_length))
      {
            memcpy(read_buffer, tmp, data_length);
            if (data_length>2)
              {
              	 printf_colour("\r\n [GREEN]Device symetric key saved in data flash is verified and successful[WHITE]\r\n");
              	 memset (read_buffer, 0, strlen (read_buffer));
              	 endpointChkFlag =true;
              }
              else
              {
              	printf_colour("\r\n [RED]Device symetric key saved in data flash is not saved correctly or corrupted[WHITE]\r\n");
              	endpointChkFlag =false;
              }
        }
    else
    {
    	printf_colour("\r\n [RED]Device symetric key point in data flash is not saved correctly or corrupted[WHITE]\r\n");
    	endpointChkFlag =false;
    }

    if((certificateChkFlag==true)&&(privatrKeyChkFlag==true)&&(iotThingChkFlag==true)&&(endpointChkFlag==true))
    {
    	printf_colour("\r\n [GREEN]All credentials in data flash is verified and successful[WHITE]\r\n");
    }
    else
    {
    	printf_colour("\r\n [RED]Please provide the credentials which is mentioned in RED[WHITE]\r\n");

    	return FSP_ERR_ABORTED;
    }

    return err;
}

