#include "boards.h"
#include "app_timer.h"
#include "nrf_gpio.h"

#include "led_flasher.h"


#if defined(BOARD_PCA10001)
	#define LED_RGB_RED					LED_1
	#define LED_RGB_GREEN				LED_0
	#define LED_RGB_BLUE				LED_RGB_GREEN
	#define SET_LED_ON(led_id)			nrf_gpio_pin_set(led_id)
	#define SET_LED_OFF(led_id)			nrf_gpio_pin_clear(led_id)	
	
#elif defined(BOARD_PCA10000)
	#define SET_LED_ON(led_id)			nrf_gpio_pin_clear(led_id)	
	#define SET_LED_OFF(led_id)			nrf_gpio_pin_set(led_id)
	
#elif defined(BOARD_BEACON)
	#define LED_RGB_RED					16
	#define LED_RGB_GREEN				12
	#define LED_RGB_BLUE				15
	#define SET_LED_ON(led_id)			nrf_gpio_pin_clear(led_id)	
	#define SET_LED_OFF(led_id)			nrf_gpio_pin_set(led_id)
	
	
#elif defined(BOARD_BEACON_CUSTOM_LED)
	#define LED_RGB_RED					2
	#define LED_RGB_GREEN				0
	#define LED_RGB_BLUE				1
	#define SET_LED_ON(led_id)			nrf_gpio_pin_clear(led_id)	
	#define SET_LED_OFF(led_id)			nrf_gpio_pin_set(led_id)
#endif


#define ARRAY_LENGTH(arr)	(sizeof(arr) / sizeof(arr[0]))


typedef struct
{
	led_flasher_pattern_t	pattern;
	uint32_t				duration;
	uint8_t					id;
}led_flasher_task_t;

static app_timer_id_t		led_flasher_timer_id;

static led_flasher_task_t	flasher_tasks[LED_FLASHER_MODE_COUNT];

static led_flasher_task_t * task = NULL;


static void led_flasher_handler(void * p_context)
{
	static uint8_t				frame = 0;
	static led_flasher_task_t	current_task;

	if (p_context)
	{
		current_task = *(led_flasher_task_t*)p_context;
		
		// if current task isn't finished
		if (current_task.pattern.length > frame)
		{
			led_flasher_pattern_t pattern = current_task.pattern;
			uint16_t timespan = current_task.pattern.timespan;
			
			if (pattern.slots & (1 << (31 - frame)))
			{			
				if (pattern.color & (1 << COLOR_RED))	SET_LED_ON(LED_RGB_RED);
				if (pattern.color & (1 << COLOR_GREEN))	SET_LED_ON(LED_RGB_GREEN);
				if (pattern.color & (1 << COLOR_BLUE))	SET_LED_ON(LED_RGB_BLUE);
			}
			else
			{
				SET_LED_OFF(LED_RGB_RED);
				SET_LED_OFF(LED_RGB_GREEN);
                SET_LED_OFF(LED_RGB_BLUE);
			}
			
			app_timer_start(led_flasher_timer_id, APP_TIMER_TICKS(timespan, 0), (void*)&current_task);
			
			if((current_task.duration != LED_FLASH_UNLIMITED) && current_task.duration)
			{
				if(current_task.duration > timespan)
				{
					current_task.duration -= timespan;
				}
				else
				{
					current_task.duration = 0;
				}
			}
			
			++frame;
		}
		else
		{
			frame = 0;
			SET_LED_OFF(LED_RGB_RED);
			SET_LED_OFF(LED_RGB_GREEN);
			SET_LED_OFF(LED_RGB_BLUE);
			if ((task == NULL) || (task->id != current_task.id))
			{
				app_timer_start(led_flasher_timer_id, APP_TIMER_MIN_TIMEOUT_TICKS, (void*)task);
			}
			else if(current_task.duration)
			{
				app_timer_start(led_flasher_timer_id, APP_TIMER_MIN_TIMEOUT_TICKS, (void*)&current_task);
			}
			else 
			{
				task = NULL;
			}				
		}
	}
}

void led_flasher_init(void)
{
	led_flasher_pattern_t initial_pattern;
	
	nrf_gpio_cfg_output(LED_RGB_RED);
	SET_LED_OFF(LED_RGB_RED);
	
	nrf_gpio_cfg_output(LED_RGB_GREEN);
	SET_LED_OFF(LED_RGB_GREEN);

	nrf_gpio_cfg_output(LED_RGB_BLUE);
	SET_LED_OFF(LED_RGB_BLUE);
	
	initial_pattern.timespan = 125;
	initial_pattern.length = 8;
	initial_pattern.slots = 0xA0000000;
	initial_pattern.color = 1 << COLOR_GREEN;
	
	led_flasher_init_patern(LED_FLASHER_MODE_MUTUAL,	&initial_pattern);
	
	initial_pattern.color = 1 << COLOR_RED;
	led_flasher_init_patern(LED_FLASHER_MODE_PARTIAL,	&initial_pattern);
	
	initial_pattern.color = 1 << COLOR_BLUE;
	led_flasher_init_patern(LED_FLASHER_MODE_HASHTAG,	&initial_pattern);
	
	initial_pattern.length = 8;
	initial_pattern.slots = 0x88000000;
	initial_pattern.color = (1 << COLOR_GREEN) | (1 << COLOR_BLUE) | (1 << COLOR_RED);
	led_flasher_init_patern(LED_FLASHER_MODE_ADV,		&initial_pattern);
	
	app_timer_create(&led_flasher_timer_id,
					APP_TIMER_MODE_SINGLE_SHOT,
					led_flasher_handler);	
}

void led_flasher_init_patern(led_flasher_mode_e mode, led_flasher_pattern_t * pattern)
{
	if (mode < LED_FLASHER_MODE_COUNT)
	{
		flasher_tasks[mode].pattern = *pattern;
	}
}

void led_flasher_start(led_flasher_mode_e mode, uint32_t duration)
{
	static uint8_t task_id = 0;
	
	if (mode < LED_FLASHER_MODE_COUNT)
	{
		++task_id;
		flasher_tasks[mode].duration = duration;
		flasher_tasks[mode].id = task_id;
		
		// IF there is no running task THEN start new immdiatelly
		if (task == NULL)
			app_timer_start(led_flasher_timer_id, APP_TIMER_MIN_TIMEOUT_TICKS, (void*)&flasher_tasks[mode]);
		
		task = &flasher_tasks[mode];
	}
}

void led_flasher_start_times(led_flasher_mode_e mode, uint16_t times)
{
	uint32_t duration;
	
	if (mode < LED_FLASHER_MODE_COUNT)
	{
		duration = (uint32_t)flasher_tasks[mode].pattern.length * flasher_tasks[mode].pattern.timespan * times;
		led_flasher_start(mode, duration);
	}
}

void led_flasher_stop(void)
{
	task = NULL;
}

