#include "fade.h"

CScene *CurrFadeScene;

int fade_timer = 100;

fFade fadeout = NULL;
fFade fadein  = NULL;

void glutFadeIn( int value ) 
{	
	value--;
	CurrFadeScene->nextFadeIn();
	glutPostRedisplay();

	if ( value > 0 ) {
		glutTimerFunc(fade_timer, glutFadeIn, value);
	} else {
		if ( fadein ) { 
			fadein();
			fadein = NULL;
		}
	}
}

void glutFadeOut( int value ) 
{	
	value++;
	CurrFadeScene->nextFadeOut();
	glutPostRedisplay();

	if ( value < CurrFadeScene->fade_steps ) { 
		glutTimerFunc(fade_timer, glutFadeOut, value);
	} else {
		if ( fadeout ) { 
			fadeout();
			fadeout = NULL;
		}
	}
}

void SceneFadeIn( fFade afadein, CScene *Scene) 
{
	CurrFadeScene = Scene;

	if ( CurrFadeScene ) {
		CurrFadeScene->startFadeIn();
	} else {
		return;
	}

	fadein = afadein;
	
	glutTimerFunc(fade_timer, glutFadeIn, CurrFadeScene->fade_steps);
}

void SceneFadeOut( fFade afadeout, CScene *Scene )
{
	CurrFadeScene = Scene;

	if ( CurrFadeScene ) {
		CurrFadeScene->startFadeOut();
	} else {
		return;
	}

	fadeout = afadeout;
	
	glutTimerFunc(fade_timer, glutFadeOut, 0);
}

void initEffects( int ft )
{
	fade_timer = ft;
}

