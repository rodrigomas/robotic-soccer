#ifndef SOUND_H
#define SOUND_H

#include <AL/al.h>
#include <AL/alc.h>
//#include <AL/alut.h>
#include "wav.h"
#include "gamedata.h"

#ifdef USE_MUSIC
#include "ogg.h"
#endif

// These index the buffers and sources.
#define APITO          		0
#define COLISAO_JxJ    		1
#define GRITO_TORCIDA      	2
#define TORCIDA_NORMAL      	3
#define COLISAO_SOCO    	4
#define LOSE      		5
#define RECEBE_SOCO      	6
#define TORCIDA_PROXIMO 	7
#define FALTA      		8
#define MOUSE_CLICK     	9
#define CHUTE    		10
#define GOL     		11
#define MOUSE_OVER      	12
#define WIN    			13

extern soccer::SGameData *gdata;

#ifdef USE_MUSIC
extern ogg_stream ogg;
#endif
//using namespace audiere;
#ifdef USE_MUSIC
extern bool ogg_play;
#endif

void playSound( int Source );

ALboolean LoadALData();

void setGain( int Source, int value );

void setSourcePos( int Source, float x, float y, float z );

void setListenerPos( float x, float y, float z );

void setListenerVel( float x, float y, float z );

void setSourceVel( int Source, float x, float y, float z );

void setListenerOri( float x, float y, float z );

void SetListenerValues();

bool initOpenAL(void);;

void KillALData();

void StopMusic(void);

void StartMusic(void);

void aluMain(int *argc, char **argv);


#endif
