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

#include "../hdr/bike_pov_SPI_dd.h"

#include "../pff2a/src/diskio.h"
#include "../pff2a/src/pff.h"

FATFS fs_s;

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
	WORD	bytes_writen;
	char	trace_str[100];
	char	temp_str[9];
	//			u8		trace_len = 0;
	u8		res;
	u8		temp_index;
	
	// init trace string
	trace_str[0] = '\0';
	
	// trace the function
	switch ( mode )
	{
		case SD_HALL_START:
			strcpy( trace_str, "HALL START, \n" );
			res = pf_write( trace_str, strlen( trace_str ), &bytes_writen );
			break;
		case SD_HALL:
			strcpy( trace_str, "HALL, " );
			
			// wheel time
			strcat( trace_str, "wheel_time, " );
			temp_index = ( bike_pov_st.time_index + 5 ) % 6;
			ultoa( bike_pov_st.wheel_time[ temp_index ] , temp_str, 10 );
			strcat( trace_str, temp_str );
			
			// append estimator time
			strcat( trace_str, ", estimator_time, ");
			itoa( bike_pov_st.compare_time, temp_str , 10);
			strcat( trace_str, temp_str );
			strcat( trace_str, "\n" );	
			res = pf_write( trace_str, strlen( trace_str ), &bytes_writen );
			if ( res == FR_OK 				) 	{ asm("nop"); } // OK
			break;
		case SD_INT0:
			strcpy( trace_str, "INT1, " );
			
			// display offset
			strcat( trace_str, ", display_offset, ");
			itoa( bike_pov_st.display_offset, temp_str, 10 );
			strcat( trace_str, temp_str );
			strcat( trace_str, "\n" );
			res = pf_write( trace_str, strlen(trace_str) , &bytes_writen );
			break;
		case SD_INT1:
			strcpy( trace_str, "INT1, " );
			
			// display offset
			strcat( trace_str, ", display_offset, ");
			itoa( bike_pov_st.display_offset, temp_str, 10 );
			strcat( trace_str, temp_str );
			strcat( trace_str, "\n" );
			res = pf_write( trace_str, strlen(trace_str) , &bytes_writen );
			break;
		case SD_INIT:
			res = pf_mount( &fs_s );
			if ( res != FR_OK )
			{
				pf_mount( &fs_s ); // second try
			}
			res = pf_open( "log.txt" );
			if ( res == FR_OK 				) 	{ asm("nop"); } // OK
			strcpy( trace_str, "SD_START, \n" );
			res = pf_write( trace_str, strlen( trace_str ), &bytes_writen );
			if ( res == FR_OK 				) 	{ asm("nop"); } // OK 
			break;
		case SD_FINALIZE:
			if( pf_write( 0 , 0, &bytes_writen ) ) asm("nop");
		case SD_OPEN_LOG:
			if( pf_open( "log.txt" ) ) asm("nop");
		case SD_OPEN_IM:
			if( pf_open( "im_name.img" )) asm("nop");
		case SD_SLEEP:
			strcpy( trace_str, "SLEEP, \n" );
			res = pf_write( 0, 0, &bytes_writen );
		default:
			break;
	}

	bike_pov_st.trace_cnt++;
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
