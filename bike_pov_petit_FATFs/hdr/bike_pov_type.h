/*
 * bike_pov_basic_type.h
 *
 * Created: 12/22/2012 5:06:47 PM
 *  Author: mruha
 */ 


#ifndef BIKE_POV_BASIC_TYPE_H_
#define BIKE_POV_BASIC_TYPE_H_

// dollar
#define _DOLLAR_					,

// defines
#define SAMPLE_MAX			6
#define WHEEL_DEGREE_MAX	256
#define WHEEL_DIST_MAX		4

// timer macros
#define TIMER1_RESET			(TCNT1   = 0			);
#define TIMER1_CMP_SET(data)	(OCR1A   = data			);
#define TIMER1_START			(TCCR1B |=  (1 << CS11)	);
#define TIMER1_STOP				(TCCR1B &= ~(1 << CS11)	);

#define TIMER0_RESET			(TCNT0  = 0				);
#define TIMER0_START			(TCCR0 |=  (1 << CS11)	);
#define TIMER0_STOP				(TCCR0 &= ~(1 << CS11)	);
#define TIMER0_OVF(store)		(store += 0xff			);
#define TIMER0_VALUE(store)		(store += TCNT0			);

#define TIME_10sec				10000000		// in us
#define TIME_1sec				1000000			// in us
#define TIME_1ms				1000			// in us
#define TIME_1us				1				// in us

// power down mode
#define CPU_SLEEP_ENABLE		(MCUCR |=  (1 << SE) |  (1 << SM1));		// enter power down mode, only wake up with external interrupt
#define CPU_SLEEP_DISABLE		(MCUCR &= ~(1 << SE) | ~(1 << SM1));		// disable sleep

typedef volatile unsigned char			u8;
typedef volatile unsigned int			u16;
typedef volatile unsigned long int		u32;

typedef volatile signed char			s8;
typedef volatile signed int				s16;
typedef volatile signed long int		s32;

typedef enum
{
	SD_HALL_START = 0,
	SD_HALL,
	SD_INT0,
	SD_INT1,
	SD_TIMER0,
	SD_TIMER1,
	SD_INIT,
	SD_OPEN_LOG,
	SD_OPEN_IM,
	SD_FINALIZE,
	SD_SLEEP
} bike_pov_trace_mode;

// bike_pov data:
typedef struct
{
	// bike_pov state
	s8 			display_offset;					// image cw and ccw offset
	u32 		wheel_total_time;   			// total time of the wheel if > 10 sec stop all
	u32 		wheel_image_time;  				// image change time
	u32			trace_cnt;						// trace counter
	
	// trace
	u8			trace_flag;						// trace flag: 1 start tracing in main loop
	bike_pov_trace_mode 	trace_mode;						// trace mode
	
	// image
	u8 			image_index;					// indicating image index to fetch from SD card
	
	// time measure samples
	u8 			sample_ctr;						// how many time_samples available for estimation
	u32 		wheel_time[SAMPLE_MAX];			// wheel time measurement = sample
	u16 		compare_time;					// actual interval between degrees
	u8 			time_index;
	
	// LED data
	u8 			LED_data_index;					// indicating next LED degree, 256 degrees
	u8  		actual_index;					// LED_data_index + display_offset
	u32			LED_data[WHEEL_DEGREE_MAX];		// LED data
} bike_pov_data_st;

#endif /* BIKE_POV_BASIC_TYPE_H_ */
