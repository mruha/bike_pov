/*
 * bike_pov.h
 *
 * Created: 12/22/2012 4:49:34 PM
 *  Author: mruha
 */ 

#define WAKE_UP 		1
#define LED_DEBUG 		0
#define WHEIGHTED_MEAN	1

#ifndef BIKE_POV_H_
#define BIKE_POV_H_

#ifdef BIKE_POV_C_
	#define _OWNER_
#endif
#include "hdr_owner.h"
#include "bike_pov_type.h"
#include "bike_pov_port_map.h"


// variables
SHARED_VAR(bike_pov_data_st, bike_pov_st)

// functions
SHARED_FUNC(void, bike_pov_init, void)
SHARED_FUNC(void, bike_pov_compare_estimator, void)
#undef _OWNER_
#endif /* BIKE_POV_H_ */
