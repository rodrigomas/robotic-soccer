#ifndef BUTTON_H
#define BUTTON_H

#include "libsoccer.h"
#include "color.h"

#ifdef USE_SOUND
#include "sound.h"
#endif

#define RELEASE 1
#define PRESS 2

#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

using std::string;

namespace soccer {

	class CButton {

		enum bt_state {

			bts_normal,
			bts_over,
			bts_clicked

		} state;

		Color Back;

		string caption;

		double sizeX;
		double sizeY;
		double x;
		double y;

	public:

		CButton( double R, double G, double B, double X, double Y, double SizeX, double SizeY, const char *name) :
			 Back(R,G,B,0)
		{
			state = bts_normal;
			caption = name;
			sizeX = SizeX;
			sizeY = SizeY;
			x = X;
			y = Y;
			//OnPress = NULL;
			//OnRelease = NULL;
		}

		void Draw(void)
		{
			int width = glutGet(GLUT_WINDOW_WIDTH);

			double offsetx = (9 * caption.size() / 2.0) / (width / 2.0) ;
			double offsety = +0.01;//(15 / 2.0) / (SCR_HEIGHT / 2.0) ;

			switch( state ) {

				case bts_normal:

				glPushMatrix();
					glLoadIdentity();
					glTranslatef(x,y,0);
					glScalef(sizeX,sizeY,1.0f);
					glColor4f(Back.R,Back.G,Back.B,0.5f);
					glBegin(GL_TRIANGLE_STRIP);
						glVertex3f(-0.5f, 0.5f ,0);
						glVertex3f(-0.5f,-0.5f ,0);
						glVertex3f( 0.5f, 0.5f ,0);
						glVertex3f( 0.5f,-0.5f ,0);
					glEnd();
				glPopMatrix();

				glColor4f(1.0f,1.0f,1.0f,0.8f);
				glRasterPos2f(x - offsetx, y - offsety);
				puts2DBitmap(caption.c_str(), GLUT_BITMAP_9_BY_15);
				break;

				case bts_over:

				glPushMatrix();
					glLoadIdentity();
					glTranslatef(x,y,0);
					glScalef(sizeX,sizeY,1.0f);
					glColor4f(Back.R,Back.G,Back.B,0.3f);
					glBegin(GL_TRIANGLE_STRIP);
						glVertex3f(-0.5f, 0.5f ,0);
						glVertex3f(-0.5f,-0.5f ,0);
						glVertex3f( 0.5f, 0.5f ,0);
						glVertex3f( 0.5f,-0.5f ,0);
					glEnd();
				glPopMatrix();

				glColor4f(1.0f,1.0f,0.0f,0.8f);
				glRasterPos2f(x - offsetx, y -offsety );
				puts2DBitmap(caption.c_str(), GLUT_BITMAP_9_BY_15);
				break;

				case bts_clicked:

				glPushMatrix();
					glLoadIdentity();
					glTranslatef(x,y,0);
					glScalef(sizeX,sizeY,1.0f);
					glColor4f(Back.R,Back.G,Back.B,0.7f);
					glBegin(GL_TRIANGLE_STRIP);
						glVertex3f(-0.5f, 0.5f ,0);
						glVertex3f(-0.5f,-0.5f ,0);
						glVertex3f( 0.5f, 0.5f ,0);
						glVertex3f( 0.5f,-0.5f ,0);
					glEnd();
				glPopMatrix();

				glColor4f(1.0f,0.0f,0.0f,0.8f);
				glRasterPos2f(x - offsetx, y - offsety);
				puts2DBitmap(caption.c_str(), GLUT_BITMAP_9_BY_15);
				break;
			}
		}

		void Motion( double x, double y )
		{

			if ( state == bts_clicked )
				return;

			double X = this->x - sizeX / 2.0;
			double Y = this->y - sizeY / 2.0;

			if ( x > X && x < X + sizeX &&
			     y > Y && y < Y + sizeY ) {

				if( state != bts_over ) {
					state = bts_over;
					#ifdef USE_SOUND
					playSound(MOUSE_OVER);
					#endif
					glutPostRedisplay();
				}
			} else {
				if( state != bts_normal ) {
					state = bts_normal;
					glutPostRedisplay();
				}
			}

			//fprintf(stderr,"%f %f\n", x, y);

		}

		int Click( int button, int mstate, double x, double y )
		{
			int result = 0;
			if ( state == bts_clicked ) {
				if( mstate == GLUT_UP ) {

					result = RELEASE;

					state = bts_normal;
					Motion(x,y);
					glutPostRedisplay();
					return result;
				}

			}


			double X = this->x - sizeX / 2.0;
			double Y = this->y - sizeY / 2.0;

			if ( x > X && x < X + sizeX &&
			     y > Y && y < Y + sizeY ) {

				if ( button == GLUT_LEFT_BUTTON && mstate == GLUT_DOWN ) {

					glutPostRedisplay();
					state = bts_clicked;
					#ifdef USE_SOUND
					playSound(MOUSE_CLICK);
					#endif

					result = PRESS;
				}

			} else {
				if( state != bts_normal ) {
					state = bts_normal;
					glutPostRedisplay();
				}
			}

			return result;
		}

		//void (*OnPress)(void);
		//void (*OnRelease)(void);

	};

};

#endif
