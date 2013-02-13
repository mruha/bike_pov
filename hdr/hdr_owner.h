/*
 * hdr_owner.h
 *
 * Created: 12/22/2012 4:50:18 PM
 *  Author: mruha
 */ 


#ifndef HDR_OWNER_H_
#define HDR_OWNER_H_

#ifdef _OWNER_

	// variables
	#define SHARED_VAR_INIT(type,var,val)		type var = {val};
	#define SHARED_VAR(type,var)				type var;
	// functions
	#define SHARED_FUNC(type, name, val)		type name(val);
#else //not an owner
	// variables
	#define SHARED_VAR_INIT(type,var,val)		extern type var;
	#define SHARED_VAR(type,var)				extern type var;
	// functions
	#define SHARED_FUNC(type,name,val)			extern type name(val);
#endif // #ifdef OWNER


#endif /* HDR_OWNER_H_ */