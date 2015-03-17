#ifndef CONFIG_H
#define CONFIG_H
/***	Specifiy which SoftDevice is in use		***/

#define	SOFT_DEVICE_05		5
#define	SOFT_DEVICE_09		9

//	#define SOFT_DEVICE_TYPE SOFT_DEVICE_05
	#define SOFT_DEVICE_TYPE SOFT_DEVICE_09
	
#ifndef SOFT_DEVICE_TYPE
	#error Specify SoftDevice
#endif

#if SOFT_DEVICE_TYPE == SOFT_DEVICE_09
	#define WORKAROUND_1	0
	#define WORKAROUND_2	0
#else
	#define WORKAROUND_1	1
	#define WORKAROUND_2	1
#endif

/**************************************************/
#endif /* BOARD_CONFIG_H */

