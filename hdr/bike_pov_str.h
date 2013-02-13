/*
 * bike_pov_str.h
 *
 * Created: 12/26/2012 12:48:52 PM
 *  Author: mruha
 */

typedef enum
{
	NUM = 0,
	STR
} str_mode;

void bike_pov_str_form_trace	( char* trace_str, u8* trace_len, char* str_to_add, u32 num_to_add, u8 mode );
