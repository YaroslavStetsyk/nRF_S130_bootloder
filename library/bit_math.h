#ifndef BIT_MATH_H__
#define BIT_MATH_H__

#include <stdint.h>


extern const uint8_t COMPRESSING_WIDTH;


uint32_t bit_math_compressing_len(uint32_t in_len, uint8_t in_width, uint8_t out_width);

uint8_t bit_compress_string(uint8_t * dest, uint8_t dest_len, uint8_t * str, uint8_t str_len);

uint8_t bit_decompress_string(uint8_t * str, uint8_t str_len, uint8_t * src, uint8_t src_len);


#endif	// BIT_MATH_H__
