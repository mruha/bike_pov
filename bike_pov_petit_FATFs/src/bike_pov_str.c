/*
 * bike_pov_str.c
 *
 * Created: 12/26/2012 12:48:52 PM
 *  Author: mruha
 */
 
 #include <stdlib.h>
 #include <string.h>
 #include "../hdr/bike_pov_type.h"
 #include "../hdr/bike_pov_str.h"

/****************************************************************************************************************
 *function:
 *	bike_pov_str_form_trace
 *arguments:
 *	*trace_str	:		string buffer pointer to store convert
 * 	*trace_len	:		number value to convert
 *  *str_to_add	:		string to append
 *  num_to_add	:		integer value to append as string
 * 	mode		:		type of string to append
 *return value:
 *	len			:		length of string
 */
void bike_pov_str_form_trace( char* trace_str, u8* trace_len, char* str_to_add, u32 num_to_add, str_mode mode )
{
	char temp_str[9];
	u8 temp_str_len;
	
	if ( mode == STR )
	{
		strcat( trace_str, str_to_add );		// append string
		trace_len += strlen( str_to_add );		// increment total trace length
	}
	else if ( mode == NUM )
	{
		temp_str_len = 	ltoa( num_to_add, temp_str, 10 );	// convert integer to string
		strcat( trace_str, &temp_str[0] );					// append
		*trace_len = temp_str_len;							// increment total trace length
	}
}
