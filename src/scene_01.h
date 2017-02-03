#ifndef SCENE_1_H
#define SCENE_1_H

#include "scene.h"
#include "button.h"
#include "fade.h"
#include "common.h"

namespace soccer {


	class CScene01: public CScene
	{

		Color Back;

		TEX *BackImage;

		double fAspect;

		double width;

		double height;

		double size2;

		virtual void ReDefine( void )
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			if ( fAspect <= 1.0f )
				gluOrtho2D (-size2, size2, -size2/fAspect, size2/fAspect);
			else
				gluOrtho2D (-size2*fAspect, size2*fAspect, -size2, size2);

			//gluOrtho2D(-1.0f,1.0f,-1.0f,1.0f);
			glMatrixMode(GL_MODELVIEW);
		}

		CButton btNewPxP;

		CButton btNewPxC;

		CButton btCredits;

		CButton btControls;

		CButton btExit;

		void btNewPxP_click(void)
		{
			SceneFadeOut(fadeto32,this);
		}

		static void fadeto32(void)
		{
			ChangeScene(3,(void*)2);
		}

		void btControls_click(void)
		{
			ChangeScene(6);
		}

		static void fadeto31(void)
		{
			ChangeScene(3,(void*)1);
		}

		void btNewPxC_click(void)
		{
			SceneFadeOut(fadeto31,this);
		}

		void btCredits_click(void)
		{
			ChangeScene(2);
		}

		void btExit_click(void)
		{
			Terminate();
		}

	public:

		virtual void glutSpecial( int key, int x, int y)
		{

		}

		virtual void glutKeyboard( unsigned char key, int x, int y)
		{
			switch (key) {

				case 'q':
					Terminate();
					break;
				#ifdef USE_MUSIC
				case '+':
					ogg.increase(5);
				break;

				case '-':
					ogg.decrease(5);
				break;

				case 'M':
				case 'm':
					ogg.mute();
				break;
				#endif
			}
		}

		virtual void glutMouse( int button, int state, int x, int y)
		{
			double X = 2 * x / width - 1;
			double Y = 1 - 2 * y / height;

			if( btNewPxP.Click(button,state,X,Y) == RELEASE )
				btNewPxP_click();

			if( btNewPxC.Click(button,state,X,Y) == RELEASE )
				btNewPxC_click();

			if( btControls.Click(button,state,X,Y) == RELEASE )
				btControls_click();

			if( btCredits.Click(button,state,X,Y) == RELEASE )
				btCredits_click();

			if( btExit.Click(button,state,X,Y) == RELEASE )
				btExit_click();

		}

		virtual void glutMotion( int x, int y)
		{
			double X = 2 * x / width - 1;
			double Y = 1 - 2 * y / height;

			btNewPxP.Motion(X,Y);
			btNewPxC.Motion(X,Y);
			btControls.Motion(X,Y);
			btCredits.Motion(X,Y);
			btExit.Motion(X,Y);
		}

		virtual void glutPassiveMotion( int x, int y)
		{
			double X = 2 * x / width - 1;
			double Y = 1 - 2 * y / height;

			btNewPxP.Motion(X,Y);
			btNewPxC.Motion(X,Y);
			btCredits.Motion(X,Y);
			btControls.Motion(X,Y);
			btExit.Motion(X,Y);
		}

		virtual void glutDisplay(void)
		{
			glClearColor(Back.R, Back.G, Back.B, 0);
			glClear(GL_COLOR_BUFFER_BIT);

			//glRasterPos2i( -1, -1 );
			//glDrawPixels(BackImage->dimx, BackImage->dimy, GL_RGB, GL_UNSIGNED_BYTE, BackImage->data);
			glColor3f(1.0, 1.0, 1.0);
			glBindTexture(GL_TEXTURE_2D, BackImage->texid);
			glEnable(GL_TEXTURE_2D);
			glBegin(GL_QUAD_STRIP);
				glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
				glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,-1.0f);
				glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, 1.0f);
				glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,-1.0f);
			glEnd();
			glDisable(GL_TEXTURE_2D);

			btNewPxP.Draw();
			btNewPxC.Draw();
			btControls.Draw();
			btCredits.Draw();
			btExit.Draw();

			// Executa os comandos OpenGL
			if ( fadeIn != 0 ) {
				glColor4f(0.0f,0.0f,0.0f,fadeIn / (double)fade_steps);
				glutRect();
			}

			if ( fadeOut != fade_steps ) {
				glColor4f(0.0f,0.0f,0.0f,fadeOut/(double)fade_steps);
				glutRect();
			}

			glutSwapBuffers();
		}

		virtual void glutReshape( int w, int h )
		{
			// Para previnir uma divisão por zero
			if ( h == 0 ) h = 1;

			// Especifica o tamanho da viewport
			glViewport(0, 0, w, h);

			// Calcula a correção de aspecto
			//fAspect = (GLfloat)w/(GLfloat)h;
			fAspect = 1; // Sem correção de aspecto
			width = w;
			height = h;

			ReDefine();
		}

		virtual void glutIdle(void)
		{
			//glutPostRedisplay();
		}

		CScene01( int fs )  : CScene(fs),
			     Back(0,0,0,0),
			 btNewPxP(0,0,0, 0.0, 0.2, 0.6,0.1,Language[LG_SC1_BT1]),
			 btNewPxC(0,0,0, 0.0, 0.1, 0.6,0.1,Language[LG_SC1_BT2]),
			btCredits(0,0,0, 0.0,-0.1, 0.6,0.1,Language[LG_SC1_BT3]),
		       btControls(0,0,0, 0.0, 0.0, 0.6,0.1,Language[LG_SC1_BT4]),
			   btExit(0,0,0, 0.0,-0.2, 0.6,0.1,Language[LG_SC1_BT5])
//			 btNewPxP(0,0,0, 0.0, 0.2, 0.6,0.1,"Novo Jogo PxP"),
//			 btNewPxC(0,0,0, 0.0, 0.1, 0.6,0.1,"Novo Jogo PxC"),
//			btCredits(0,0,0, 0.0,-0.1, 0.6,0.1,"Creditos"),
//		       btControls(0,0,0, 0.0, 0.0, 0.6,0.1,"Controles"),
//			   btExit(0,0,0, 0.0,-0.2, 0.6,0.1,"Sair")
		{
			size2 = 1.0f;

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			//BackImage = LoadJPG("textures/main.jpg");
			BackImage = LoadTexture("textures/main.jpg",true);

			/*  btExit.OnRelease = btExit_click;
			btNewPxP.OnRelease = btNewPxP_click;
			btNewPxC.OnRelease = btNewPxC_click;
			btCredits.OnRelease = btCredits_click;
			btControls.OnRelease = btControls_click;*/

			//width = SCR_WIDTH;
			//height = SCR_HEIGHT;

			width = glutGet(GLUT_WINDOW_WIDTH);
			height = glutGet(GLUT_WINDOW_HEIGHT);

			fadeIn = fs;
			fadeOut = fs;

		}

		~CScene01()
		{

		}

		void setWH( int w, int h)
		{
			width = w;
			height = h;
		}

		void getWH( int &w, int &h)
		{
			w = (int)width;
			h = (int)height;
		}

	};

};

#endif
