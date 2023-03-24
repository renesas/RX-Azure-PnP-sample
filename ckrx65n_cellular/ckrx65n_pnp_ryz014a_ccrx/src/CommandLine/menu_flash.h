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
 * File Name    : menu_sensor.c
 * Version      : .
 * Description  : Data from the on-board sensors connected to i2c bus
 *********************************************************************************************************************/
#include "common_init.h"

#ifndef MENU_SENSOR_H_
#define MENU_SENSOR_H_

#define INFO                       		'a'
#define WRITE_ENDPOINT             		'b'
#define WRITE_ID_SCOPE             		'c'
#define WRITE_REGISTRATION_ID      		'd'
#define WRITE_DEVICE_SYMMETRIC_KEY   	'e'
#define READ_FLASH                 		'f'
#define CHECK_CREDENTIALS          		'g'
#define HELP                       		'h'

#define CREDENTIAL_COUNT                  (4)

#define MODULE_NAME_FLASH_INFO         "\r\nDATA FLASH INFO\r\n"
#define FLASH_WRITE_CERT_MENU          "\r\nDATA FLASH WRITE CERTIFICATE\r\n"
#define FLASH_WRITE_KEY_MENU           "\r\nDATA FLASH WRITE PRIVATE KEY\r\n"
#define FLASH_WRITE_THING_NAME          "\r\nDATA FLASH WRITE HOST NAME\r\n"
#define FLASH_WRITE_MQTT_END_POINT     "\r\nDATA FLASH WRITE DEVICE ID\r\n"
#define FLASH_CHECK_CREDENTIALS        "\r\nCHECK CREDENTIALS STORED IN DATA FLASH\r\n"
#define OPTIONS_SELECT                 "\r\n> Select from the options in the menu below:\r\n$"
#define OPTIONS_SELECT_FLASH           "\r\n> Select from the options in the menu below:\r\n"
#define SELECT_FILE                    "\r\n Select the file to write data in data flash \r\n$"
#define WRITE_HOST_NAME_STRING          "\r\n Copy & Paste Host name to write data in data flash \r\n$"
#define WRITE_DEVICE_ID_STRING          "\r\n Copy & Paste Device ID name to write data in data flash \r\n$"
#define MODULE_NAME_FLASH_READ         "\r\nDATA FLASH READ\r\n"
#define MODULE_NAME_FLASH_HELP         "\r\nHELP\r\n\r\n"
#define FLASH_HELP_TAG1                "  Please type a for flash memory information\r\n"
#define FLASH_HELP_TAG2                "  Please type b to save certificate\r\n"
#define FLASH_HELP_TAG3                "  Please type c to save Private Key\r\n"
#define FLASH_HELP_TAG4                "  Please type d to save Host name\r\n"
#define FLASH_HELP_TAG5                "  Please type e to save Device Id\r\n"
#define FLASH_HELP_TAG6                "  Please type f to read data saved in flash\r\n"
#define FLASH_HELP_TAG7                "  Please type g to check and validate credentials saved\r\n"


typedef enum e_credentials
{
	AZ_DEVICE_CER = 0,
	AZ_PVT_KEY,
	AZ_HOST_N,
	AZ_DEVICE_IDD
}credentials_t;


extern test_fn sensor_display_menu (void);
test_fn AWS_cert_key_write (void);
test_fn data_flash_menu(void);
void flash_info(void);
void help_menu(void);
void flash_display_menu(uint8_t credential_type);
fsp_err_t check_credentials_stored (void);
fsp_err_t flash_hp_data_read(bool print_data);
extern char g_write_buffer[2048];
extern uint8_t g_start_read_flag;
//int8_t flash_write_menu(int8_t selected_menu);

#define DATAFLASH_LABEL_NAME_TIMEZONE "time_zone"
#define DATAFLASH_LABEL_NAME_ENDPOINT "end_point"
//#define DATAFLASH_LABEL_NAME_ID_SCOPE "id_scope"
#define DATAFLASH_LABEL_NAME_REGISTRATION_ID "registration_id"
#define DATAFLASH_LABEL_NAME_DEVICE_SYMETRIC_KEY "device_symetric_key"
#define DATAFLASH_LABEL_NAME_CODE_SIGNER_CERTIFICATE "code_signer_certificate"


#define DATAFLASH_LABEL_NAME_HOST_NAME 				"host_name"
#define DATAFLASH_LABEL_NAME_DEVICE_ID_X509 		"device_id"
#define DATAFLASH_LABEL_NAME_DEVICE_CERTIFICATE 	"device_certificate"
#define DATAFLASH_LABEL_NAME_PRIVATE_KEY 			"private_key"


extern const char timezone_label[];
extern const char endpoint_label[];
extern const char id_scope_label[];
extern const char registration_id_label[];
extern const char device_symetric_key_label[];
//extern const char code_signer_certificate_label[];
extern const char tcp_send_performance_server_ip_address_label[];
extern const char tcp_send_performance_server_port_number_label[];


extern const char hostname_label[];
extern const char deviceID_label[];
extern const char device_cert_label[];
extern const char private_key_label[];




#endif /* MENU_SENSOR_H_ */
