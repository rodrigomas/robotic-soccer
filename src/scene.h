#ifndef SCENE_H
#define SCENE_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

namespace soccer {

	class CScene 
	{

	protected:
		virtual void ReDefine( void )  = 0;

		int fadeIn;

		int fadeOut;

	public:

		virtual void glutSpecial( int key, int x, int y) = 0;

		virtual void glutKeyboard( unsigned char key, int x, int y) = 0;

		virtual void glutMouse( int button, int state, int x, int y) = 0;

		virtual void glutMotion( int x, int y) = 0;

		virtual void glutPassiveMotion( int x, int y) = 0;

		virtual void glutDisplay(void) = 0;		

		virtual void glutReshape( int w, int h ) = 0;

		virtual void glutIdle(void) = 0;

		virtual void setWH( int w, int h) = 0;

		virtual void getWH( int &w, int &h) = 0;
		
		virtual void Display(void) {};

		CScene( int fs )
		{
			fade_steps = fs;
		}

		int fade_steps;

		void startFadeIn(void)
		{
			fadeIn = fade_steps;
		}

		void startFadeOut(void)
		{
			fadeOut = 0;
		}

		void stopFadeIn(void)
		{
			fadeIn = 0;
		}

		void stopFadeOut(void)
		{
			fadeOut = fade_steps;
		}

		void nextFadeIn(void)
		{
			fadeIn--;
		}

		void nextFadeOut(void)
		{
			fadeOut++;
		}

		virtual ~CScene() {};

	};

};

#endif
