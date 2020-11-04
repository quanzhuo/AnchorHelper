/*! ------------------------------------------------------------------------------------------------------------------
* @file	crc16.h
* @brief
*
* @attention
*
* Copyright 2014-2015 (c) DecaWave Ltd, Dublin, Ireland.
*
* All rights reserved.
*
* @author
*
* DecaWave
*/
#ifndef CRC16_H_
#define CRC16_H_

#include <stdint.h>

typedef enum {
	CRC_OKAY = 0,
	CRC_ERROR = -1
}crc_err_t;

uint16_t calc_crc16(uint8_t *frame, uint16_t flen);

crc_err_t check_crc16(uint8_t *frame, uint16_t flen);

void init_crc16(void);

#endif /* CRC16_H_ */
