#include "defs.h"

double TO_RADIANS( double v ) 
{
	return (v / 180.0 * M_PI);
}

double TO_DEGREES( double v ) 
{
	return (v * 180.0 / M_PI);
}

double POW2( double v ) 
{
	return (v * v);
}
