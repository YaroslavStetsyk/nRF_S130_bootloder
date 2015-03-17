#include <stdint.h>

#include "crc8.h"


uint8_t crc8(const uint8_t *data, uint8_t data_len, uint8_t crc)
{
	static const uint8_t crcpoly = 0x8C;
	uint8_t index = 8;
	uint8_t fb;
	uint8_t i;
	uint8_t byte;
	
	for(i = 0; i<data_len; ++i)
	{
		index = 8;
		byte = data[i];
		do
		{
			fb = (crc ^ byte) & 0x01;
			byte >>= 1;
			crc >>= 1;
			if (fb)
				crc ^= crcpoly;

		}while (--index);
	}

	return crc;
}

uint8_t crc8_simple(const uint8_t *data, uint8_t data_len, uint8_t crc)
{
	uint8_t i;
	
	for(i = 0; i<data_len; ++i)
	{
		crc += data[i];
	}
	
	return crc;
}
