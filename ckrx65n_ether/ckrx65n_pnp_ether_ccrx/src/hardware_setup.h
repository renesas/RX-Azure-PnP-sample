
#ifndef HARDWARE_SETUP_H
#define HARDWARE_SETUP_H
#include"stdint.h"
#include "stdio.h"
#define SERIAL_BUFFER_QUEUE_NUMBER 512
#define SERIAL_BUFFER_SIZE 1
#define SCI_BUFFER_SIZE 256
#define COMMAND_SIZE 16
#define ARGUMENT1_SIZE 256

#define MESSAGE_BUFFER_SIZE 512

#define COMMAND_UNKNOWN -1
#define COMMAND_START 1
#define COMMAND_DATAFLASH 5

#define DATAFLASH_LABEL_NAME_ID_SCOPE "idscope"
#define DATAFLASH_LABEL_NAME_DEVICE_ID "deviceid"
#define DATAFLASH_LABEL_NAME_PRIMARY_KEY "primary_key"

typedef enum
{
	DEMO_ID_SCOPE = 0,
	DEMO_DEVICE_ID,
	DEMO_PRIMARY_KEY
} connecting_info;

void platform_setup(void);
void sci_callback(void *pArgs);
void demo_get_connecting_info(connecting_info label, uint8_t* info);

#endif // HARDWARE_SETUP_H
