#ifndef SCENE_4_H
#define SCENE_4_H

#include "scene.h"
#include "button.h"
#include "info.h"
#include "gamedata.h"
#include "common.h"

extern soccer::SGameData *gdata;

namespace soccer {

	class CScene04 : public CScene
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

		CButton btGo;

		void btGo_click(void)
		{
			ChangeScene(1);
		}

		struct pass_info *info;

		char output[255];

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

			glColor4f(1.0f,1.0f,1.0f,1.0f);
			// Dados do Jogo ( TIME 1 )
			glRasterPos2f( -0.85 ,0.45);
			puts2DBitmap("GOLS:", GLUT_BITMAP_8_BY_13);
			for( register int i = 0 ; i < info->team01->nGols ; i++)
			{
				glRasterPos2f( -0.82 ,0.4 - i * 0.05);
				puts2DBitmap(info->team01->Gols[i], GLUT_BITMAP_8_BY_13);
			}

			glRasterPos2f( -0.85 ,0.4 - 0.05 * info->team01->nGols );
			puts2DBitmap("CARTOES:", GLUT_BITMAP_8_BY_13);
			for( register int i = 0 ; i < info->team01->nCartoes ; i++)
			{
				if ( info->team01->Cartoes[i][CARD_TYPE] == 'v' ) {
					glColor4f(1.0f,0.0f,0.0f,1.0f);
				} else {
					glColor4f(1.0f,1.0f,0.0f,1.0f);
				}

				glRasterPos2f( -0.82 ,0.4 - (i + info->team01->nGols + 1 ) * 0.05);
				puts2DBitmap(info->team01->Cartoes[i], GLUT_BITMAP_8_BY_13);
			}

			glColor4f(0.0f,0.0f,0.0f,1.0f);

			glRasterPos2f(-0.41,-0.23);
			sprintf(output,"%d",info->team01->nGols);
			puts2DBitmap(output, GLUT_BITMAP_9_BY_15);

			glRasterPos2f(-0.41,-0.33);
			sprintf(output,"%d",info->team01->nFaltas);
			puts2DBitmap(output, GLUT_BITMAP_9_BY_15);

			glRasterPos2f(-0.41,-0.45);
			sprintf(output,"%d",info->team01->nLaterais);
			puts2DBitmap(output, GLUT_BITMAP_9_BY_15);

			glRasterPos2f(-0.41,-0.56);
			sprintf(output,"%d",info->team01->nEscanteios);
			puts2DBitmap(output, GLUT_BITMAP_9_BY_15);

			glRasterPos2f(-0.43,-0.67);
			sprintf(output,"%.2f%%",info->team01->Posse);
			puts2DBitmap(output, GLUT_BITMAP_9_BY_15);


			glColor4f(0.0f,0.0f,0.0f,1.0f);
			glRasterPos2f( 0.3 ,0.53);
			puts2DBitmap(gdata->team2->name.c_str(), GLUT_BITMAP_HELVETICA_18);
			// Dados do Jogo ( TIME 2 )
			glColor4f(1.0f,1.0f,1.0f,1.0f);
			glRasterPos2f( 0.3 , 0.45);
			puts2DBitmap("GOLS:", GLUT_BITMAP_8_BY_13);
			for( register int i = 0 ; i < info->team02->nGols ; i++)
			{
				glRasterPos2f( 0.33 , 0.4 - i * 0.05);
				puts2DBitmap(info->team02->Gols[i], GLUT_BITMAP_8_BY_13);
			}

			glRasterPos2f( 0.3 ,0.4 - 0.05 * info->team02->nGols );
			puts2DBitmap("CARTOES:", GLUT_BITMAP_8_BY_13);
			for( register int i = 0 ; i < info->team02->nCartoes ; i++)
			{
				if ( info->team02->Cartoes[i][CARD_TYPE] == 'v' ) {
					glColor4f(1.0f,0.0f,0.0f,1.0f);
				} else {
					glColor4f(1.0f,1.0f,0.0f,1.0f);
				}

				glRasterPos2f( 0.33 ,0.4 - (i + info->team02->nGols + 1 ) * 0.05);
				puts2DBitmap(info->team02->Cartoes[i], GLUT_BITMAP_8_BY_13);
			}

			glColor4f(0.0f,0.0f,0.0f,1.0f);

			glRasterPos2f(0.73,-0.23);
			sprintf(output,"%d",info->team02->nGols);
			puts2DBitmap(output, GLUT_BITMAP_9_BY_15);

			glRasterPos2f(0.73,-0.33);
			sprintf(output,"%d",info->team02->nFaltas);
			puts2DBitmap(output, GLUT_BITMAP_9_BY_15);

			glRasterPos2f(0.73,-0.45);
			sprintf(output,"%d",info->team02->nLaterais);
			puts2DBitmap(output, GLUT_BITMAP_9_BY_15);

			glRasterPos2f(0.73,-0.56);
			sprintf(output,"%d",info->team02->nEscanteios);
			puts2DBitmap(output, GLUT_BITMAP_9_BY_15);

			glRasterPos2f(0.71,-0.67);
			sprintf(output,"%.2f%%",info->team02->Posse);
			puts2DBitmap(output, GLUT_BITMAP_9_BY_15);

			glRasterPos2f(-0.13,0.8);
			if( info->team01->nGols > info->team02->nGols ) {
				glColor4f(1.0f,0.0f,0.0f,1.0f);

				std::string val = gdata->team1->name;

				print_upper(val);

				val = val + " VENCEU!";

				//puts2DBitmap("FLAMENGO VENCEU!", GLUT_BITMAP_HELVETICA_18);
				puts2DBitmap(val.c_str(), GLUT_BITMAP_HELVETICA_18);
			} else if( info->team02->nGols > info->team01->nGols ) {
				glColor4f(0.0f,0.0f,0.0f,1.0f);

				std::string val = gdata->team2->name;

				print_upper(val);

				val = val + " VENCEU!";

				puts2DBitmap(val.c_str(), GLUT_BITMAP_HELVETICA_18);
//				puts2DBitmap("BOTAFOGO VENCEU!", GLUT_BITMAP_HELVETICA_18);
			} else {
				glColor4f(0.0f,0.0f,0.0f,1.0f);
				puts2DBitmap("     EMPATE     ", GLUT_BITMAP_HELVETICA_18);
			}

			if ( fadeIn != 0 ) {
				glColor4f(0.0f,0.0f,0.0f,fadeIn / (double)fade_steps);
				glutRect();
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
			//glutPostRedisplay();
		}

		CScene04( int fs )  : CScene(fs),
			     Back(0,0,0,0),
			   btGo(0,0,0, 0.7,-0.9, 0.4,0.1,Language[LG_SC4_BT1])
		{
			size2 = 1.0f;

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			BackImage = LoadTexture("textures/results.jpg",true);

			//btGo.OnRelease = btGo_click;

			width = glutGet(GLUT_WINDOW_WIDTH);
			height = glutGet(GLUT_WINDOW_HEIGHT);

			info = NULL;

		}

		~CScene04()
		{

			if ( info != NULL ) {
				deleteInfo(info->team01);
				deleteInfo(info->team02);
				delete info;
			}
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
			if ( info != NULL )
				this->info = (struct pass_info*)info;
		}

	};

};

#endif
