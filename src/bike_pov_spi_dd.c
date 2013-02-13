/*
 * bike_pov_SPI_dd.c
 *
 * Created: 12/23/2012 6:47:45 PM
 *  Author: mruha
 */ 
#ifndef BIKE_POV_SPI_DD_C_
#define BIKE_POV_SPI_DD_C_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include "../hdr/bike_pov.h"
#include "../hdr/bike_pov_type.h"
#include "../hdr/bike_pov.h"
#include <avr/interrupt.h>
#include "../ff9a/src/diskio.h"
#include "../ff9a/src/ff.h"
#include "../hdr/bike_pov_SPI_dd.h"

/********************************************************************
 *function:
 *	SPI_LED_driver_mode_switch
 *arguments:
 *	mode = normal or special
 *return value:
 *	none
 *comment:
 *	swithes between normal and special mode
 */

void SPI_LED_driver_mode_switch(u8 mode)
{
	SPI_EN			// enable SPI
	SPI_DATA(0x00)	// send dummy byte to activate clk
	
	// mode switching sequence
	SPI_OE_LOW		
	SPI_MODE_SWITCH_WAIT
	SPI_OE_HIGH
	SPI_MODE_SWITCH_WAIT
	
	if (mode == 0)	// special mode
	{
		SPI_LED_LE_ON
		SPI_MODE_SWITCH_WAIT
		SPI_LED_LE_OFF
	}
	else           // normal mode
	{
		SPI_MODE_SWITCH_WAIT
		SPI_MODE_SWITCH_WAIT
	}
	
	SPI_DIS			// disable SPI
}

/********************************************************************
 *function:
 *	SPI_program
 *arguments:
 *	data_size	= data size to send
 *	*data		= data to send
 *	device		= 0 LED driver, 1 SD card
 *return value:
 *	none
 *comment:
 *	sends bytes to SPI device
 */
void SPI_program(u8 data_size _DOLLAR_ u8 device)
{
	u8 byte_cnt,i;
	u8 data_to_send = 0;
	u8 sd_on = 0;

	if ( SPI_SD_SS_ON ) 	{ sd_on = 1; SPI_SD_OFF }

	SPI_EN			// enable SPI
	
	if (device == SPI_DEVICE_LED)		// if LED then latch
	{
		for (byte_cnt = data_size; byte_cnt > 0; byte_cnt--)
		{
			data_to_send = (u8)((bike_pov_st.LED_data[bike_pov_st.actual_index]) >> ((byte_cnt - 1)*8) & 0xff);
			SPI_DATA(data_to_send)							// send bytes
			while(!(SPSR & (1 << SPIF)));					// wait for transmission end flag
		}

		SPI_LED_LE_A_ON					// latch A
		SPI_LED_LE_DELAY				// wait 5 us to latch B side
		SPI_LED_LE_B_ON					// latch B
		SPI_MODE_SWITCH_WAIT			// wait one SCK cycle
		SPI_LED_LE_OFF					// both off
	}

	SPI_DIS			// disable SPI

	if ( sd_on ) 			{ SPI_SD_ON }

}

/********************************************************************
 *function:
 *	SPI_SD_trace
 *arguments:
 *	mode:	select trace mode: HALL,INT0,INT1,INIT,...
 *return value:
 *	none
 *comment:
 *	trace the selected content to SD log.txt
 */
void SPI_SD_trace( bike_pov_trace_mode mode )
{
	s8 res;
	FIL 	file;
	FATFS	fs;

	u32 	trace_cnt;
	u8 		time_index;
	u32		wheel_time;
	u16		compare_time;
	u8		display_offset;

	// init trace content
	trace_cnt 		= (u32) bike_pov_st.trace_cnt;
	time_index		= (u8)  ((bike_pov_st.time_index + 5)%6);
	wheel_time		= (u32) bike_pov_st.wheel_time[time_index];
	display_offset	= (u8) 	bike_pov_st.display_offset;
	compare_time	= (u16) bike_pov_st.compare_time;
	
	res = f_mount( 0, &fs );

	res = f_open	( &file, "0:LOG.TXT", FA_WRITE | FA_READ );
	res = f_lseek	( &file, f_size( &file ) );
	
	// trace the function
	switch ( mode )
	{
		case SD_HALL_START:
			res = f_printf( &file, "%d: HALL: start\n", trace_cnt );
			break;
		case SD_HALL:
		#if 0
			res = f_printf( &file, "%d: HALL trace: time_index %d: wheel_time %d: compare_time %d\n",
									trace_cnt,
									time_index,
									wheel_time,
									compare_time);
		#else
			res = f_printf( &file, "%d: HALL trace: ", trace_cnt);
			res = f_printf( &file, "time_index %d: ", time_index);
			res = f_printf( &file, "sample_ctr %d: ", bike_pov_st.sample_ctr);
			res = f_printf( &file, "wheel_time %d: ", wheel_time);
			res = f_printf( &file, "compare_time %d\n", compare_time);
		#endif
			break;
		case SD_INT0:
			res = f_printf( &file, "%d: INT0 trace: display_offset %d\n", 
									trace_cnt,
									display_offset);
			break;
		case SD_INT0_CLOSE:
			res = f_printf( &file, "%d: INT0 close: display_offset %d\n",
									trace_cnt,
									display_offset);
			break;
		case SD_INT1:
			res = f_printf( &file, "%d: INT1 trace: display_offset %d\n",
									trace_cnt,
									display_offset);
			break;
		case SD_INT1_CLOSE:
			res = f_printf( &file, "%d: INT1 close: display_offset %d\n",
									trace_cnt,
									display_offset);
			break;
		default:
			break;
	}
	
	// increment trace counter
	bike_pov_st.trace_cnt++;
	res = f_close( &file );
	
	f_mount( 0, 0 );
}
void SPI_SD_trace_init( char* trace_str )
{
	u16 i;

	for( i = 0; i < 512; i++ )
	{
		trace_str[i] = ' ';
	}
}
/********************************************************************
 *function:
 *	SPI_SD_read
 *arguments:
 *	mode:	select trace mode: HALL,INT0,INT1,INIT,...
 *return value:
 *	none
 *comment:
 *	read bike_pov_image to buffer
 */
 void SPI_SD_read( u32* buff, u8 buff_size )
 {
	 // to do
 }

#undef BIKE_POV_SPI_DD_C_
#endif
