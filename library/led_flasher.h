#ifndef LED_FLASHER_H_
#define LED_FLASHER_H_

#include <stdint.h>

#define LED_FLASH_UNLIMITED					0xFFFFFFFF
#define LED_FLASH_ONCE						0x00000000

typedef enum
{	
	COLOR_BLUE	= 0,
	COLOR_GREEN	= 1,
	COLOR_RED	= 2
}led_flasher_colors_e;

typedef enum
{
	LED_FLASHER_MODE_MUTUAL,
	LED_FLASHER_MODE_PARTIAL,
	LED_FLASHER_MODE_HASHTAG,
	LED_FLASHER_MODE_ADV,
	LED_FLASHER_MODE_COUNT
}led_flasher_mode_e;

typedef struct
{
	uint8_t		color;
	uint16_t	timespan;
	uint8_t		length;
	uint32_t	slots;
}led_flasher_pattern_t;


void led_flasher_init(void);

void led_flasher_init_patern(led_flasher_mode_e mode, led_flasher_pattern_t * pattern);

void led_flasher_start(led_flasher_mode_e mode, uint32_t duration);

void led_flasher_start_times(led_flasher_mode_e mode, uint16_t times);

void led_flasher_stop(void);


#endif //LED_FLASHER_H_
