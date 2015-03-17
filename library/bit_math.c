#include <stdint.h>
#include "bit_math.h"

static const uint8_t mask[] = {		0x00,//0b00000000
									0x01,//0b00000001
									0x03,//0b00000011
									0x07,//0b00000111
									0x0F,//0b00001111
									0x1F,//0b00011111
									0x3F,//0b00111111	
									0x7F,//0b01111111
									0xFF,//0b11111111
								};

const uint8_t COMPRESSING_WIDTH = 5;								
								
uint32_t bit_math_compressing_len(uint32_t in_len, uint8_t in_width, uint8_t out_width)
{
	uint32_t result = 0;
	
	result = (in_len * out_width) / in_width;
	if((in_len * out_width) % in_width)
		result += 1;
	
	return result;
}


uint8_t bit_compress_string(uint8_t * dest, uint8_t dest_len, uint8_t * str, uint8_t str_len)
{
	uint8_t dst_bit_cnt = 0;
	uint8_t res_len;
	uint8_t scr_byte = 0;
	int8_t bit_offset;
	const uint8_t out_width = COMPRESSING_WIDTH;

	res_len = bit_math_compressing_len( str_len, 8, out_width );
	if(res_len > dest_len)
	{
		res_len = dest_len;
	}

	for(uint8_t i = 0; i<str_len && (dst_bit_cnt / 8) < res_len; ++i, dst_bit_cnt += out_width)
	{
		scr_byte = 0;
		bit_offset = (int16_t)(8 - out_width) - (dst_bit_cnt % 8);

		if(str[i] >= 'a' && str[i] <= 'z')
			scr_byte = str[i] - 'a' + 1;

		if(bit_offset > 0)
		{
			dest[dst_bit_cnt / 8] |= ((scr_byte & mask[out_width]) << bit_offset);;
		}
		else if(bit_offset < 0)
		{
			bit_offset = -bit_offset;
			dest[dst_bit_cnt / 8] |= (scr_byte & mask[out_width]) >> (bit_offset);;

			if(((dst_bit_cnt + out_width) / 8) < res_len)
			{
				dest[(dst_bit_cnt + out_width) / 8] |= (scr_byte & mask[out_width]) << (8 - bit_offset);;
			}
		}
		else
		{
			dest[dst_bit_cnt / 8] |= (scr_byte & mask[out_width]);
		}
	}

	return res_len;
}


uint8_t bit_decompress_string(uint8_t * str, uint8_t str_len, uint8_t * src, uint8_t src_len)
{
	uint8_t src_bit_cnt = 0;
	uint8_t res_len;
	uint8_t src_byte = 0;
	uint8_t bit_offset;
	uint8_t bit_offset_end;
	const uint8_t in_width = COMPRESSING_WIDTH;

	res_len = bit_math_compressing_len( src_len, in_width, 8 );
	if(res_len > str_len)
	{
		res_len = str_len;
	}

	for(uint8_t i = 0; i<res_len; ++i, src_bit_cnt += in_width)
	{
		src_byte = 0;
		bit_offset = (src_bit_cnt % 8);
		bit_offset_end = bit_offset + in_width; 
		// within one src bytes 
		if (bit_offset_end <= 8)
		{
			src_byte = src[src_bit_cnt / 8] >> (8 - bit_offset_end);
			str[i] = src_byte & mask[in_width];
		}
		else // within two bytes
		{
			src_byte = src[src_bit_cnt / 8] << (in_width - (8 - bit_offset));
			str[i] = src_byte & mask[in_width];
			src_byte = src[(src_bit_cnt + in_width) / 8] >> (16 - bit_offset_end);
			str[i] |= src_byte;
		}

		if(str[i])
			str[i] = str[i] + 'a' - 1;
	}

	return res_len;
}
