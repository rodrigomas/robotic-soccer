#ifndef SCENE_3_H
#define SCENE_3_H

#include "scene.h"
#include "button.h"
#include "common.h"
#include "gamedata.h"

extern soccer::SGameData *gdata;

namespace soccer {

	class CScene03 : public CScene
	{

		Color Back;

		TEX *BackImage, *LoadingImage;

		double fAspect;

		double width;

		double height;

		double size2;

		char buffer[256];

		int nPlayers;

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

		CButton btGo;

		void btGo_click(void)
		{
            loading = true;
            glutDisplay();
			ChangeScene(5,(void*)nPlayers);
		}

	public:

        bool loading;

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

			if( btGo.Click(button,state,X,Y) == RELEASE )
			{
				btGo_click();
			}
		}

		virtual void glutMotion( int x, int y)
		{
			double X = 2 * x / width - 1;
			double Y = 1 - 2 * y / height;

			btGo.Motion(X,Y);
		}

		virtual void glutPassiveMotion( int x, int y)
		{
			double X = 2 * x / width - 1;
			double Y = 1 - 2 * y / height;

			btGo.Motion(X,Y);
		}

		virtual void glutDisplay(void)
		{
			glClearColor(Back.R, Back.G, Back.B, 0);
			glClear(GL_COLOR_BUFFER_BIT);

			glColor3f(1.0, 1.0, 1.0);
            if( !loading ) {
                glBindTexture(GL_TEXTURE_2D, BackImage->texid);
                glEnable(GL_TEXTURE_2D);
                glBegin(GL_QUAD_STRIP);
                    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
                    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,-1.0f);
                    glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, 1.0f);
                    glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,-1.0f);
                glEnd();
                glDisable(GL_TEXTURE_2D);

                btGo.Draw();

                glColor4f(0.0f,0.0f,0.0f,1.0f);
                glRasterPos2f( -0.85 ,0.53);
                puts2DBitmap(gdata->team1->name.c_str(), GLUT_BITMAP_HELVETICA_18);

                for( register int i = 0 ; i < gdata->team1->nplayers ; i++ ) {
                    glRasterPos2f( -0.85 ,0.45 - 0.06 * i);
                    sprintf(buffer,"%2d - %s", gdata->team1->players[i].number,
                                   gdata->team1->players[i].name.c_str());

                    puts2DBitmap(buffer,GLUT_BITMAP_HELVETICA_18);
                }

                glColor4f(1.0f,1.0f,1.0f,1.0f);
                glRasterPos2f( -0.85 ,-0.23);
                puts2DBitmap(("T: " + gdata->team1->coach).c_str(), GLUT_BITMAP_HELVETICA_18);

                glColor4f(0.0f,0.0f,0.0f,1.0f);
                glRasterPos2f( 0.3 , 0.53);
                puts2DBitmap(gdata->team2->name.c_str(), GLUT_BITMAP_HELVETICA_18);

                for( register int i = 0 ; i < gdata->team2->nplayers ; i++ ) {
                    glRasterPos2f( 0.3 ,0.45 - 0.06 * i);

                    sprintf(buffer,"%2d - %s", gdata->team2->players[i].number,
                                   gdata->team2->players[i].name.c_str());

                    puts2DBitmap(buffer,GLUT_BITMAP_HELVETICA_18);
                }

                glColor4f(1.0f,1.0f,1.0f,1.0f);
                glRasterPos2f( 0.3 , -0.23);
                puts2DBitmap(("T: " + gdata->team2->coach).c_str(), GLUT_BITMAP_HELVETICA_18);

                if ( fadeIn != 0 ) {
                    glColor4f(0.0f,0.0f,0.0f,fadeIn / (double)fade_steps);
                    glutRect();
                }
            } else {
                glBindTexture(GL_TEXTURE_2D, LoadingImage->texid);
                glEnable(GL_TEXTURE_2D);
                glBegin(GL_QUAD_STRIP);
                    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
                    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,-1.0f);
                    glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, 1.0f);
                    glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,-1.0f);
                glEnd();
                glDisable(GL_TEXTURE_2D);
            }
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

		}

		CScene03( int fs )  : CScene(fs),
			     Back(0,0,0,0),
			   btGo(0,0,0, 0.7,-0.9, 0.4,0.1,Language[LG_SC3_BT1])
		{
			size2 = 1.0f;

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			BackImage = LoadTexture("textures/scale.jpg",true);
			LoadingImage = LoadTexture("textures/load.jpg",true);

			//btGo.OnRelease = btGo_click;

			width = glutGet(GLUT_WINDOW_WIDTH);
			height = glutGet(GLUT_WINDOW_HEIGHT);

			nPlayers = 1;

			loading = false;

		}

		~CScene03()
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

		void getInfo( void* info )
		{
			nPlayers = (int)info;
		}

	};

};

#endif
