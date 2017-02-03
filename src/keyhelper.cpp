#include "keyhelper.h"
#include "common.h"

int KeyDown[N_KEYS] ;

int SpecialKeyDown[N_S_KEYS];

int Modifiers = 0;

bool joyButton[4];
int joyAxis[3];

namespace soccer {

void InitHelper(void)
{
	memset( KeyDown, 0, N_KEYS );
	memset( SpecialKeyDown, 0, N_S_KEYS );
}

void keyboard(unsigned char key, int x, int y)
{
	Modifiers = glutGetModifiers();
    	KeyDown[key] = 1;
}

void specialKeyboardUp(unsigned char key, int x, int y)
{
	Modifiers = glutGetModifiers();
    	SpecialKeyDown[key] = 0;
}

void specialKeyboard(int key, int x, int y)
{
    	Modifiers = glutGetModifiers();
	SpecialKeyDown[key] = 1;
}

void keyboardUp(int key, int x, int y)
{
	Modifiers = glutGetModifiers();
    	KeyDown[key] = 0;
}

bool checkKey( unsigned char key )
{
	return (KeyDown[key] == 1);
}

bool checkKeyUpper( unsigned char key )
{
	return (KeyDown[key] == 1 || KeyDown[toupper(key)] == 1);
}

bool checkSpecialKey( int key )
{
	if ( key >= N_S_KEYS ) return false;
	return (SpecialKeyDown[key] == 1);
}

int GetModifiers(void)
{
	return Modifiers;
}

void joystick(unsigned int buttonMask, int x, int y, int z)
{
    joyButton[0] = buttonMask & GLUT_JOYSTICK_BUTTON_A;
    joyButton[1] = buttonMask & GLUT_JOYSTICK_BUTTON_B;
    joyButton[2] = buttonMask & GLUT_JOYSTICK_BUTTON_C;
    joyButton[3] = buttonMask & GLUT_JOYSTICK_BUTTON_D;

    joyAxis[0] = x;
    joyAxis[1] = y;
    joyAxis[2] = z;

    //fprintf(stderr,"joy 0x%x, x=%d y=%d z=%d\n" , buttonMask, x, y, z);

}

bool checkjoyBtn( int bt )
{
    return joyButton[bt];
}

int getjoyAxis( int ax )
{
    return joyAxis[ax & JOY_PLUS_CLEAR];
}

bool getjoyAxisB( int ax, int *axis )
{
    int i = ax & JOY_PLUS_CLEAR;
    //fprintf(stderr,"i=%d j=%d a=%d;%d\n" , i, joyAxis[i], axis[i], axis[i+3]);
    if( (ax & JOY_PLUS_SET) != 0 ) { // +
      //  fprintf(stderr,"+\n");
        return (joyAxis[i] > axis[i]);
    } else {
        //fprintf(stderr,"-\n");
        return (joyAxis[i] < axis[i+3]);
    }

//    return false;
}

};
