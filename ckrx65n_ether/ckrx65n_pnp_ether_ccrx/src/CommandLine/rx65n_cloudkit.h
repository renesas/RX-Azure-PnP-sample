/*
 * rx65n_cloudkit.h
 *
 *  Created on: 24-May-2022
 *      Author: 27779
 */

#ifndef APPLICATION_CODE_COMMANDLINE_RX65N_CLOUDKIT_H_
#define APPLICATION_CODE_COMMANDLINE_RX65N_CLOUDKIT_H_
#include "menu_flash.h"
#include "stdint.h"
#include "stdio.h"

const char timezone_label[] = DATAFLASH_LABEL_NAME_TIMEZONE;
const char endpoint_label[] = DATAFLASH_LABEL_NAME_ENDPOINT;
//const char id_scope_label[] = DATAFLASH_LABEL_NAME_ID_SCOPE;
const char registration_id_label[] = DATAFLASH_LABEL_NAME_REGISTRATION_ID;
const char device_symetric_key_label[] = DATAFLASH_LABEL_NAME_DEVICE_SYMETRIC_KEY;


const char hostname_label[]=DATAFLASH_LABEL_NAME_HOST_NAME;
const char deviceID_label[]=DATAFLASH_LABEL_NAME_DEVICE_ID_X509;
const char device_cert_label[]=DATAFLASH_LABEL_NAME_DEVICE_CERTIFICATE;
const char private_key_label[]=DATAFLASH_LABEL_NAME_PRIVATE_KEY;
//const char code_signer_certificate_label[] = DATAFLASH_LABEL_NAME_CODE_SIGNER_CERTIFICATE;

#endif /* APPLICATION_CODE_COMMANDLINE_RX65N_CLOUDKIT_H_ */
