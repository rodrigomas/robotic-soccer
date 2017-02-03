#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "vector3d.h"
#include "color.h"

namespace soccer {

	class CRigidBody {
	
	public:	
		double w;
		double h;
		double l;
		double r;

		double mass;
		double massInv;

		CVector3D pos;
		CVector3D vel;
		CVector3D front;
		
		Color color;

		CVector3D force;
		CVector3D oldforce;

		double rot;

		virtual ~CRigidBody() {}
	
		CRigidBody( void ) :  pos(0,0,0), vel(0,0,0), front(0,0,1), color(0,0,0,1), force(0,0,0)
		{

			l = w = h = 1.0f;

			mass = massInv = 1.0;
		
			r = l * sqrt(2) / 2.0;

			rot = 0.0;
		}

		virtual void Draw( int mode = 1 ) 
		{			
			glPushMatrix();
				glTranslatef(pos.x,pos.y,pos.z);
				glRotatef(rot,0,1,0);
				glScalef(w,h,l);
				
				if ( mode == 0 || mode == 2 ) {
					glColor4f(color.R, color.G, color.B, color.A);
					glutSolidCube(1.0f);
				}
				if ( mode == 1 ) {
					glColor3f(0.0f,0.0f,0.0f);
					glutWireCube(1.0f);
				}
			glPopMatrix();
		}

	};

};

#endif
