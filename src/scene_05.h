#ifndef SCENE_5_H
#define SCENE_5_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "defs.h"
#include "scene.h"
#include "button.h"
#include "camera.h"
#include "info.h"
#include "gamedata.h"
#include "flagman.h"
#include "sky.h"
#include "soccerfield.h"
#include "ball.h"
#include "lights.h"
#include "physics.h"
#include "rules.h"
#include "placar.h"
#include "player.h"
#include "keyhelper.h"

using std::string;
using std::ostringstream;
extern lua_State *lua_team1;
extern lua_State *lua_team2;
extern soccer::SGameData *gdata;
extern int LastCollisionIndex;

namespace soccer {

	extern ERuleState ruleState;

	extern const int draw_mode;

	class CScene05 : public CScene
	{

		Color Back;

		double fAspect;

		double width, height, size2;

		int nPlayers;

		TEX *team01, *team02, *imclock, *field;

		virtual void ReDefine(void) {};

		CFocusCamera Camera01, Camera02, Camera03, Camera05, Camera06;
		CWalkCamera Camera04;

		CCamera *CurrCamera;

		CFlagMan *rightFlag;

		CFlagMan *leftFlag;

		CSky *sky;

		CPlacar *placar;

		CSoccerField Field;

		CBall Ball;

		bool Paused, End;

		double ClockMin, ClockSec;

		char output[255];

		clock_t start, end;
		double elapsed;

		SInfo *Team01, *Team02;

		int nLights;

		CLight **Lights;

		bool Team01Ball;

		CPlayer *CurrPlayer01;

		int CurrPlayerIndex0;

		CPlayer *CurrPlayer02;

		int CurrPlayerIndex1;

		CPlayer **Team01Players;

		CPlayer **Team02Players;

		int OldCamX, OldCamY;

		bool CamRotate;

		ERuleApplyed gameStatus;

		bool WaitForKick;

		bool autoSeek, drawLights;

		int MapX;
		bool ApperMap;

		bool motionblur;

	public:

		virtual void glutSpecial( int key, int x, int y)
		{

//			int Modifiers = glutGetModifiers();

//			int Extra = ( Modifiers & GLUT_ACTIVE_SHIFT ) ? 100 : 10;

			switch (key) {

				case GLUT_KEY_F1:
					CurrCamera = &Camera01;
					if ( Paused ) glutPostRedisplay();
				break;

				case GLUT_KEY_F2:
					CurrCamera = &Camera02;
					if ( Paused ) glutPostRedisplay();
				break;

				case GLUT_KEY_F3:
					CurrCamera = &Camera03;
					if ( Paused ) glutPostRedisplay();
				break;

				case GLUT_KEY_F4:
					CurrCamera = &Camera04;
					if ( Paused ) glutPostRedisplay();
				break;

				case GLUT_KEY_F5:
					if( nPlayers != 2 ) {
						CurrCamera = &Camera05;
						if ( Paused ) glutPostRedisplay();
					}
				break;

				case GLUT_KEY_F6:
					if( nPlayers != 2 ) {
						CurrCamera = &Camera06;
						if ( Paused ) glutPostRedisplay();
					}
				break;

				case GLUT_KEY_PAGE_DOWN:

					if ( CurrPlayerIndex0 == 0 )
						CurrPlayerIndex0 = gdata->team1->nplayers - 1;
					else
 						CurrPlayerIndex0 = abs(CurrPlayerIndex0 - 1) % gdata->team1->nplayers ;

 					CurrPlayer01 = Team01Players[CurrPlayerIndex0];
 					if ( Paused ) glutPostRedisplay();
				break;

				case GLUT_KEY_PAGE_UP:
					CurrPlayerIndex0 = (abs(CurrPlayerIndex0 + 1)) % gdata->team1->nplayers;
					CurrPlayer01 = Team01Players[CurrPlayerIndex0];
					if ( Paused ) glutPostRedisplay();
				break;

			}
		}

		virtual void glutKeyboard( unsigned char key, int x, int y)
		{

//			int Modifiers = glutGetModifiers();

			struct pass_info *pass;

			switch (key) {

				case '1':
					autoSeek = !autoSeek;
				break;

				case 'Q':
				case 'q':
					pass = new struct pass_info;

					pass->team01 = Team01;
					pass->team02 = Team02;

                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    gluOrtho2D (-size2, size2, -size2,size2);
					ChangeScene(4, (void*)pass);
				break;

				case 'p':
				case 'P':
					Paused = !Paused;
                    glutPostRedisplay();
					end = clock();
				break;

				case 'r':
				case 'R':
					Camera04.setPos(0,13,20);
					Camera04.resetUp();
				break;

				case 'u':
				case 'U':

					if ( CurrPlayerIndex1 == 0 )
						CurrPlayerIndex1 = gdata->team2->nplayers- 1;
					else
 						CurrPlayerIndex1 = abs(CurrPlayerIndex1 - 1) % gdata->team2->nplayers ;

 					CurrPlayer02 = Team02Players[CurrPlayerIndex1];
 					if ( Paused ) glutPostRedisplay();
				break;

				case 'i':
				case 'I':
					CurrPlayerIndex1 = (abs(CurrPlayerIndex1 + 1)) % gdata->team2->nplayers;
					CurrPlayer02 = Team02Players[CurrPlayerIndex1];
					if ( Paused ) glutPostRedisplay();
				break;

				case '2':
					gdata->graphics.use_light = !gdata->graphics.use_light;
					if ( Paused ) glutPostRedisplay();
				break;

				case '3':
					useMaterial = !useMaterial;
					if ( Paused ) glutPostRedisplay();
				break;

				case '4':
					gdata->graphics.draw_light = !gdata->graphics.draw_light;
					if ( Paused ) glutPostRedisplay();
				break;

				case '5':
					gdata->graphics.useFog = !gdata->graphics.useFog;
					if ( Paused ) glutPostRedisplay();
				break;

				case 'm':
				case 'M':
					ApperMap = !ApperMap;
				break;

				case '6':
					motionblur = !motionblur;
				break;

				case '7':
					if( useShader ) {
						gdata->graphics.useshader = !gdata->graphics.useshader;
					}
				break;

			}
		}

		virtual void glutMouse( int button, int state, int x, int y)
		{
// 			double X = 2 * x / width - 1;
// 			double Y = 1 - 2 * y / height;

  			if ( button == GLUT_LEFT_BUTTON ){
				if ( state == GLUT_UP )
					CamRotate = !CamRotate;
  			}
//	if ( button == GLUT_RIGHT_BUTTON ){
// 				Camera01.walk(10);
// 				Camera03.walk(10);
// 			}

		}

		virtual void glutMotion( int x, int y)
		{
 			//double X = 2 * x / width - 1;
 			//double Y = 1 - 2 * y / height;
		}

		virtual void glutPassiveMotion( int x, int y)
		{

			if( CurrCamera == &Camera04 && CamRotate ) {
				Camera04.Tilt( - 0.5 * (y-OldCamY) );
				Camera04.Pan( - 0.5 * (x-OldCamX ));
			}

			OldCamX = x;
			OldCamY = y;

		}

		inline void DrawInterface(void)
		{
			double x = 0, y = 0;
			int w, h, w2, h2;

            w = glutGet(GLUT_WINDOW_WIDTH);
			h = glutGet(GLUT_WINDOW_HEIGHT);
			w2 = w / 2;
			h2 = h / 2;
			// Parte de desenho da interface
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluOrtho2D (-w2, w2, -h2, h2);

			//Modelagem
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			resetMaterial();

			if ( ApperMap && MapX > 0 ) {
				MapX-=2;
			} else if ( !ApperMap && MapX < 208 ) {
				MapX+=2;
			}

			if ( MapX < 208 ) {
				//Mapa 208 x 140
				glPushMatrix();
					//glTranslatef(192.0f,-285.0f,0);
                    glTranslatef(w2+MapX-104,-h2 + 90,0);

                    glPushMatrix();
                    glScalef(104,70,1.0);
                    glBindTexture(GL_TEXTURE_2D, field->texid);
                    glEnable(GL_TEXTURE_2D);
                    glBegin(GL_QUAD_STRIP);
                        glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
                        glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,-1.0f);
                        glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, 1.0f);
                        glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,-1.0f);
                    glEnd();
                    glDisable(GL_TEXTURE_2D);
                    glPopMatrix();

                    // 0,0 e o  meio do campo
					//glTranslatef(104.0f+MapX,70.0f,0);
					glScalef(104.0/60.0,70.0/45.0,0);
                    glColor3f(1.0, 1.0, 1.0);

					glPointSize(5.0f);
					glBegin(GL_POINTS);
						for( register int i = 0 ; i < gdata->team1->nplayers ; i++ ) {
							x = -Team01Players[i]->pos.z;
							y = -Team01Players[i]->pos.x;

							if ( Team01Players[i]->isSelected ) {
								glColor3f(1.0f,0.5,0.5);
							} else {
								glColor3f(1.0f,0.0,0.0);
							}

							glVertex2f( x, y );
						}

						for( register int i = 0 ; i < gdata->team2->nplayers ; i++ ) {

							x = -Team02Players[i]->pos.z;
							y = -Team02Players[i]->pos.x;

							if ( Team02Players[i]->isSelected ) {
								glColor3f(0.5f,0.5,0.5);
							} else {
								glColor3f(0.0f,0.0,0.0);
							}

							glVertex2f( x, y );
						}

						glColor3f(1.0f,1.0,0.0);

						x = -Ball.pos.z;
						y = -Ball.pos.x;

						glVertex2f( x, y );

					glEnd();
					glPointSize(1.0f);
				glPopMatrix();
			}
			//Barra de Informacoes
			//glColor4f(1.0f,1.0f,1.0f,0.7f);
			glColor4f(1.0f,1.0f,1.0f,1.0f);
			glPushMatrix();
				glTranslatef(0.0f,-h2 + 9.0,0);
				glScalef(w2,9.0,1.0f);
				glutRect();
			glPopMatrix();

			//glColor4f(0.0f,0.0f,0.0f,0.8f);
			glColor4f(0.0f,0.0f,0.0f,1.0f);

			// Dados dos Jogadores (TIME DIR)
			sprintf(output,"%d - %s", CurrPlayer01->num, CurrPlayer01->name.c_str());
			glRasterPos2f(-w2 + 5, -h2 + 4.5);
			puts2DBitmap(output, GLUT_BITMAP_9_BY_15);

			// Dados dos Jogadores (TIME ESQ)
			sprintf(output,"%d - %s", CurrPlayer02->num, CurrPlayer02->name.c_str());
			glRasterPos2f(w2 -strlen(output) * 15,-h2 + 4.5);
			puts2DBitmap(output, GLUT_BITMAP_9_BY_15);


			//Dados do Jogo
			//glColor4f(1.0f,1.0f,1.0f,0.2f);
			glColor4f(0.2f,0.2f,0.2f,1.0f);
			glPushMatrix();
				glTranslatef(w2-60,h2-25,0);
				glScalef(60,25,1.0f);
				glutRect();
			glPopMatrix();

			glColor4f(0.7f,0.7f,0.7f,1.0f);
			// Placar
			sprintf(output,"%.2d x %.2d", Team01->nGols, Team02->nGols);
			glRasterPos2f(w2 - 120 + 30,h2-16);
			puts2DBitmap(output, GLUT_BITMAP_9_BY_15);

			// Bandeira 1
			glRasterPos2f(w2 - 120 + 10,h2-20);
			glDrawPixels(team01->dimx, team01->dimy, GL_RGB, GL_UNSIGNED_BYTE, team01->data);

			// Bandeira 2
			glRasterPos2f(w2 -20,h2-20);
			glDrawPixels(team02->dimx, team02->dimy, GL_RGB, GL_UNSIGNED_BYTE, team02->data);

			// Tempo de Jogo
			sprintf(output,"%.2d:%.2d", (int)ClockMin, (int)ClockSec);
			glRasterPos2f(w2 - 120 + 30,h2-50 + 10 );
			puts2DBitmap(output, GLUT_BITMAP_9_BY_15);

			// Relogio
			glRasterPos2f(w2 - 120 + 10,h2 - 50 + 9);
			glDrawPixels(imclock->dimx, imclock->dimy, GL_RGB, GL_UNSIGNED_BYTE, imclock->data);

			// Técnica de Fade
			if ( fadeIn != 0 ) {
				glColor4f(0.0f,0.0f,0.0f,fadeIn / (double)fade_steps);
                glPushMatrix();
                    glScalef(w2,h2,1.0f);
                    glutRect();
                glPopMatrix();
			}

			if ( Paused ) {
				glColor4f(1.0f,1.0f,1.0f,0.5f);
				glPushMatrix();
					glScalef(60,15,1.0f);
					glutRect();
				glPopMatrix();

				glColor4f(1.0f,1.0f,0.0f,1.0f);
				glRasterPos2f(-26,-4.5f);
				puts2DBitmap(Language[LG_SC5_PAUSE_TXT], GLUT_BITMAP_9_BY_15);
			}

			if ( End ) {
				glColor4f(1.0f,1.0f,1.0f,0.2f);
				glPushMatrix();
					glTranslatef(0.0f,0.0f,0);
					glScalef(60,15,1.0f);
					glutRect();
				glPopMatrix();

				glColor4f(1.0f,1.0f,0.0f,1.0f);
				glRasterPos2f(-26,-4.5f);
				puts2DBitmap(Language[LG_SC5_END_TXT], GLUT_BITMAP_9_BY_15);
			}


		}

		void DrawScene( bool use_cam = true )
		{
			// Parte de desenho da cena 3D
			//

			//Camera
			if ( CurrCamera && use_cam ) {
				CurrCamera->display();
			} else {
				Camera03.display();
			}

			glClearColor(Back.R, Back.G, Back.B, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
 			glFrontFace(GL_CCW);
			glDisable(GL_COLOR_MATERIAL);
			glColorMaterial(GL_FRONT,GL_DIFFUSE);

			if( gdata->graphics.useFog )  {

				glFogi(GL_FOG_MODE, gdata->graphics.fog.fog_filter ); //GL_LINEAR, GL_EXP, GL_EXP2
				glFogfv(GL_FOG_COLOR, gdata->graphics.fog.color);
				glFogf(GL_FOG_DENSITY, gdata->graphics.fog.density);
				glFogf(GL_FOG_START, gdata->graphics.fog.start);
				glFogf(GL_FOG_END, gdata->graphics.fog.end);
				glEnable(GL_FOG);
			}

			if( gdata->graphics.use_light )
				glEnable(GL_LIGHTING);

			//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (const GLfloat*)gdata->graphics.ambient_light );

			for( register int i = 0; i < gdata->graphics.nlights ; i++ ) {
				glEnable(GL_LIGHT0 + i);
				Lights[i]->place(i);
			}

			// Base
			glPushMatrix();

			Field.Draw(draw_mode);
			Ball.Draw(draw_mode);

			if( useMaterial ) {
				glEnable(GL_COLOR_MATERIAL);
				resetMaterial();
			}
			for( register int i = 0 ; i < gdata->team1->nplayers ; i++ ) {

				if( Team01Players[i]->ncards < 2 ) {
					Team01Players[i]->isSelected = (CurrPlayer01 == Team01Players[i]);
					Team01Players[i]->Draw();
				}
			}

			if( useMaterial ) {
				glEnable(GL_COLOR_MATERIAL);
				resetMaterial();
			}
			for( register int i = 0 ; i < gdata->team2->nplayers ; i++ ) {

				if( Team02Players[i]->ncards < 2) {
					Team02Players[i]->isSelected = (CurrPlayer02 == Team02Players[i]);
					Team02Players[i]->Draw();
				}
 			}

			rightFlag->Draw();
			leftFlag->Draw();

			sky->Draw();

			placar->Draw();

			for( register int i = 0; i < gdata->graphics.nlights; i++ ) {
				if( gdata->graphics.draw_light ) {
					Lights[i]->draw();
				}
			}

			for( register int i = 0; i < gdata->graphics.nlights; i++ ) {
				glDisable(GL_LIGHT0 + i);
			}

			glDisable(GL_FOG);
			glDisable(GL_LIGHTING);
			glDisable(GL_COLOR_MATERIAL);
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			glPopMatrix();

			//DrawInterface();

		}

		virtual void glutDisplay(void)
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(60,fAspect,0.5,500);
			glViewport(0, 0, (GLint)width, (GLint)height); // Especifica a viewport

			if( gdata->graphics.useshader ) {
				renderShader();
			}

			if( !motionblur ) {
				DrawScene();
			} else {
				bool fst = true;
				for( register int i = 0 ; i < gdata->graphics.motionframes ; i++ ) {

					if( fst ) {
						glAccum(GL_LOAD,1.0);
						fst = false;
					}
					DrawScene();
					// Faz "fade out" na imagem acumulada at�agora
					glAccum(GL_MULT, gdata->graphics.motionfactor);
					// E acumula imagem corrente
					glAccum(GL_ACCUM, 1-gdata->graphics.motionfactor);

					glutIdle();
				}
				// Retorna resultado na tela
				glAccum(GL_RETURN, 1.0);
			}

			if( gdata->graphics.useshader ) {
				resetShader();
			}

			DrawInterface();

			// Executa os comandos OpenGL
			glutSwapBuffers();
			//
		}

		virtual void glutReshape( int w, int h )
		{
			// Para previnir uma divisão por zero
			if ( h == 0 ) h = 1;

			// Especifica o tamanho da viewport
			//glViewport(0, 0, w, h);

			// Calcula a correção de aspecto
			fAspect = (GLfloat)w/(GLfloat)h;

			width = w;
			height = h;
		}

		int check_key( int p, int key )
		{
		    if( p == 1 ) {
		        if( (key & JOY_SET) == 0 ) { // teclado
                    if( (key & SPECIAL_KEY_SET) != 0 ) { // especial
                        return (int)checkSpecialKey(key&SPECIAL_KEY_CLEAR);
                    } else {
                        return (int)checkKeyUpper(key&SPECIAL_KEY_CLEAR);
                    }
		        } else { // joy
                    if( (key & SPECIAL_KEY_SET) != 0 ) { // axis
                        return (int)getjoyAxisB(key&SPECIAL_KEY_CLEAR&JOY_CLEAR, gdata->axis);
                    } else {
                        return (int)checkjoyBtn(key&SPECIAL_KEY_CLEAR&JOY_CLEAR);
                    }
		        }
		    } else {
                if( (key & SPECIAL_KEY_SET) != 0 ) { // especial
                    return (int)checkSpecialKey(key&SPECIAL_KEY_CLEAR);
                } else {
                    return (int)checkKeyUpper(key&SPECIAL_KEY_CLEAR);
                }
		    }
		}


		void processInteraction(void)
		{

		    if( !gdata->usepooling && gdata->usejoystick) {
                glutForceJoystickFunc();
		    }

			int Modifiers = GetModifiers();

			int Extra = ( Modifiers & GLUT_ACTIVE_SHIFT ) ? 1000 : 500;
			int Extra2 = ( Modifiers & GLUT_ACTIVE_CTRL ) ? 1000 : 500;

            if ( !WaitForKick && check_key(1,gdata->p1.fwd)  ) {
				applyForcePlayer( CurrPlayer01, CurrPlayer01->front * (Extra));
			}

			if ( !WaitForKick && check_key(1,gdata->p1.bwd) ) {
      			applyForcePlayer( CurrPlayer01, CurrPlayer01->front * (- Extra));
			}

			if ( check_key(1,gdata->p1.left) ) {
				CurrPlayer01->rotateY(1);
			}

			if ( check_key(1,gdata->p1.right) ) {
				CurrPlayer01->rotateY(-1);
			}

			if ( check_key(1,gdata->p1.kickY) ) {
				playerClickHL(CurrPlayer01,&Ball,100);
				if( Team01Ball ) { WaitForKick = false; ruleState = esNone; }
			}

			if ( check_key(1,gdata->p1.kickXZ) ) {
				playerClickHLA(CurrPlayer01,&Ball,10);
				if( Team01Ball ) { WaitForKick = false; ruleState = esNone; }
			}


			// Player 2

			if ( nPlayers == 2 ) {

                if ( !WaitForKick && check_key(2,gdata->p2.fwd)  ) {
                    applyForcePlayer( CurrPlayer02, CurrPlayer02->front * (Extra2));
                }

                if ( !WaitForKick && check_key(2,gdata->p2.bwd) ) {
                    applyForcePlayer( CurrPlayer02, CurrPlayer02->front * (- Extra2));
                }

                if ( check_key(2,gdata->p2.left) ) {
                    CurrPlayer02->rotateY(1);
                }

                if ( check_key(2,gdata->p2.right) ) {
                    CurrPlayer02->rotateY(-1);
                }

                if ( check_key(2,gdata->p2.kickY) ) {
                    playerClickHL(CurrPlayer02,&Ball,100);
                    if( !Team01Ball ) { WaitForKick = false; ruleState = esNone; }
                }

                if ( check_key(2,gdata->p2.kickXZ) ) {
                    playerClickHLA(CurrPlayer02,&Ball,10);
                    if( !Team01Ball ) { WaitForKick = false; ruleState = esNone; }
                }
			}


			if ( checkKey('a') || checkKey('A') ) {
				if( CurrCamera == &Camera04 )
					Camera04.walk(1);
			}

			if ( checkKey('z') || checkKey('Z') ) {
				if( CurrCamera == &Camera04 )
					Camera04.walk(-1);
			}

			if ( checkSpecialKey(GLUT_KEY_LEFT) ) {
				if( CurrCamera == &Camera04 )
					Camera04.Pan(1);
			}

			if ( checkSpecialKey(GLUT_KEY_RIGHT) ) {
				if( CurrCamera == &Camera04 )
					Camera04.Pan(-1);
			}

			if ( checkSpecialKey(GLUT_KEY_UP) ) {
				if( CurrCamera == &Camera04 )
					Camera04.Tilt(0.5);
			}

			if ( checkSpecialKey(GLUT_KEY_DOWN) ) {
				if( CurrCamera == &Camera04 )
					Camera04.Tilt(-0.5);
			}

			if ( checkKey('+') ) {
				if( CurrCamera == &Camera04 )
					Camera04.Roll(1);
			}

			if ( checkKey('-') ) {
				if( CurrCamera == &Camera04 )
					Camera04.Roll(-1);
			}
		}

		void luaUpdateEnemyPos( lua_State *L, CPlayer *p )
		{
		    if( !L ) return;

			lua_getglobal(L,"update_enemy_pos");

			if( !lua_isfunction(L,-1) ){
				fprintf(stderr,"[ERRO] %s\n", Language[LG_LUA_ERROR_FCN]);
				return;
			}

			lua_pushinteger(L, p->num);
			lua_pushnumber(L, p->pos.x);
			lua_pushnumber(L, p->pos.z);

			// 3 Argumentos, 0 resultados
			lua_call(L, 3, 0);
		}

		virtual void glutIdle(void)
		{

			if ( !Paused && !End ) {

				double oldZpos = Ball.pos.z;
				double oldXpos = Ball.pos.x;

				int Falta = 0, fPlayer0 = 0, fPlayer1 = 0, fstFalta = 0;
				bool Test = true;

				CVector3D cameraVel = CurrCamera->getPos();

				start = clock();

				elapsed = ((double) (start - end )) / CLOCKS_PER_SEC;

				if( !WaitForKick ) {
					ClockSec = ClockSec + elapsed * 9.0 ;

					if ( ClockSec > 60 ) {
						ClockMin++;
						ClockSec = 0;
					}

					if( Team01Ball ) {
						Team01->Posse += elapsed / 6.0;
					} else {
						Team02->Posse += elapsed / 6.0;
					}
				}

				end = start;

				elapsed = fabs(elapsed);

				// Animação
				for( register int i = 0 ; i < gdata->team1->nplayers ; i++ ) {
					Team01Players[i]->animate(elapsed);
				}

				for( register int i = 0 ; i < gdata->team2->nplayers ; i++ ) {
					Team02Players[i]->animate(elapsed);
				}
				rightFlag->animate(elapsed);
				leftFlag->animate(elapsed);



				// Interação com o usuário
				processInteraction();

				if( CurrPlayer02->isCPU && WaitForKick && !Team01Ball ) {
					CurrPlayer02->runIA( Team01Ball, WaitForKick, &Field, &Ball, ClockMin >= 45 );
					playerClickHL(CurrPlayer02,&Ball,140);
					CurrPlayer02->state = eWaiting;
					WaitForKick = false;
					ruleState = esNone;
				}

				// Simulação
				if( !WaitForKick ) {

				for( register int i = 0 ; i < gdata->team1->nplayers ; i++ ) {

					applyPhysics4Player(Team01Players[i], &Field, elapsed);

					testPhysicsCollision4Ball(Team01Players[i],&Ball, &Team01Ball);

					for( register int j = 0 ; j < gdata->team1->nplayers ; j++ ) {

						if( j == i ) continue;

						Falta = testPhysicsCollision4Player( Team01Players[i],
										     Team01Players[j], &Field,
										     elapsed, Team01Ball);

						if( Test && Falta != 0 ) {
							fPlayer0 = i;
							fPlayer1 = j;
							Test = false;
							fstFalta = Falta;
						}

					}

					for( register int j = 0 ; j < gdata->team2->nplayers ; j++ ) {

						Falta = testPhysicsCollision4Player( Team01Players[i],
										     Team02Players[j], &Field,
										     elapsed, Team01Ball);

						if( Test && Falta != 0 ){
							fPlayer0 = i;
							fPlayer1 = j;
							Test = false;
							fstFalta = Falta;
						}
					}
 				}

				for( register int i = 0 ; i < gdata->team2->nplayers ; i++ ) {

					applyPhysics4Player(Team02Players[i], &Field, elapsed);

					testPhysicsCollision4Ball(Team02Players[i],&Ball, &Team01Ball);

					for( register int j = 0 ; j < gdata->team2->nplayers ; j++ ) {

						if( j == i ) continue;

						Falta = testPhysicsCollision4Player( Team02Players[i],
										     Team02Players[j], &Field,
										     elapsed, Team01Ball);

						if( Test && Falta != 0 ){
							fPlayer0 = i;
							fPlayer1 = j;
							Test = false;
							fstFalta = Falta;
						}
					}

					for( register int j = 0 ; j < gdata->team1->nplayers ; j++ ) {

						Falta = testPhysicsCollision4Player( Team02Players[i],
										     Team01Players[j], &Field,
										     elapsed, Team01Ball);
						if( Test && Falta != 0 ){
							fPlayer0 = i;
							fPlayer1 = j;
							Test = false;
							fstFalta = Falta;
						}
					}
 				}

				updatePhysicsWind();
				applyPhysics4Ball(&Ball,elapsed);

				applyPhysics4FlagMan(rightFlag, &Field, elapsed);
				applyPhysics4FlagMan(leftFlag, &Field, elapsed);

				for( register int i = 0 ; i < gdata->team1->nplayers ; i++ ) {
                    luaUpdateEnemyPos(lua_team2, Team01Players[i]);
				}

				for( register int i = 0 ; i < gdata->team2->nplayers ; i++ ) {
                    luaUpdateEnemyPos(lua_team1, Team02Players[i]);
				}

				// IA
				for( register int i = 0 ; i < gdata->team1->nplayers ; i++ ) {

					// Time 1 na positiva quando ClockMin > 45
					Team01Players[i]->runIA( !Team01Ball, !WaitForKick, &Field, &Ball, ClockMin >= 45 );

					if( Team01Players[i]->state == eDone ) {
						if( (Team01Players[i]->pos - CurrPlayer01->pos).abs() < 20.0 ||
						    ( Team01Ball && LastCollisionIndex >= 0  &&
						    ( Team01Players[i] == Team01Players[LastCollisionIndex])) ) {
							CurrPlayer01 = Team01Players[i];
						}
						Team01Players[i]->state = eWaiting;
					}
				}

				for( register int i = 0 ; i < gdata->team2->nplayers ; i++ ) {

					Team02Players[i]->runIA( Team01Ball, !WaitForKick, &Field, &Ball, ClockMin >= 45 );

					if( Team02Players[i]->state == eDone ) {

						if( (Team02Players[i]->pos - CurrPlayer02->pos).abs() < 20.0 ||
						    ( !Team01Ball && LastCollisionIndex >= 0  &&
						    ( Team02Players[i] == Team02Players[LastCollisionIndex])) ) {
							CurrPlayer02 = Team02Players[i];
						}

						Team02Players[i]->state = eWaiting;

					}/* else if ( Team02Players[i]->state == eKick ) {

						playerClickHL(Team02Players[i],&Ball,140);
						Team02Players[i]->state = eRunning;
					}*/
				}

				rightFlag->runIA(  !WaitForKick, &Ball, ClockMin >= 45 );
				leftFlag->runIA(  !WaitForKick, &Ball, ClockMin >= 45 );

				// Regras do Jogo
				CVector3D playerPos;
				gameStatus = applySoccerRules4Ball(&Ball, &Field, Team01Ball, ClockMin >= 45, playerPos,
								   CurrPlayer01->r);
				Falta = fstFalta;
				//fprintf(stderr,"%.2f %.2f\n", CurrPlayer01->pos.x, CurrPlayer01->pos.z);

				if( Falta != 0 ) {

					gameStatus = applySoccerRules4Faults( &Ball, Team01Ball, ClockMin >= 45, playerPos,
							Team01Players[fPlayer0], Team02Players[fPlayer1] );

					if( Team01Ball ) {
						#ifdef USE_SOUND
						setSourcePos(RECEBE_SOCO,
							Team01Players[fPlayer0]->pos.x,
							Team01Players[fPlayer0]->pos.y,
							Team01Players[fPlayer0]->pos.z);

						setSourceVel(RECEBE_SOCO,
							Team01Players[fPlayer0]->vel.x,
							Team01Players[fPlayer0]->vel.y,
							Team01Players[fPlayer0]->vel.z);

						playSound(RECEBE_SOCO);

						setSourcePos(COLISAO_SOCO,
							Team02Players[fPlayer1]->pos.x,
							Team02Players[fPlayer1]->pos.y,
							Team02Players[fPlayer1]->pos.z);

						setSourceVel(COLISAO_SOCO,
							Team02Players[fPlayer1]->vel.x,
							Team02Players[fPlayer1]->vel.y,
							Team02Players[fPlayer1]->vel.z);

						playSound(COLISAO_SOCO);
						#endif

						if( Falta != 3 ) {
							Team02Players[fPlayer1]->ncards += Falta;

		 					Team02->Cartoes[Team02->nCartoes] = new char[100];

		 					sprintf(Team02->Cartoes[Team02->nCartoes],
								"%.2d:%.2d | %d - %s", (int)ClockMin, (int)ClockSec,
								Team02Players[fPlayer1]->num,
								Team02Players[fPlayer1]->name.c_str());

							Team02->Cartoes[Team02->nCartoes][CARD_TYPE] = (Falta == 1 ) ? 'a' : 'v';

							Team02->nCartoes++;

							if( Falta == 1 && Team02Players[fPlayer1]->ncards == 2 ) {

								Team02->Cartoes[Team02->nCartoes] = new char[100];
								sprintf(Team02->Cartoes[Team02->nCartoes],
									"%.2d:%.2d | %d - %s", (int)ClockMin, (int)ClockSec,
									Team02Players[fPlayer1]->num,
									Team02Players[fPlayer1]->name.c_str());

								Team02->Cartoes[Team02->nCartoes][CARD_TYPE] = 'v';
								Team02->nCartoes++;
							}

						}

						CurrPlayer01 = Team01Players[fPlayer0];

					} else {

						#ifdef USE_SOUND
						setSourcePos(RECEBE_SOCO,
							Team02Players[fPlayer1]->pos.x,
							Team02Players[fPlayer1]->pos.y,
							Team02Players[fPlayer1]->pos.z);

						setSourceVel(RECEBE_SOCO,
							Team02Players[fPlayer1]->vel.x,
							Team02Players[fPlayer1]->vel.y,
							Team02Players[fPlayer1]->vel.z);

						playSound(RECEBE_SOCO);

						setSourcePos(COLISAO_SOCO,
							Team01Players[fPlayer0]->pos.x,
							Team01Players[fPlayer0]->pos.y,
							Team01Players[fPlayer0]->pos.z);

						setSourceVel(COLISAO_SOCO,
							Team01Players[fPlayer0]->vel.x,
							Team01Players[fPlayer0]->vel.y,
							Team01Players[fPlayer0]->vel.z);

						playSound(COLISAO_SOCO);
						#endif

						Team01Players[fPlayer0]->ncards += ( Falta != 3 ) ? Falta : 0;

						if( Falta != 3 ) {
							Team01Players[fPlayer0]->ncards += Falta;

		 					Team01->Cartoes[Team01->nCartoes] = new char[100];

		 					sprintf(Team01->Cartoes[Team01->nCartoes],
								"%.2d:%.2d | %d - %s", (int)ClockMin, (int)ClockSec,
								Team01Players[fPlayer0]->num,
								Team01Players[fPlayer0]->name.c_str());

							Team01->Cartoes[Team01->nCartoes][CARD_TYPE] = (Falta == 1 ) ? 'a' : 'v';

							Team01->nCartoes++;

							if( Falta == 1 && Team01Players[fPlayer0]->ncards == 2 ) {

								Team01->Cartoes[Team01->nCartoes] = new char[100];
								sprintf(Team01->Cartoes[Team01->nCartoes],
									"%.2d:%.2d | %d - %s", (int)ClockMin, (int)ClockSec,
									Team01Players[fPlayer0]->num,
									Team01Players[fPlayer0]->name.c_str());

								Team01->Cartoes[Team01->nCartoes][CARD_TYPE] = 'v';
								Team01->nCartoes++;
							}
						}

						CurrPlayer02 = Team02Players[fPlayer1];
					}

				 	if ( Falta == 1 || Falta == 2 ) {
						#ifdef USE_SOUND
						playSound(FALTA);
						#endif
					}
				}

				switch( gameStatus )  {


					case eLateral:

						if( !Team01Ball ) {

							Team01->nLaterais++;

							CurrPlayer01 =
								nearestPlayer(Team01Players,gdata->team1->nplayers,playerPos);
							CurrPlayer01->pos.x = playerPos.x;
							CurrPlayer01->pos.z = playerPos.z;

						} else {
							CurrPlayer02 =
								nearestPlayer(Team02Players,gdata->team2->nplayers,playerPos);
							CurrPlayer02->pos.x = playerPos.x;
							CurrPlayer02->pos.z = playerPos.z;
							Team02->nLaterais++;
						}

						stopPlayers();
						Team01Ball = !Team01Ball;
						WaitForKick = true;

					break;

					case eEscanteio:

						if( !Team01Ball ) {

							Team01->nEscanteios++;

							CurrPlayer01 = nearestPlayer(Team01Players,gdata->team1->nplayers,playerPos);
							CurrPlayer01->pos.x = playerPos.x;
							CurrPlayer01->pos.z = playerPos.z;

							Team01Ball = true;

						} else {
							CurrPlayer02 = nearestPlayer(Team02Players,gdata->team2->nplayers,playerPos);
							CurrPlayer02->pos.x = playerPos.x;
							CurrPlayer02->pos.z = playerPos.z;

							Team02->nEscanteios++;

							Team01Ball = false;
						}
						stopPlayers();
						WaitForKick = true;
					break;

					case eGolPos:
						if( ClockMin <= 45 ) {
		 					Team01->Gols[Team01->nGols] = new char[150];

							if ( Team01Ball ) {
		 						sprintf(Team01->Gols[Team01->nGols],
									"%.2d:%.2d | %d - %s", (int)ClockMin, (int)ClockSec,
									Team01Players[LastCollisionIndex]->num,
									Team01Players[LastCollisionIndex]->name.c_str());
							} else {
		 						sprintf(Team01->Gols[Team01->nGols],
									"%.2d:%.2d | %d - %s (-)", (int)ClockMin, (int)ClockSec,
									Team02Players[LastCollisionIndex]->num,
									Team02Players[LastCollisionIndex]->name.c_str());
							}

							Team01->nGols++;
							placar->gols1++;

							Team01Ball = false;
						} else {
		 					Team02->Gols[Team02->nGols] = new char[150];

							if ( Team01Ball ) {
		 						sprintf(Team02->Gols[Team02->nGols],
									"%.2d:%.2d | %d - %s (-)", (int)ClockMin, (int)ClockSec,
									Team01Players[LastCollisionIndex]->num,
									Team01Players[LastCollisionIndex]->name.c_str());
							} else {
		 						sprintf(Team02->Gols[Team02->nGols],
									"%.2d:%.2d | %d - %s", (int)ClockMin, (int)ClockSec,
									Team02Players[LastCollisionIndex]->num,
									Team02Players[LastCollisionIndex]->name.c_str());
							}

							Team02->nGols++;
							placar->gols2++;

							Team01Ball = true;

						}
						stopPlayers();
						resetPlayers(Team01Ball);

						WaitForKick = true;
					break;

					case eGolNeg:

						if( ClockMin > 45 ) {
		 					Team01->Gols[Team01->nGols] = new char[150];

							if ( Team01Ball ) {
		 						sprintf(Team01->Gols[Team01->nGols],
									"%.2d:%.2d | %d - %s", (int)ClockMin, (int)ClockSec,
									Team01Players[LastCollisionIndex]->num,
									Team01Players[LastCollisionIndex]->name.c_str());
							} else {
		 						sprintf(Team01->Gols[Team01->nGols],
									"%.2d:%.2d | %d - %s (-)", (int)ClockMin, (int)ClockSec,
									Team02Players[LastCollisionIndex]->num,
									Team02Players[LastCollisionIndex]->name.c_str());
							}

							Team01->nGols++;
							placar->gols1++;
							Team01Ball = false;
						} else {
		 					Team02->Gols[Team02->nGols] = new char[150];

							if ( Team01Ball ) {
		 						sprintf(Team02->Gols[Team02->nGols],
									"%.2d:%.2d | %d - %s (-)", (int)ClockMin, (int)ClockSec,
									Team01Players[LastCollisionIndex]->num,
									Team01Players[LastCollisionIndex]->name.c_str());
							} else {
		 						sprintf(Team02->Gols[Team02->nGols],
									"%.2d:%.2d | %d - %s", (int)ClockMin, (int)ClockSec,
									Team02Players[LastCollisionIndex]->num,
									Team02Players[LastCollisionIndex]->name.c_str());
							}

							Team02->nGols++;
							placar->gols2++;
							Team01Ball = true;
						}
						stopPlayers();
						resetPlayers(Team01Ball);
						WaitForKick = true;
					break;

					case eFalta:

						if( Team01Ball ) {

							CurrPlayer01->pos.x = playerPos.x;
							CurrPlayer01->pos.z = playerPos.z;

							Team02->nFaltas++;

						} else {

							Team01->nFaltas++;

							CurrPlayer02->pos.x = playerPos.x;
							CurrPlayer02->pos.z = playerPos.z;
						}

						stopPlayers();
						Team01Ball = !Team01Ball;
						WaitForKick = true;
					break;

					case ePenalty:

						if( Team01Ball ) {

							CurrPlayer01->pos.x = playerPos.x;
							CurrPlayer01->pos.z = playerPos.z;

							Team02->nFaltas++;

						} else {

							Team01->nFaltas++;

							CurrPlayer02->pos.x = playerPos.x;
							CurrPlayer02->pos.z = playerPos.z;
						}

						stopPlayers();
						Team01Ball = !Team01Ball;
						WaitForKick = true;
					break;

					default:
					break;
				}
}

				// Jogadores atuais
				if( autoSeek ) {
					CurrPlayer01 = Team01Players[setNewCurrPlayer( Team01Players, gdata->team1->nplayers, &Ball )];
				}

				if( autoSeek || nPlayers == 1 ) {
					CurrPlayer02 = Team02Players[setNewCurrPlayer( Team02Players, gdata->team2->nplayers, &Ball )];
				}

				if( ClockMin == 45 && ClockSec == 0)
				{
					Paused = true;
					resetPlayers(!Team01Ball);
					stopPlayers();
					ClockSec++;
				}

				if( ClockMin == 90 && ClockSec == 0 )
				{
					End = true;
					ClockSec++;
				}

				// Atualização das Cameras
				Camera02.setPos(Ball.pos.x,Ball.pos.y,Ball.pos.z);
				Camera02.lookTarget(1.1 * Ball.pos.x,1.1 * Ball.pos.y,1.1 * Ball.pos.z); // Bola
				Camera02.setPan(TO_DEGREES(Ball.rot.z));
				Camera02.setTilt(TO_DEGREES(Ball.rot.y));
				Camera02.setRoll(TO_DEGREES(Ball.rot.x) + 90);

				Camera05.setPos(CurrPlayer01->pos.x,CurrPlayer01->headY,CurrPlayer01->pos.z);

				Camera05.setTarget(
						CurrPlayer01->pos.x + CurrPlayer01->front.x,
						CurrPlayer01->headY,
						CurrPlayer01->pos.z + CurrPlayer01->front.z);

				Camera06.setPos(
						CurrPlayer01->pos.x - 10 * CurrPlayer01->front.x,
						CurrPlayer01->headY + 5,
						CurrPlayer01->pos.z - 10 * CurrPlayer01->front.z);

				Camera06.setTarget(
						CurrPlayer01->pos.x + 10 * CurrPlayer01->front.x,
						CurrPlayer01->headY,
						CurrPlayer01->pos.z + 10 * CurrPlayer01->front.z);

				Camera01.incZ(Ball.pos.z-oldZpos);
				Camera01.incX(Ball.pos.x-oldXpos);

				Camera03.incZ(Ball.pos.z-oldZpos);
				Camera03.incX(Ball.pos.x-oldXpos);

				// Calculo de velocidade da Camera e Atualização de Som
				cameraVel = (CurrCamera->getPos() - cameraVel) / elapsed;

				updateListener(cameraVel);

				glutPostRedisplay();
			}

			if( ClockMin == 110 )
			{
				End = true;
				struct pass_info *pass = new struct pass_info;

				pass->team01 = Team01;
				pass->team02 = Team02;
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluOrtho2D (-size2, size2, -size2,size2);
				ChangeScene(4, (void*)pass);
			}
		}

		void updateListener( CVector3D &vel )
		{
			CVector3D pos = CurrCamera->getPos();
			CVector3D target = CurrCamera->getTarget();

			target = pos - target;
			#ifdef USE_SOUND
			setListenerPos(pos.x,pos.y,pos.z);
			setListenerPos(vel.x,vel.y,vel.z);
			setListenerOri(target.x,target.y,target.z);
			#endif

		}

		void stopPlayers(void) // Modificar
		{
			CVector3D zero = CVector3D(0,0,0);

			for( register int i = 0 ; i < gdata->team1->nplayers ; i++ ) {
				Team01Players[i]->vel = zero;
			}

			for( register int i = 0 ; i < gdata->team2->nplayers ; i++ ) {
				Team02Players[i]->force = zero;
			}
		}

		void resetPlayers( bool team01 = true) // Modificar
		{

			CVector3D zero = CVector3D(0,0,0);
			CVector3D front = CVector3D(0,0,1);

			int initCnt = -1;

			for( register int i = 0 ; i < gdata->team1->nplayers ; i++ ) {

				Team01Players[i]->vel = zero;
				Team01Players[i]->force = zero;
				Team01Players[i]->rot = 0;

				Team01Players[i]->front = front;

				if( ClockMin >= 45 ) {
					Team01Players[i]->rotateY(180);
					Team01Players[i]->pos.x = - (gdata->team1->players[i].p0x);
					Team01Players[i]->pos.z = fabs(gdata->team1->players[i].p0z);
					Team01Players[i]->pf.x = -gdata->team1->players[i].pfx;
					Team01Players[i]->pf.z = -gdata->team1->players[i].pfz;
				} else {
					Team01Players[i]->pos.x = (gdata->team1->players[i].p0x);
					Team01Players[i]->pos.z = -fabs(gdata->team1->players[i].p0z);
					Team01Players[i]->pf.x =  gdata->team1->players[i].pfx;
					Team01Players[i]->pf.z =  gdata->team1->players[i].pfz;
				}

				Team01Players[i]->p0 =  Team01Players[i]->pos;

				if ( initCnt < 2 && team01 && Team01Players[i]->type == "ataque" ) {
					Team01Players[i]->pos.z = 0;
					Team01Players[i]->pos.x = initCnt * (Ball.r + Team01Players[i]->r + 1.0);
					initCnt += 2;
				}
 			}

			for( register int i = 0 ; i < gdata->team2->nplayers ; i++ ) {

				Team02Players[i]->vel = zero;
				Team02Players[i]->force = zero;
				Team02Players[i]->rot = 0;

				Team02Players[i]->front = front;

				if( ClockMin < 45 ) {
					Team02Players[i]->rotateY(180);
					Team02Players[i]->pos.x =(gdata->team2->players[i].p0x);
					Team02Players[i]->pos.z = fabs(gdata->team2->players[i].p0z);
					Team02Players[i]->pf.x =  gdata->team2->players[i].pfx;
					Team02Players[i]->pf.z =  gdata->team2->players[i].pfz;
				} else {
					Team02Players[i]->pos.x = - (gdata->team2->players[i].p0x);
					Team02Players[i]->pos.z = - fabs(gdata->team2->players[i].p0z);
					Team02Players[i]->pf.x =  -gdata->team2->players[i].pfx;
					Team02Players[i]->pf.z =  -gdata->team2->players[i].pfz;
				}

				Team02Players[i]->p0 =  Team02Players[i]->pos;

				if ( initCnt < 2 && !team01 && Team02Players[i]->type == "ataque" ) {
					Team02Players[i]->pos.z = 0;
					Team02Players[i]->pos.x = initCnt * (Ball.r + Team02Players[i]->r + 1.0);
					initCnt += 2;
				}
 			}

			if(  initCnt < 2 )
				if( team01 ) {
					Team01Players[gdata->team1->nplayers-1]->pos.z = 0;
					Team01Players[gdata->team1->nplayers-1]->pos.x = -(Ball.r +
										Team01Players[gdata->team1->nplayers-1]->r
										 + 1.0);

					Team01Players[gdata->team1->nplayers-2]->pos.z = 0;
					Team01Players[gdata->team1->nplayers-2]->pos.x = (Ball.r +
									       Team01Players[gdata->team1->nplayers-2]->r
										+ 1.0);
				} else {
					Team02Players[gdata->team2->nplayers-1]->pos.z = 0;
					Team02Players[gdata->team2->nplayers-1]->pos.x = -(Ball.r +
										Team02Players[gdata->team2->nplayers-1]->r
										+ 1.0);

					Team02Players[gdata->team2->nplayers-2]->pos.z = 0;
					Team02Players[gdata->team2->nplayers-2]->pos.x = (Ball.r +
										Team02Players[gdata->team2->nplayers-2]->r
										+ 1.0);
				}
			#ifdef USE_SOUND
			playSound(APITO);
			#endif

			Ball.vel = zero;

			rightFlag->pos.z = 0;
			leftFlag->pos.z = 0;

		}

		CScene05( int nPlayers, int fs ) : CScene(fs), Back(0,0,0,0), Camera01(40,60,0,30), Camera02(0,20,40,20), Camera03(0,80,0,30), Camera05( 0,0,0, 20), Camera06( 0,0,0, 20), Camera04(0,13,20,10)
		{
			size2 = 1.0f;

			MapX = 208;
			ApperMap = false;

			width = SCR_WIDTH;
			height = SCR_HEIGHT;
			this->nPlayers = nPlayers;

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			//team01 = LoadJPG("textures/team01.jpg");
			team01 = LoadJPG(gdata->team1->imbrasao.c_str());
			//team02 = LoadJPG("textures/team02.jpg");
			team02 = LoadJPG(gdata->team2->imbrasao.c_str());

			imclock =  LoadJPG("textures/clock.jpg");
			//field =  LoadJPG("textures/field.jpg");
			field =  LoadTexture("textures/field.jpg",true);

			Camera01.lookTarget(0,0,0); // Lateral
			Camera03.lookTarget(0,0,0); // Superior
			Camera05.setPan(0);

			rightFlag = new CFlagMan(gdata->lateral1.c_str());
			rightFlag->pos.x = 50;

			leftFlag = new CFlagMan(gdata->lateral2.c_str());
			leftFlag->pos.x = -50;

			CurrCamera = &Camera01;
			LastCollisionIndex = -1;
			CamRotate = false;

			WaitForKick = false;
			autoSeek = true;
			Ball.zero();
			Ball.pos.y = Ball.r;

			Paused = false;

			Lights = new CLight*[gdata->graphics.nlights];

			CSpotLight *ll;
			CLight *l;

			nLights = gdata->graphics.nlights;

			for( register int i = 0 ; i < gdata->graphics.nlights ; i++ ) {

				if( gdata->graphics.lights[i].type == "spot" ) {
					ll = new CSpotLight(i);

					ll->setDirection( gdata->graphics.lights[i].direction[0],
							  gdata->graphics.lights[i].direction[1],
							  gdata->graphics.lights[i].direction[2]);

					ll->setCutOff(gdata->graphics.lights[i].cutoff);
					ll->setExponent(gdata->graphics.lights[i].exponent);

					Lights[i] = ll;
					l = ll;
				} else {
					l = new CLight(i);
					Lights[i] = l;
				}

				l->setAmbient( gdata->graphics.lights[i].ambient[0],
						gdata->graphics.lights[i].ambient[1],
						gdata->graphics.lights[i].ambient[2],
						gdata->graphics.lights[i].ambient[3]);


				l->setDiffuse( gdata->graphics.lights[i].diffuse[0],
						gdata->graphics.lights[i].diffuse[1],
						gdata->graphics.lights[i].diffuse[2],
						gdata->graphics.lights[i].diffuse[3]);

				l->setSpecular( gdata->graphics.lights[i].specular[0],
						 gdata->graphics.lights[i].specular[1],
						 gdata->graphics.lights[i].specular[2],
						 gdata->graphics.lights[i].specular[3]);

				l->setPosition( gdata->graphics.lights[i].position[0],
						 gdata->graphics.lights[i].position[1],
						 gdata->graphics.lights[i].position[2]);


				l->setConstAtenuation( gdata->graphics.lights[i].atenuation[0] );
				l->setLinearAtenuation( gdata->graphics.lights[i].atenuation[1] );
				l->setQuadAtenuation( gdata->graphics.lights[i].atenuation[2] );

			}

			Team01Players = new CPlayer*[gdata->team1->nplayers];

			for( register int i = 0 ; i < gdata->team1->nplayers ; i++ ) {
				//Team01Players[i] = new CPlayer(names1[i],i+1,0);
				Team01Players[i] = new CPlayer( gdata->team1->players[i].name.c_str(),
								gdata->team1->players[i].number,
								gdata->team1->imcamisa.c_str(),
								gdata->team1->players[i].type.c_str(),0, lua_team1,
								gdata->team1->stupid);

				Team01Players[i]->colorBody = Color(0.8,0.3,0.3,1.0);
 			}

			Team02Players = new CPlayer*[gdata->team2->nplayers];

			for( register int i = 0 ; i < gdata->team2->nplayers ; i++ ) {
					//Team02Players[i] = new CPlayer(names2[i],i+1,1);
				Team02Players[i] = new CPlayer( gdata->team2->players[i].name.c_str(),
								gdata->team2->players[i].number,
								gdata->team2->imcamisa.c_str(),
								gdata->team2->players[i].type.c_str(),1, lua_team2,
								gdata->team2->stupid);
				Team02Players[i]->colorBody = Color(0.2,0.2,0.2,1.0);
				Team02Players[i]->isCPU = ( nPlayers < 2 );
 			}

			sky = new CSky(CVector3D(550,250,550),gdata->graphics.usecubemap);
			sky->pos.y = 250 / 2 - 1;

			placar = new CPlacar();

			placar->pos = CVector3D(50,0,65);
			placar->angle = -135;

			placar->team1 = gdata->team1->sigla;
			placar->team2 = gdata->team2->sigla;

			WaitForKick = true;
			Team01Ball = true;
			resetPlayers(Team01Ball);

			CurrPlayerIndex0 = 0;
			CurrPlayer01 = Team01Players[CurrPlayerIndex0];

			CurrPlayerIndex1 = 0;
			CurrPlayer02 = Team02Players[CurrPlayerIndex1];

			start = end = clock();

			ClockMin = 0;
			ClockSec = 0;

			End = false;

			Team01 = createInfo();
			Team02 = createInfo();

			ruleState = esNone;

			motionblur = gdata->graphics.enablemotion;
			useMaterial = true;
			useLight = gdata->graphics.use_light;
		}

		~CScene05()
		{
			deleteJPG(team01);
			deleteJPG(team02);
			deleteJPG(imclock);

			for( register int i = 0; i < nLights; i++ ) {
				delete Lights[i];
			}

			delete [] Lights;

			for( register int i = 0 ; i < gdata->team1->nplayers ; i++ ) {
				delete Team01Players[i];
			}

			for( register int i = 0 ; i < gdata->team2->nplayers ; i++ ) {
				delete Team02Players[i];
 			}

			delete [] Team01Players;

			delete [] Team02Players;

			delete rightFlag;
			delete leftFlag;
			delete sky;
			delete placar;
		}

		void setWH( int w, int h)
		{
			width = w;
			height = h;
			fAspect = (GLfloat)w/(GLfloat)h;
		}

		void getWH( int &w, int &h)
		{
			w = (int)width;
			h = (int)height;
		}

		static void glutTimer( int value )
		{

		}

	};
};

#endif
