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

uint16_t calc_crc16_len16(uint8_t *frame, uint16_t flen);
uint16_t calc_crc16_len32(uint8_t *frame, uint32_t flen);

crc_err_t check_crc16_len16(uint8_t *frame, uint16_t flen);
crc_err_t check_crc16_len32(uint8_t *frame, uint32_t flen);

void init_crc16(void);

int16_t get_crc16_from_value(int16_t crc_init_value, uint8_t *data, uint32_t flen);
uint16_t get_crc16_final(int16_t crcvalue);

#endif /* CRC16_H_ */
