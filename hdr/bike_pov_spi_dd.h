/*
 * bike_pov_SPI_dd.h
 *
 * Created: 12/23/2012 6:48:50 PM
 *  Author: mruha
 */ 
 
#ifndef BIKE_POV_SPI_DD_H_
#define BIKE_POV_SPI_DD_H_

#ifdef BIKE_POV_SPI_DD_C_
	#define _OWNER_
#endif

#include "../hdr/hdr_owner.h"

// macros
#define LED_10mA				0x5A
#define LED_15mA				0xFE

#define SPI_EN					(SPCR |=  (1 << SPE) | (1 << MSTR));
#define SPI_MASTER				(SPCR |=  (1 << MSTR));
#define SPI_DIS					(SPCR &= ~(1 << SPE));

#define SPI_DATA(data)			(SPDR = data);
#define SPI_OE_LOW				(PORTA &= ~(1 << 3));
#define SPI_OE_HIGH				(PORTA |=  (1 << 3));

#define SPI_MODE_SWITCH_WAIT 	asm("nop"); \
								asm("nop");

#define SPI_LED_LE_A_ON			(PORTA |=	(1 << 1));
#define SPI_LED_LE_A_OFF		(PORTA &=  ~(1 << 1));
#define SPI_LED_LE_B_ON			(PORTA |=	(1 << 2));
#define SPI_LED_LE_B_OFF		(PORTA &=  ~(1 << 2));
#define SPI_LED_LE_DELAY		for (i = 0; i < 40; i++) asm("nop");	// 5us delay

#define SPI_SD_ON				(PORTB &= ~( 1 << 1 ));			/* CS = L */
#define	SPI_SD_OFF				(PORTB |=  ( 1 << 1 ));			/* CS = H */

#define SPI_LED_LE_ON			(PORTA |=   0x06 );
#define SPI_LED_LE_OFF			(PORTA &= ~(0x06));

#define SPI_LED_ON				SPI_OE_LOW
#define SPI_LED_OFF				SPI_OE_HIGH

#define SPI_SD_SS_ON			(!(PORTB & ( 1 << 1 )))

#define TRACING_LEVEL 			2		// 3 = ALL, 2 = timers excluded, 1 = only error tracing ON, 0 = no tracing

#if TRACING_LEVEL > 0
#define ERROR_TRACING
#if TRACING_LEVEL > 1
#define DEBUG_TRACING
#if TRACING_LEVEL > 2
#define TIMER0_TRACING
#define TIMER1_TRACING
#endif
#endif
#endif

// typedefs
typedef enum
{
	SPI_DEVICE_LED = 0,
	SPI_DEVICE_SD
} bike_pov_device;

SHARED_FUNC(void, SPI_led_driver_tx					, void										)
SHARED_FUNC(void, SPI_LED_driver_mode_switch        , u8 mode									)
SHARED_FUNC(void, SPI_program						, u8 data_size _DOLLAR_ u8 device			)
SHARED_FUNC(void, SPI_SD_init						, void										)
SHARED_FUNC(u8	, SPI_receive						, bike_pov_device device					)
SHARED_FUNC(void, SPI_send							, u8 data_byte	_DOLLAR_ bike_pov_device	)
SHARED_FUNC(void, SPI_delay_100us					, void										)
SHARED_FUNC(void, SPI_SD_trace						, bike_pov_trace_mode mode					)
SHARED_FUNC(void, SPI_SD_read						, u32* buff _DOLLAR_ u8 buff_size			)
SHARED_FUNC(void, SPI_SD_trace_init					, char* trace_str 							)
#undef _OWNER_
#endif /* BIKE_POV_SPI_DD_H_ */
