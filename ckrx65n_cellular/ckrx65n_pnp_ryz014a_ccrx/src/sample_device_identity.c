/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

#include "nx_api.h"

/* Device certificate.  */
#ifndef DEVICE_CERT
#define DEVICE_CERT                                 {0x00}
#endif /* DEVICE_CERT */

/* Device Private Key.  */
#ifndef DEVICE_PRIVATE_KEY
#define DEVICE_PRIVATE_KEY                          {0x00}
#endif /* DEVICE_PRIVATE_KEY */

/*const UCHAR sample_device_cert_ptr[] = {
		  0x30, 0x82, 0x02, 0xb7, 0x30, 0x82, 0x01, 0x9f, 0x02, 0x14, 0x0b, 0x6d,
		  0xec, 0xf7, 0x6b, 0x4e, 0xa3, 0x63, 0xc8, 0xa5, 0x9b, 0x8a, 0x0a, 0xe6,
		  0x10, 0xe7, 0xbe, 0xdb, 0x3b, 0x5c, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86,
		  0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x30, 0x18, 0x31,
		  0x16, 0x30, 0x14, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x0d, 0x43, 0x4b,
		  0x5f, 0x52, 0x41, 0x36, 0x4d, 0x35, 0x5f, 0x58, 0x35, 0x30, 0x39, 0x30,
		  0x1e, 0x17, 0x0d, 0x32, 0x32, 0x31, 0x32, 0x30, 0x38, 0x31, 0x30, 0x31,
		  0x39, 0x30, 0x34, 0x5a, 0x17, 0x0d, 0x32, 0x33, 0x31, 0x32, 0x30, 0x38,
		  0x31, 0x30, 0x31, 0x39, 0x30, 0x34, 0x5a, 0x30, 0x18, 0x31, 0x16, 0x30,
		  0x14, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x0d, 0x43, 0x4b, 0x5f, 0x52,
		  0x41, 0x36, 0x4d, 0x35, 0x5f, 0x58, 0x35, 0x30, 0x39, 0x30, 0x82, 0x01,
		  0x22, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01,
		  0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0f, 0x00, 0x30, 0x82, 0x01,
		  0x0a, 0x02, 0x82, 0x01, 0x01, 0x00, 0xb6, 0xf3, 0x58, 0xf5, 0x3f, 0x97,
		  0x88, 0xfa, 0x95, 0x2a, 0x67, 0x43, 0x5a, 0xed, 0xa3, 0xe2, 0x71, 0x6f,
		  0xa3, 0x12, 0x0e, 0x30, 0xdc, 0x33, 0x94, 0x6e, 0xd4, 0xdb, 0x5e, 0x2b,
		  0x18, 0x0b, 0xd1, 0x20, 0x2a, 0xb1, 0x57, 0x5c, 0xb5, 0x5e, 0x1c, 0x10,
		  0x98, 0xaf, 0x71, 0xdd, 0x38, 0xcf, 0xae, 0x04, 0x64, 0xb8, 0x56, 0x48,
		  0x71, 0xeb, 0x45, 0x75, 0x85, 0x56, 0x46, 0x76, 0xd2, 0x4f, 0x56, 0xcb,
		  0xad, 0x67, 0x5f, 0xe0, 0x26, 0xf4, 0x0a, 0xb2, 0x17, 0xce, 0x88, 0x9d,
		  0x4c, 0x95, 0x51, 0xa3, 0x5d, 0x80, 0x93, 0xf5, 0xe7, 0x2b, 0xc6, 0x59,
		  0xfe, 0xe6, 0x9a, 0x5f, 0x50, 0x41, 0xa5, 0x9e, 0x2c, 0xcf, 0x3f, 0x9f,
		  0x12, 0xb9, 0x65, 0xc0, 0x80, 0x4b, 0x36, 0xc3, 0x1e, 0x3f, 0xca, 0xdd,
		  0x0d, 0x4a, 0xc2, 0x07, 0x49, 0xb0, 0xef, 0x87, 0x3d, 0xcb, 0xd5, 0x4c,
		  0xe7, 0x4d, 0xe1, 0x22, 0x0b, 0x07, 0xdb, 0x8c, 0x78, 0xb0, 0xba, 0x11,
		  0x43, 0xfc, 0x9c, 0x46, 0xc7, 0xcb, 0x16, 0x4a, 0xa5, 0xa5, 0x41, 0x2a,
		  0x03, 0x6a, 0x51, 0x0c, 0x53, 0xc3, 0x5c, 0x43, 0x89, 0xa9, 0xb8, 0xc7,
		  0x04, 0x25, 0x7c, 0x97, 0xf3, 0x36, 0xb9, 0x70, 0xfc, 0x08, 0xec, 0x27,
		  0xa8, 0x2e, 0x77, 0x27, 0x74, 0x7f, 0x40, 0xa9, 0x91, 0x67, 0xa5, 0x98,
		  0xd6, 0xc6, 0x50, 0x5d, 0x46, 0x00, 0x64, 0x0d, 0x67, 0xe1, 0xa8, 0xc1,
		  0xc8, 0x22, 0x14, 0x9d, 0xe3, 0x26, 0x0d, 0x88, 0x28, 0x87, 0x26, 0x8c,
		  0x87, 0xf6, 0xdd, 0x38, 0xa4, 0x80, 0x8c, 0x29, 0xd1, 0x25, 0x89, 0xe3,
		  0x58, 0x42, 0x24, 0xe7, 0xcd, 0x4d, 0xd8, 0xec, 0x2c, 0x6d, 0xcb, 0x58,
		  0x36, 0x08, 0x6d, 0x9e, 0x14, 0x38, 0xda, 0x98, 0x5f, 0xaa, 0x8d, 0x80,
		  0x43, 0x41, 0x3c, 0x01, 0x16, 0x06, 0x20, 0x3c, 0xd0, 0xa3, 0x02, 0x03,
		  0x01, 0x00, 0x01, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7,
		  0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x03, 0x82, 0x01, 0x01, 0x00, 0x55,
		  0x5a, 0x35, 0xce, 0xbc, 0xda, 0x9e, 0x5c, 0xde, 0xa5, 0xae, 0x0a, 0xc2,
		  0x34, 0x8a, 0x41, 0xe8, 0x78, 0x22, 0x04, 0xfb, 0x10, 0xdd, 0xf0, 0xb6,
		  0x05, 0x29, 0x1a, 0xe5, 0x3c, 0x3a, 0x0f, 0x3c, 0xc1, 0xe4, 0xf0, 0x09,
		  0xfb, 0x6e, 0xab, 0xc6, 0xdb, 0x27, 0xc2, 0x67, 0x63, 0xdc, 0xe4, 0x7a,
		  0x39, 0x0c, 0x07, 0xa1, 0x5d, 0xa6, 0x27, 0x4b, 0x69, 0x0b, 0xa6, 0x4d,
		  0x23, 0x98, 0xe5, 0xcc, 0x23, 0x56, 0x22, 0xd1, 0xb1, 0x84, 0xe1, 0x5a,
		  0x1c, 0x98, 0x66, 0x8c, 0x85, 0x51, 0xa5, 0xad, 0xf1, 0x82, 0xc7, 0xd7,
		  0x8f, 0x39, 0x56, 0x6f, 0x8e, 0xbb, 0x28, 0x90, 0x21, 0xcd, 0x23, 0xd3,
		  0x35, 0xfe, 0xb2, 0xb6, 0xba, 0xa4, 0x4b, 0xc6, 0x94, 0x7f, 0x65, 0xb1,
		  0x02, 0xdc, 0xaf, 0x21, 0x20, 0x1c, 0xbe, 0xa3, 0x00, 0x7e, 0x30, 0xf6,
		  0xcf, 0xdd, 0xc9, 0xb7, 0x2d, 0xc3, 0xb9, 0x13, 0x8e, 0xde, 0x15, 0xe5,
		  0x82, 0xc6, 0xbf, 0xa0, 0xf7, 0x91, 0x22, 0x02, 0x90, 0xfd, 0xcc, 0x6e,
		  0xe0, 0xaf, 0x79, 0x7b, 0x74, 0x38, 0xbc, 0x63, 0xc5, 0x5f, 0x61, 0x39,
		  0x6d, 0xc6, 0x8c, 0x2a, 0xd0, 0x17, 0x32, 0xe4, 0x2f, 0xa1, 0xb9, 0xbd,
		  0x97, 0x61, 0xa2, 0x25, 0x2c, 0xf9, 0x35, 0x44, 0x06, 0x5f, 0x3c, 0xe5,
		  0x05, 0x3a, 0x07, 0x0e, 0x17, 0x97, 0x16, 0xd2, 0x39, 0x68, 0x02, 0x84,
		  0x04, 0x70, 0xaa, 0x92, 0xec, 0xac, 0x15, 0xa3, 0x6a, 0xbc, 0x55, 0x7f,
		  0x24, 0x2b, 0xbe, 0x5a, 0xfa, 0x4c, 0x21, 0xe1, 0xcf, 0x7e, 0x39, 0x72,
		  0x16, 0x8c, 0x90, 0xd9, 0x40, 0x86, 0x81, 0x5c, 0xe5, 0x01, 0x32, 0x4a,
		  0xf4, 0x5f, 0x44, 0x42, 0x1a, 0x9d, 0x25, 0x51, 0xf3, 0x9c, 0xb7, 0x4c,
		  0xf0, 0xe7, 0x60, 0x34, 0x58, 0x2a, 0x47, 0x3e, 0x3b, 0x12, 0x38, 0xdd,
		  0xfe, 0x98, 0x16
		};
const UINT sample_device_cert_len = sizeof(sample_device_cert_ptr);
const UCHAR sample_device_private_key_ptr[] = {
		  0x30, 0x82, 0x04, 0xa3, 0x02, 0x01, 0x00, 0x02, 0x82, 0x01, 0x01, 0x00,
		  0xb6, 0xf3, 0x58, 0xf5, 0x3f, 0x97, 0x88, 0xfa, 0x95, 0x2a, 0x67, 0x43,
		  0x5a, 0xed, 0xa3, 0xe2, 0x71, 0x6f, 0xa3, 0x12, 0x0e, 0x30, 0xdc, 0x33,
		  0x94, 0x6e, 0xd4, 0xdb, 0x5e, 0x2b, 0x18, 0x0b, 0xd1, 0x20, 0x2a, 0xb1,
		  0x57, 0x5c, 0xb5, 0x5e, 0x1c, 0x10, 0x98, 0xaf, 0x71, 0xdd, 0x38, 0xcf,
		  0xae, 0x04, 0x64, 0xb8, 0x56, 0x48, 0x71, 0xeb, 0x45, 0x75, 0x85, 0x56,
		  0x46, 0x76, 0xd2, 0x4f, 0x56, 0xcb, 0xad, 0x67, 0x5f, 0xe0, 0x26, 0xf4,
		  0x0a, 0xb2, 0x17, 0xce, 0x88, 0x9d, 0x4c, 0x95, 0x51, 0xa3, 0x5d, 0x80,
		  0x93, 0xf5, 0xe7, 0x2b, 0xc6, 0x59, 0xfe, 0xe6, 0x9a, 0x5f, 0x50, 0x41,
		  0xa5, 0x9e, 0x2c, 0xcf, 0x3f, 0x9f, 0x12, 0xb9, 0x65, 0xc0, 0x80, 0x4b,
		  0x36, 0xc3, 0x1e, 0x3f, 0xca, 0xdd, 0x0d, 0x4a, 0xc2, 0x07, 0x49, 0xb0,
		  0xef, 0x87, 0x3d, 0xcb, 0xd5, 0x4c, 0xe7, 0x4d, 0xe1, 0x22, 0x0b, 0x07,
		  0xdb, 0x8c, 0x78, 0xb0, 0xba, 0x11, 0x43, 0xfc, 0x9c, 0x46, 0xc7, 0xcb,
		  0x16, 0x4a, 0xa5, 0xa5, 0x41, 0x2a, 0x03, 0x6a, 0x51, 0x0c, 0x53, 0xc3,
		  0x5c, 0x43, 0x89, 0xa9, 0xb8, 0xc7, 0x04, 0x25, 0x7c, 0x97, 0xf3, 0x36,
		  0xb9, 0x70, 0xfc, 0x08, 0xec, 0x27, 0xa8, 0x2e, 0x77, 0x27, 0x74, 0x7f,
		  0x40, 0xa9, 0x91, 0x67, 0xa5, 0x98, 0xd6, 0xc6, 0x50, 0x5d, 0x46, 0x00,
		  0x64, 0x0d, 0x67, 0xe1, 0xa8, 0xc1, 0xc8, 0x22, 0x14, 0x9d, 0xe3, 0x26,
		  0x0d, 0x88, 0x28, 0x87, 0x26, 0x8c, 0x87, 0xf6, 0xdd, 0x38, 0xa4, 0x80,
		  0x8c, 0x29, 0xd1, 0x25, 0x89, 0xe3, 0x58, 0x42, 0x24, 0xe7, 0xcd, 0x4d,
		  0xd8, 0xec, 0x2c, 0x6d, 0xcb, 0x58, 0x36, 0x08, 0x6d, 0x9e, 0x14, 0x38,
		  0xda, 0x98, 0x5f, 0xaa, 0x8d, 0x80, 0x43, 0x41, 0x3c, 0x01, 0x16, 0x06,
		  0x20, 0x3c, 0xd0, 0xa3, 0x02, 0x03, 0x01, 0x00, 0x01, 0x02, 0x82, 0x01,
		  0x00, 0x58, 0x6f, 0xc9, 0xd0, 0xc3, 0xac, 0xbc, 0xca, 0x64, 0xd0, 0xda,
		  0x99, 0xf4, 0x9a, 0x6b, 0x5e, 0x4a, 0xf7, 0x6c, 0x1c, 0x8e, 0x3f, 0x90,
		  0x32, 0x5e, 0xce, 0x34, 0x96, 0xd6, 0x2e, 0x7e, 0xb1, 0x37, 0xa6, 0x14,
		  0xfd, 0x24, 0xfd, 0xeb, 0xae, 0x52, 0x62, 0xb3, 0x6c, 0x1b, 0xde, 0x94,
		  0x57, 0xd0, 0x06, 0xcf, 0xa9, 0x4c, 0x0c, 0x39, 0x05, 0x41, 0xce, 0x4d,
		  0x66, 0x27, 0x29, 0xc9, 0x00, 0x11, 0x34, 0xe3, 0xdd, 0xd4, 0xba, 0xc6,
		  0x19, 0xb2, 0x11, 0xe4, 0xf0, 0x31, 0x99, 0xd8, 0x2b, 0x56, 0x0b, 0x62,
		  0x67, 0x85, 0x1e, 0x99, 0x60, 0xcf, 0xc1, 0x31, 0x36, 0xc7, 0xe8, 0xcb,
		  0x22, 0xf0, 0xac, 0x45, 0x7c, 0xe9, 0x84, 0x3b, 0xc8, 0xe3, 0x26, 0xea,
		  0xdd, 0x70, 0xdb, 0x61, 0x03, 0xf5, 0x4c, 0x65, 0xe0, 0x7d, 0x6a, 0x88,
		  0x74, 0xbf, 0x54, 0xe0, 0xca, 0x67, 0x91, 0x85, 0x3d, 0x8f, 0x01, 0xff,
		  0x91, 0x97, 0xf3, 0xa9, 0x50, 0x9d, 0xd3, 0x44, 0x88, 0x31, 0x68, 0x27,
		  0xbc, 0x12, 0x13, 0xfb, 0xbe, 0x32, 0x07, 0xd6, 0xe9, 0x9e, 0x28, 0xf4,
		  0x57, 0x54, 0xef, 0x70, 0xfc, 0x8d, 0x3e, 0x1d, 0x44, 0xb0, 0xf2, 0x1f,
		  0x78, 0x9e, 0x14, 0xfb, 0x24, 0x7c, 0xf4, 0x49, 0x0e, 0x54, 0xf6, 0x2c,
		  0x9f, 0x2f, 0xd0, 0xba, 0x1d, 0x9c, 0xa0, 0xfa, 0x73, 0xfd, 0xaf, 0xd2,
		  0xd6, 0x1d, 0xd9, 0xbd, 0x39, 0x6f, 0xc9, 0xdc, 0x48, 0x6f, 0xbe, 0x4a,
		  0x7d, 0x86, 0x5c, 0x91, 0xad, 0x1c, 0x13, 0xd9, 0x8b, 0x18, 0xd3, 0xd0,
		  0x64, 0x98, 0x4f, 0x9b, 0xfd, 0x94, 0xe9, 0x27, 0xf0, 0x49, 0x87, 0x02,
		  0x73, 0x61, 0xfd, 0xbd, 0x7f, 0x3c, 0x11, 0x29, 0x59, 0x55, 0xa2, 0xcf,
		  0x7a, 0xf1, 0xd8, 0xa1, 0x42, 0xdc, 0x33, 0x1c, 0x83, 0x5f, 0x5c, 0x97,
		  0xec, 0xa1, 0xa5, 0xcc, 0x01, 0x02, 0x81, 0x81, 0x00, 0xdf, 0x43, 0x65,
		  0xa1, 0x15, 0xb1, 0x82, 0x8d, 0x2d, 0xb0, 0xdd, 0xbc, 0xbe, 0x3c, 0x69,
		  0x89, 0xaa, 0x8c, 0x2c, 0xa8, 0xa3, 0xc8, 0x54, 0xbb, 0x59, 0xc0, 0x06,
		  0xc6, 0x8a, 0x1c, 0xaf, 0x09, 0x65, 0x22, 0x7b, 0x8d, 0x94, 0x36, 0xe9,
		  0x11, 0x2a, 0xf6, 0xcf, 0xe2, 0x9d, 0x05, 0x08, 0xd5, 0xfa, 0xa7, 0x62,
		  0xa0, 0x58, 0x90, 0xad, 0x50, 0x1a, 0x9d, 0x98, 0x90, 0xae, 0x49, 0x92,
		  0x81, 0x31, 0x61, 0x8a, 0xa9, 0xe8, 0x52, 0x3e, 0x24, 0xb1, 0x25, 0x30,
		  0x12, 0x8a, 0x8b, 0x57, 0xfd, 0x4b, 0x4e, 0x9f, 0x1f, 0x0b, 0xfe, 0x6e,
		  0x4f, 0x28, 0xef, 0xe2, 0x5b, 0x87, 0xe3, 0xb2, 0x43, 0x15, 0xbf, 0x4e,
		  0x90, 0xad, 0xb0, 0xa6, 0x95, 0xec, 0x59, 0x87, 0xb8, 0x32, 0x1e, 0xa5,
		  0x34, 0x51, 0x65, 0xc8, 0x5d, 0xaf, 0xbb, 0xd3, 0x20, 0x0a, 0xfd, 0x4b,
		  0xf9, 0x4d, 0x68, 0x20, 0xc9, 0x02, 0x81, 0x81, 0x00, 0xd1, 0xc6, 0xbd,
		  0x08, 0x75, 0x22, 0x52, 0x29, 0x2d, 0xf6, 0x1a, 0x00, 0xda, 0x8b, 0xf9,
		  0xa1, 0xaa, 0x31, 0x2c, 0x4c, 0x68, 0x10, 0x6a, 0x04, 0x63, 0x3c, 0xa2,
		  0xd5, 0xf5, 0x43, 0xc4, 0x8c, 0xb0, 0x42, 0x71, 0xd0, 0xae, 0x87, 0xb1,
		  0xe1, 0xd3, 0xab, 0x19, 0x95, 0x90, 0x9d, 0xa9, 0x26, 0x98, 0x64, 0x21,
		  0x36, 0x98, 0x95, 0x30, 0xc6, 0xae, 0x29, 0x15, 0xb3, 0x5e, 0x8e, 0x23,
		  0x28, 0x6f, 0xd3, 0xfc, 0x17, 0xd7, 0x2a, 0x23, 0xf5, 0x88, 0xa7, 0x4a,
		  0xa4, 0x2e, 0xfe, 0xb0, 0x65, 0x48, 0xdf, 0xde, 0x3f, 0xe1, 0x02, 0x8a,
		  0x8d, 0xa1, 0xe6, 0x3e, 0xd8, 0x17, 0xec, 0x89, 0xdd, 0x0b, 0x17, 0x54,
		  0xe4, 0xb2, 0x55, 0xa4, 0xe5, 0xe2, 0xff, 0xee, 0x19, 0x6b, 0xa0, 0x3b,
		  0x93, 0xc8, 0x22, 0x67, 0x7f, 0x91, 0xd2, 0xfa, 0x4a, 0x99, 0x39, 0xa5,
		  0x41, 0xa0, 0x84, 0x28, 0x0b, 0x02, 0x81, 0x81, 0x00, 0xbb, 0x25, 0x4c,
		  0x90, 0x4d, 0x95, 0xe1, 0x40, 0x24, 0x37, 0xa8, 0x76, 0xe8, 0xe3, 0xcc,
		  0xf7, 0x87, 0xcd, 0x3e, 0x36, 0x12, 0x2b, 0xff, 0xd2, 0x97, 0xb5, 0xde,
		  0x38, 0x18, 0x57, 0x58, 0xe5, 0xc0, 0x2f, 0xba, 0x6f, 0x7f, 0xf8, 0xd3,
		  0x3e, 0xff, 0xcb, 0xa9, 0x90, 0x64, 0x5c, 0x6e, 0xfe, 0xa6, 0xc0, 0x1e,
		  0x6d, 0x18, 0x30, 0x16, 0x07, 0x1a, 0x78, 0x56, 0x05, 0x99, 0x02, 0xb4,
		  0xda, 0xb4, 0xa0, 0xf5, 0xd1, 0xdc, 0x9a, 0xeb, 0xda, 0x67, 0xdf, 0x2b,
		  0x3f, 0xaf, 0x80, 0x84, 0x3e, 0x1f, 0x1d, 0x05, 0x46, 0xca, 0x0f, 0xf9,
		  0x39, 0x43, 0xb0, 0x69, 0x06, 0x9e, 0x8e, 0xdf, 0xc3, 0x59, 0xfc, 0x65,
		  0x87, 0xe3, 0x00, 0x3b, 0x73, 0x5a, 0x8d, 0xe5, 0xb5, 0x21, 0xd6, 0x0c,
		  0x57, 0x21, 0x8f, 0xf6, 0xf7, 0x89, 0x32, 0x21, 0x14, 0x30, 0x02, 0x08,
		  0xe8, 0x2d, 0xd9, 0x85, 0xf9, 0x02, 0x81, 0x80, 0x6b, 0x91, 0xa3, 0x81,
		  0x60, 0xbf, 0x36, 0xae, 0x53, 0x19, 0x2d, 0x80, 0x5e, 0x91, 0x04, 0x72,
		  0xf7, 0xd8, 0xbb, 0x44, 0x7a, 0xe0, 0x5f, 0xdc, 0xdc, 0x1e, 0x4d, 0xc9,
		  0xb5, 0x39, 0x08, 0xe2, 0xb3, 0xc7, 0x7e, 0xa1, 0x69, 0xbd, 0xbf, 0x63,
		  0x73, 0xac, 0xfb, 0x33, 0x8c, 0xb2, 0xab, 0xf8, 0x54, 0x90, 0x93, 0xcf,
		  0x71, 0x32, 0x9f, 0x8e, 0x61, 0xe4, 0x43, 0x72, 0x74, 0xfc, 0x21, 0xdf,
		  0x2c, 0x68, 0xe1, 0x8e, 0x40, 0x11, 0x25, 0xdb, 0x54, 0x35, 0xa4, 0xba,
		  0xc8, 0x36, 0x65, 0xb1, 0xaa, 0x4a, 0xd1, 0xa6, 0xa3, 0xf5, 0x2f, 0xa3,
		  0x2c, 0xef, 0xea, 0x2d, 0xd9, 0x26, 0xbc, 0x13, 0x13, 0x0a, 0xe5, 0xc7,
		  0x58, 0x03, 0x3e, 0xc9, 0x7b, 0x3b, 0x8d, 0x96, 0x8f, 0x6f, 0xdc, 0xdd,
		  0x0c, 0xf5, 0x47, 0x69, 0x4d, 0x17, 0x88, 0xad, 0x14, 0x21, 0x51, 0x32,
		  0x73, 0xe3, 0x10, 0xbd, 0x02, 0x81, 0x80, 0x2c, 0x15, 0x67, 0x88, 0x2d,
		  0x19, 0xde, 0x5c, 0x29, 0x0c, 0x1b, 0xd6, 0x46, 0x30, 0x4f, 0x68, 0xc7,
		  0xf1, 0xc3, 0x95, 0x7d, 0x36, 0x73, 0x32, 0x40, 0x17, 0x84, 0xdb, 0x36,
		  0xea, 0x4e, 0x14, 0x2b, 0x68, 0x67, 0x61, 0xe1, 0x18, 0x7b, 0x33, 0x56,
		  0x22, 0xbe, 0x7e, 0x66, 0xa8, 0xc0, 0xf6, 0x66, 0x77, 0xe9, 0x16, 0xeb,
		  0x2e, 0x25, 0x16, 0xaa, 0xea, 0x3e, 0x9d, 0x5d, 0x7b, 0x9e, 0xb2, 0x8e,
		  0x31, 0x37, 0x49, 0x5f, 0x4a, 0x35, 0x9b, 0x24, 0x4c, 0xbf, 0x46, 0xc0,
		  0x3b, 0xee, 0x83, 0x92, 0xc1, 0x92, 0xd3, 0xb5, 0x12, 0xb1, 0xb2, 0x11,
		  0xcd, 0x71, 0xa9, 0x65, 0x83, 0x4c, 0xae, 0x76, 0xf0, 0x96, 0x94, 0x2b,
		  0x2b, 0x0b, 0x45, 0x96, 0x87, 0x8c, 0x28, 0x10, 0x16, 0x02, 0x31, 0x72,
		  0xc8, 0xb8, 0x3b, 0x08, 0x43, 0x9d, 0xfc, 0x80, 0xa9, 0x1f, 0x83, 0xb4,
		  0xee, 0xd8, 0x3e
		};
const UINT sample_device_private_key_len = sizeof(sample_device_private_key_ptr);*/