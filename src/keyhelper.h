#ifndef KEY_HELPER_H
#define KEY_HELPER_H

#define N_KEYS 256
#define N_S_KEYS 110

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <memory.h>
#include <stdio.h>

namespace soccer {

void InitHelper(void);

void keyboard(unsigned char key, int x, int y);

void specialKeyboardUp(unsigned char key, int x, int y);

void specialKeyboard(int key, int x, int y);

void keyboardUp(int key, int x, int y);

bool checkKey( unsigned char key );

bool checkKeyUpper( unsigned char key );

bool checkSpecialKey( int key );

int GetModifiers(void);

void joystick(unsigned int buttonMask, int x, int y, int z);

bool checkjoyBtn( int bt );

int getjoyAxis( int ax );

bool getjoyAxisB( int ax, int *axis );


};

#endif
