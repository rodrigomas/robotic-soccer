#ifndef SCENE_2_H
#define SCENE_2_H

#include "scene.h"
#include "button.h"
#include "common.h"

namespace soccer {

	class CScene02 : public CScene
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

		CButton btBack;

		void btBack_click(void)
		{
			ChangeScene(1);
		}

	public:

		virtual void glutSpecial( int key, int x, int y)
		{

		}

		virtual void glutKeyboard( unsigned char key, int x, int y)
		{
			switch (key) {

				case 'q':
					ChangeScene(1);
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

			if( btBack.Click(button,state,X,Y) == RELEASE ) {
				btBack_click();
			}
		}

		virtual void glutMotion( int x, int y)
		{
			double X = 2 * x / width - 1;
			double Y = 1 - 2 * y / height;

			btBack.Motion(X,Y);
		}

		virtual void glutPassiveMotion( int x, int y)
		{
			double X = 2 * x / width - 1;
			double Y = 1 - 2 * y / height;

			btBack.Motion(X,Y);
		}

		virtual void glutDisplay(void)
		{
			glClearColor(Back.R, Back.G, Back.B, 0);
			glClear(GL_COLOR_BUFFER_BIT);

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

			btBack.Draw();

			/*if ( fadeIn != 0 ) {
				glColor4f(0.0f,0.0f,0.0f,fadeIn / (double)fade_steps);
				glutRect();
			}*/

			// Executa os comandos OpenGL
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

		CScene02( int fs )  : CScene(fs),
			     Back(0,0,0,0),
			   btBack(0,0,0, 0.7,-0.8, 0.4,0.1,Language[LG_SC2_BT1])
		{
			size2 = 1.0f;

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			BackImage = LoadTexture("textures/credits.jpg",true);

			//btBack.OnRelease = btBack_click;

			width = glutGet(GLUT_WINDOW_WIDTH);
			height = glutGet(GLUT_WINDOW_HEIGHT);

		}

		~CScene02()
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
