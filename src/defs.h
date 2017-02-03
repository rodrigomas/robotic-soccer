#ifndef DEFS_H
#define DEFS_H

#ifndef M_PI
#define M_PI 3.1415926
#endif

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

#define ANIM_INC 100.0
#define ANIM_SEEK 2

//#define TO_RADIANS(g) (g) / 180.0 * M_PI
//#define TO_DEGREES(g) (g) * 180.0 / M_PI
//#define POW2(g) (g)*(g)

double TO_RADIANS( double v );

double TO_DEGREES( double v );

double POW2( double v );

#endif
