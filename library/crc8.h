#ifndef CRC8_H__
#define CRC8_H__

#include <stdint.h>


uint8_t crc8(const uint8_t *data, uint8_t data_len, uint8_t crc);

uint8_t crc8_simple(const uint8_t *data, uint8_t data_len, uint8_t crc);


#endif // CRC8_H__
