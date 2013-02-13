/*
 * bike_pov.c
 *
 * Created: 12/26/2012 12:48:52 PM
 *  Author: mruha
 */
#ifndef BIKE_POV_C_
#define BIKE_POV_C_
#define F_CPU 8000000UL //8MHz
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../hdr/bike_pov.h"
#include "../hdr/bike_pov_SPI_dd.h"

#define SD_TEST
#undef SD_TEST

/****************************************************************************************************************
 *function:
 *	main
 *arguments:
 *	none
 *return value:
 *	none
 */
int main(void)
{

	bike_pov_init();

	// wait before first SD action
	_delay_ms(1000);
	
	// INIT SD trace
	SPI_SD_trace( SD_INIT );
	
	#ifdef SD_TEST

	SPI_SD_trace( SD_HALL_START );
	
	while(1){}
	#endif
	
	sei();		// enable interrupts
	
    while(1)
    {
		if ( bike_pov_st.trace_flag == 1 )
		{
			SPI_SD_trace( bike_pov_st.trace_mode );
			bike_pov_st.trace_flag = 0;
		}
        //TODO:: Please write your application code 
    }
}

/****************************************************************************************************************
 *function:
 *	bike_pov_init
 *arguments:
 *	none
 *return value:
 *	none
 *comment:
 *	initializes hw and data before idle
 */

void bike_pov_init(void)
{
	u16 i;
	u8  current_bytes[2] ;
	
	// initialize data structures
	bike_pov_st.time_index			= 0;	// index for the latest time stamp
	bike_pov_st.display_offset		= 0;	// image offset on wheel
	bike_pov_st.compare_time		= 0;	// output of estimator, the actual compare time
	bike_pov_st.image_index			= 0;	// image index, show image number xx from sd card
	bike_pov_st.LED_data_index		= 0;	// index for display degree
	bike_pov_st.sample_ctr			= 0;	// samples in for the estimator
	bike_pov_st.wheel_image_time	= 0;	// wheel image time
	bike_pov_st.wheel_total_time	= 0;	// total_time of the whee
	bike_pov_st.trace_cnt			= 0;	// trace counter
//	bike_pov_st.trace_byte_idx		= 0;	// trace byte index in file

#if 0	
	for (i = 0; i < 256; i++)
	{
		bike_pov_st.LED_data[i] = 0x00000000;	// LED input data		
	}
#else
	i = 0;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xfff00000;  i++;
	bike_pov_st.LED_data[i] = 0xfff00000;  i++;
	bike_pov_st.LED_data[i] = 0xfff00000;  i++;
	bike_pov_st.LED_data[i] = 0xfff00000;  i++;
	bike_pov_st.LED_data[i] = 0xfff00000;  i++;
	bike_pov_st.LED_data[i] = 0xfff80000;  i++;
	bike_pov_st.LED_data[i] = 0xfff80000;  i++;
	bike_pov_st.LED_data[i] = 0xfff80000;  i++;
	bike_pov_st.LED_data[i] = 0xfff80000;  i++;
	bike_pov_st.LED_data[i] = 0xfff80000;  i++;
	bike_pov_st.LED_data[i] = 0xfffc0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffc0000;  i++;
	bike_pov_st.LED_data[i] = 0xffff0000;  i++;
	bike_pov_st.LED_data[i] = 0xffffe000;  i++;
	bike_pov_st.LED_data[i] = 0xfffff000;  i++;
	bike_pov_st.LED_data[i] = 0xfffff000;  i++;
	bike_pov_st.LED_data[i] = 0xffff8000;  i++;
	bike_pov_st.LED_data[i] = 0xffff0000;  i++;
	bike_pov_st.LED_data[i] = 0xffffe000;  i++;
	bike_pov_st.LED_data[i] = 0xfffff800;  i++;
	bike_pov_st.LED_data[i] = 0xfffffc00;  i++;
	bike_pov_st.LED_data[i] = 0xfffff000;  i++;
	bike_pov_st.LED_data[i] = 0xffffc000;  i++;
	bike_pov_st.LED_data[i] = 0xffff8000;  i++;
	bike_pov_st.LED_data[i] = 0xffff0000;  i++;
	bike_pov_st.LED_data[i] = 0xffff0000;  i++;
	bike_pov_st.LED_data[i] = 0xffff0000;  i++;
	bike_pov_st.LED_data[i] = 0xffff0000;  i++;
	bike_pov_st.LED_data[i] = 0xffff0000;  i++;
	bike_pov_st.LED_data[i] = 0xffff0000;  i++;
	bike_pov_st.LED_data[i] = 0xffff0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffe0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffe0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffe0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffe0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffe0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffc0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffe0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffc0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffc0000;  i++;
	bike_pov_st.LED_data[i] = 0xfff80000;  i++;
	bike_pov_st.LED_data[i] = 0xfff80000;  i++;
	bike_pov_st.LED_data[i] = 0xfff80000;  i++;
	bike_pov_st.LED_data[i] = 0xfff80000;  i++;
	bike_pov_st.LED_data[i] = 0xfff00000;  i++;
	bike_pov_st.LED_data[i] = 0xfff00000;  i++;
	bike_pov_st.LED_data[i] = 0xfff00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xfe000000;  i++;
	bike_pov_st.LED_data[i] = 0xfe000000;  i++;
	bike_pov_st.LED_data[i] = 0xfc000000;  i++;
	bike_pov_st.LED_data[i] = 0xf8000000;  i++;
	bike_pov_st.LED_data[i] = 0xe0000000;  i++;
	bike_pov_st.LED_data[i] = 0x60000000;  i++;
	bike_pov_st.LED_data[i] = 0x00000000;  i++;
	bike_pov_st.LED_data[i] = 0x00000000;  i++;
	bike_pov_st.LED_data[i] = 0x00000000;  i++;
	bike_pov_st.LED_data[i] = 0x00000000;  i++;
	bike_pov_st.LED_data[i] = 0x00000000;  i++;
	bike_pov_st.LED_data[i] = 0x00000000;  i++;
	bike_pov_st.LED_data[i] = 0x00000000;  i++;
	bike_pov_st.LED_data[i] = 0x00000000;  i++;
	bike_pov_st.LED_data[i] = 0x00000000;  i++;
	bike_pov_st.LED_data[i] = 0x00000000;  i++;
	bike_pov_st.LED_data[i] = 0x00000000;  i++;
	bike_pov_st.LED_data[i] = 0x80000000;  i++;
	bike_pov_st.LED_data[i] = 0x80000000;  i++;
	bike_pov_st.LED_data[i] = 0x80000000;  i++;
	bike_pov_st.LED_data[i] = 0x80000000;  i++;
	bike_pov_st.LED_data[i] = 0x80000000;  i++;
	bike_pov_st.LED_data[i] = 0x80000000;  i++;
	bike_pov_st.LED_data[i] = 0x80000000;  i++;
	bike_pov_st.LED_data[i] = 0xc0000000;  i++;
	bike_pov_st.LED_data[i] = 0xc0000000;  i++;
	bike_pov_st.LED_data[i] = 0xc0000000;  i++;
	bike_pov_st.LED_data[i] = 0xc0000000;  i++;
	bike_pov_st.LED_data[i] = 0xe0000000;  i++;
	bike_pov_st.LED_data[i] = 0xe0000000;  i++;
	bike_pov_st.LED_data[i] = 0xe0000000;  i++;
	bike_pov_st.LED_data[i] = 0xf0000000;  i++;
	bike_pov_st.LED_data[i] = 0xf8000000;  i++;
	bike_pov_st.LED_data[i] = 0xf8000000;  i++;
	bike_pov_st.LED_data[i] = 0xfc000000;  i++;
	bike_pov_st.LED_data[i] = 0xfe000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xfffff000;  i++;
	bike_pov_st.LED_data[i] = 0xfffff000;  i++;
	bike_pov_st.LED_data[i] = 0xffffe000;  i++;
	bike_pov_st.LED_data[i] = 0xffffe000;  i++;
	bike_pov_st.LED_data[i] = 0xffffe000;  i++;
	bike_pov_st.LED_data[i] = 0xffffe000;  i++;
	bike_pov_st.LED_data[i] = 0xffffc000;  i++;
	bike_pov_st.LED_data[i] = 0xffffc000;  i++;
	bike_pov_st.LED_data[i] = 0xffffc000;  i++;
	bike_pov_st.LED_data[i] = 0xffff8000;  i++;
	bike_pov_st.LED_data[i] = 0xffff8000;  i++;
	bike_pov_st.LED_data[i] = 0xffff0000;  i++;
	bike_pov_st.LED_data[i] = 0xffff0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffe0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffc0000;  i++;
	bike_pov_st.LED_data[i] = 0xfff80000;  i++;
	bike_pov_st.LED_data[i] = 0xfff00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xfff60000;  i++;
	bike_pov_st.LED_data[i] = 0xfffe0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffe0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffe0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffe0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffc0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffc0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffc0000;  i++;
	bike_pov_st.LED_data[i] = 0xfff80000;  i++;
	bike_pov_st.LED_data[i] = 0xfff80000;  i++;
	bike_pov_st.LED_data[i] = 0xfff80000;  i++;
	bike_pov_st.LED_data[i] = 0xfff80000;  i++;
	bike_pov_st.LED_data[i] = 0xfff00000;  i++;
	bike_pov_st.LED_data[i] = 0xfff00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff300000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00800;  i++;
	bike_pov_st.LED_data[i] = 0xffc00800;  i++;
	bike_pov_st.LED_data[i] = 0xff801000;  i++;
	bike_pov_st.LED_data[i] = 0xff803000;  i++;
	bike_pov_st.LED_data[i] = 0xff807000;  i++;
	bike_pov_st.LED_data[i] = 0xff81e000;  i++;
	bike_pov_st.LED_data[i] = 0xff03e000;  i++;
	bike_pov_st.LED_data[i] = 0xff07e000;  i++;
	bike_pov_st.LED_data[i] = 0xff0fc000;  i++;
	bike_pov_st.LED_data[i] = 0xff1fc000;  i++;
	bike_pov_st.LED_data[i] = 0xff7fc000;  i++;
	bike_pov_st.LED_data[i] = 0xfeff8000;  i++;
	bike_pov_st.LED_data[i] = 0xffff8000;  i++;
	bike_pov_st.LED_data[i] = 0xffff8000;  i++;
	bike_pov_st.LED_data[i] = 0xffff0000;  i++;
	bike_pov_st.LED_data[i] = 0xffff0000;  i++;
	bike_pov_st.LED_data[i] = 0xffff0000;  i++;
	bike_pov_st.LED_data[i] = 0xffff0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffe0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffe0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffe0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffe0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffe0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffc0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffc0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffc0000;  i++;
	bike_pov_st.LED_data[i] = 0xfffc0000;  i++;
	bike_pov_st.LED_data[i] = 0xfff80000;  i++;
	bike_pov_st.LED_data[i] = 0xfff80000;  i++;
	bike_pov_st.LED_data[i] = 0xfff80000;  i++;
	bike_pov_st.LED_data[i] = 0xfff80000;  i++;
	bike_pov_st.LED_data[i] = 0xfff00000;  i++;
	bike_pov_st.LED_data[i] = 0xfff00000;  i++;
	bike_pov_st.LED_data[i] = 0xfff00000;  i++;
	bike_pov_st.LED_data[i] = 0xfff00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffe00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff000000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xff800000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;
	bike_pov_st.LED_data[i] = 0xffc00000;  i++;

#endif
	for (i = 0; i < SAMPLE_MAX; i++)
	{
		bike_pov_st.wheel_time[i] = 0;		// sample values for estimator
	}
	
	// initialize ports
	DDRA |= 0b00001110;		// PIN3 = OE, PIN2 = LED_LE_B, PIN1 = LED_LE_A
	DDRB |= 0b10110010;		// PIN7 = SCK , PIN5 = MOSI, PIN4 = SS ,PIN1 = SS_SD_CARD
	DDRC |= 0b00010000;		// PIN4 = TDO
	
	PORTA |= (1 << 3);		// OE low active, disable with high signal
	PORTB |= (1 << 1);		// SS_SD_CARD low active
	
	// initialize interrupts
	MCUCR  &=	~(1 << ISC11) | \
				~(1 << ISC10) | \
				~(1 << ISC01) | \
				~(1 << ISC00);
				 
	MCUCSR &=  ~(1 << ISC2);	// falling edge => interrupt
	GICR   |=	(1 << INT1) | \
				(1 << INT0) | \
				(1 << INT2);	// enable external interrupts
	

	// initialize timer0
	TIMSK |= (1 << TOIE0);		// timer0 overflow interrupt
	
	// initialize timer1
	TCCR1B |= (1 << WGM12);		// CTC mode
	TIMSK  |= (1 << OCIE1A);	// output compare match interrupt enable
	
	// SPI setup
	SPCR	|=	(1 << MSTR);	// select CPU master mode
	SPSR	|=	(1 << SPI2X);	// SPI clk f_clk/2
	
	
	// LED driver current setup
//	SPI_LED_driver_mode_switch(0);	// switch to special mode
//	SPI_program(2, current_bytes, SPI_DEVICE_LED);
//	SPI_LED_driver_mode_switch(1);	// switch back to normal mode
}

/****************************************************************************************************************
 *function:
 *	ISR(INT2_vect)
 *arguments:
 *	none
 *return value:
 *	none
 *comment:
 *	ISR for hall sensor
 */
ISR(INT2_vect)
{
	cli();	// disable other interrupts


	// store wheel time value for estimator and 
	TIMER0_VALUE(bike_pov_st.wheel_time[bike_pov_st.time_index])		// store last counter0 value to wheel_time for total time
	TIMER0_VALUE(bike_pov_st.wheel_image_time)							// store last counter0 value
	
	// timer0 start
	TIMER0_RESET				// reset timer0 value
	TIMER0_START				// start timer0 if not on already

		
	// reset wheel total time, resets 10sek counter
	bike_pov_st.wheel_total_time	= 0;
	// reset LED data index
	bike_pov_st.LED_data_index		= 0;
	
	if (bike_pov_st.sample_ctr >= ( SAMPLE_MAX - 1))
	{
		SPI_LED_ON				// make picture visible when enough timing samples
		
		// find estimate for next time interval
		bike_pov_compare_estimator();
	
		// set TIMER1 compare value
		TIMER1_CMP_SET(bike_pov_st.compare_time)

		// timer1 start
		TIMER1_RESET				// reset timer1 value
		TIMER1_START				// start timer1
				
		SPI_program(4,SPI_DEVICE_LED);	// here should come the first led action
		bike_pov_st.LED_data_index++;	// update index to next
	} 
	else
	{
		bike_pov_st.sample_ctr++;
		SPI_LED_OFF				// dont show image when not enough samples for timing
	}
	
	// increment time index
	bike_pov_st.time_index = (bike_pov_st.time_index + 1)%6;
	// reset wheel time
	bike_pov_st.wheel_time[bike_pov_st.time_index] = 0;
	
#ifdef DEBUG_TRACING
	bike_pov_st.trace_flag = 1;
	if 	( bike_pov_st.sample_ctr <= 1 )	
	{
		bike_pov_st.trace_mode = SD_HALL_START;
	}
	else
	{
		bike_pov_st.trace_mode = SD_HALL;
	}
#endif		
	
	while( !( PINB & ( 1 << PINB2 ) ) ); // wait untill released
	
	sei();	// enable interrupts again
}

/****************************************************************************************************************
 *function:
 *	ISR(INT0_vect)
 *arguments:
 *	none
 *return value:
 *	none
 *comment:
 *	ISR for button0
 */
ISR(INT0_vect)
{
	cli();		// disable interrupts

	// stop clocks
	TIMER0_STOP
	TIMER1_STOP
	
	// shut leds off
	SPI_OE_HIGH

	// reset some status values
	bike_pov_st.wheel_total_time	= 0;
	bike_pov_st.wheel_image_time	= 0;
	bike_pov_st.sample_ctr			= 0;
	bike_pov_st.time_index			= 0;
	bike_pov_st.LED_data_index		= 0;
	
	// set new display offset
	bike_pov_st.display_offset += 1;
#ifdef DEBUG_TRACING
	bike_pov_st.trace_flag = 1;
	bike_pov_st.trace_mode = SD_INT0;
#endif
	// wait until button released
	while(!(PIND & (1 << 2)));

	sei();		// enable interrupts
}

/****************************************************************************************************************
 *function:
 *	ISR(INT1_vect)
 *arguments:
 *	none
 *return value:
 *	none
 *comment:
 *	ISR for button1
 */
ISR(INT1_vect)
{
	cli();		// disable interrupts

	// stop clocks
	TIMER0_STOP
	TIMER1_STOP
	
	// shut leds off
	SPI_OE_HIGH
	
	// reset some status values
	bike_pov_st.wheel_total_time	= 0;
	bike_pov_st.wheel_image_time	= 0;
	bike_pov_st.sample_ctr			= 0;
	bike_pov_st.time_index			= 0;
	bike_pov_st.LED_data_index		= 0;
	
	// set new display offset
	bike_pov_st.display_offset -= 1;
#ifdef DEBUG_TRACING	
	bike_pov_st.trace_flag = 1;
	bike_pov_st.trace_mode = SD_INT1;
#endif	
	// wait until button released
	while(!(PIND & (1 << 3)));
	
	sei();		// enable interrupts
}

/****************************************************************************************************************
 *function:
 *	ISR(TIMER0_OVF_vect)
 *arguments:
 *	none
 *return value:
 *	none
 *comment:
 *	ISR for counter0 overflow
 */
ISR(TIMER0_OVF_vect)
{
	cli();
	// store 0xff to time values
	TIMER0_OVF(bike_pov_st.wheel_time[bike_pov_st.time_index])
	TIMER0_OVF(bike_pov_st.wheel_total_time)
	TIMER0_OVF(bike_pov_st.wheel_image_time)
	
	// store rest of the timer value
	TIMER0_VALUE(bike_pov_st.wheel_time[bike_pov_st.time_index])
	TIMER0_VALUE(bike_pov_st.wheel_total_time)
	TIMER0_VALUE(bike_pov_st.wheel_image_time)	

	// if 10 seconds passed without hall interrupt shutdown LEDs and CPU
	if (bike_pov_st.wheel_total_time >= TIME_10sec)
	{
		// stop timers
		TIMER0_STOP
		TIMER1_STOP	
		
		// leds off
		SPI_OE_HIGH
		
		// close tracing
		SPI_SD_trace( SD_SLEEP 		);
		SPI_SD_trace( SD_FINALIZE 	);
		SPI_LED_OFF				// shut LEDs off
		CPU_SLEEP_ENABLE		// set power down CPU
		asm ("sleep");			// sleep -> power down
		CPU_SLEEP_DISABLE		// disable sleep		
	}
	
#ifdef TIMER0_TRACING
	bike_pov_st.trace_flag = 1;
	bike_pov_st.trace_mode = SD_TIMER0;
#endif
	sei();
}
/****************************************************************************************************************
 *function:
 *	ISR(TIMER1_COMPA_vect)
 *arguments:
 *	none
 *return value:
 *	none
 *comment:
 *	ISR for counter1 compare match
 */
ISR(TIMER1_COMPA_vect)
{	
	cli();

	SPI_LED_ON			// make sure OE low

	// count led index from offset
	if (bike_pov_st.display_offset < 0)
	{
		bike_pov_st.actual_index = bike_pov_st.LED_data_index + ((u8)0xff + bike_pov_st.display_offset);
	} 
	else
	{
		bike_pov_st.actual_index = bike_pov_st.LED_data_index + bike_pov_st.display_offset;
	}

	// program LED data through SPI
	if(bike_pov_st.sample_ctr >= ( SAMPLE_MAX - 1))
	{
		SPI_program(4,SPI_DEVICE_LED);

		// update index to next
		bike_pov_st.LED_data_index++;
	}
#ifdef TIMER1_TRACING
	bike_pov_st.trace_flag = 1;
	bike_pov_st.trace_mode = SD_TIMER1;
#endif
	sei();
}

/****************************************************************************************************************
 *function:
 *	bike_pov_compare_estimator
 *arguments:
 *	none
 *return value:
 *	none
 *comment:
 *	Estimator to provide next estimate for interval timing between degrees
 */
#define WM_DIVIDER 3
void bike_pov_compare_estimator(void)
{

#if WHEIGHTED_MEAN
	u8 i;
	u32 mean = 0;
	s32 accel = 0;

	// first try is weighted mean	
	for (i = 0; i < SAMPLE_MAX; i++)
	{
		mean 	+= (5 - i)*0.2*bike_pov_st.wheel_time[((bike_pov_st.time_index + i)%6)];
		accel 	+= i*0.2*(bike_pov_st.wheel_time[((bike_pov_st.time_index + i)%6)] - bike_pov_st.wheel_time[bike_pov_st.time_index]);
	}

	// count the weighted mean
	mean = mean / WM_DIVIDER;
	mean = (u32)(mean + accel);

	// store the weighted mean value divided to 256 degree
	bike_pov_st.compare_time = (u16)(mean/256);
#else
	bike_pov_st.compare_time = (u16)(bike_pov_st.wheel_time[bike_pov_st.time_index] / 256);
#endif
}

#undef BIKE_POV_C_
#endif
