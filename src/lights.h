#ifndef LIGHTS_H
#define LIGHTS_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "vector3d.h"

namespace soccer {

	class CLight {
		
	protected:
		float ambient[4];
		float diffuse[4];
		float specular[4];
		float position[4];
		
		int iId;

		double c_atenuation;
		double l_atenuation;
		double q_atenuation;
		
	public:

		void setConstAtenuation( double v ) 
		{
			c_atenuation = v;
		}

		void setLinearAtenuation( double v ) 
		{
			l_atenuation = v;
		}

		void setQuadAtenuation( double v ) 
		{
			q_atenuation = v;
		}

		virtual ~CLight() {

		}

		CLight( int iID = 0 ) {
			this->iId = iID;
			c_atenuation = 0.5;
			l_atenuation = 0.025;
			q_atenuation = 0.0;
		}

		virtual void place(int iIndex = -1) 
		{
			iIndex = (iIndex == -1)?iId:iIndex;

			glLightfv(GL_LIGHT0 + iIndex, GL_AMBIENT, ambient);
			glLightfv(GL_LIGHT0 + iIndex, GL_DIFFUSE, diffuse);
			glLightfv(GL_LIGHT0 + iIndex, GL_SPECULAR, specular);
			glLightfv(GL_LIGHT0 + iIndex, GL_POSITION, position);
    			glLightf(GL_LIGHT0 + iIndex, GL_CONSTANT_ATTENUATION, c_atenuation);
    			glLightf(GL_LIGHT0 + iIndex, GL_LINEAR_ATTENUATION, l_atenuation);
    			glLightf(GL_LIGHT0 + iIndex, GL_QUADRATIC_ATTENUATION, q_atenuation);
//fprintf(stderr,"%f,%f,%f\n", diffuse[0],diffuse[1],diffuse[2]);			
			
		}

		virtual void draw( void ) 
		{
			bool Enable = false;

			Enable = glIsEnabled(GL_LIGHTING);	

// 			if ( useLight ) {
// 				glDisable(GL_LIGHTING);
// 				Enable = true;
// 			}

			glPushMatrix();
				glTranslatef(position[0], position[1], position[2]);
				glColor3f(diffuse[0],diffuse[1],diffuse[2]);
				glutSolidSphere(1,5,5);
			glPopMatrix();
			
			if ( Enable ) {
				glEnable(GL_LIGHTING);
			}	
		}

		void setAmbient( double a0, double a1, double a2, double a3 )
		{
			ambient[0] = a0;
			ambient[1] = a1;
			ambient[2] = a2;
			ambient[3] = a3;
		}

		void setDiffuse( double a0, double a1, double a2, double a3 )
		{
			diffuse[0] = a0;
			diffuse[1] = a1;
			diffuse[2] = a2;
			diffuse[3] = a3;
		}

		void setSpecular( double a0, double a1, double a2, double a3 )
		{
			specular[0] = a0;
			specular[1] = a1;
			specular[2] = a2;
			specular[3] = a3;
		}

		void setPosition( double a0, double a1, double a2 )
		{
			position[0] = a0;
			position[1] = a1;
			position[2] = a2;
			position[3] = 1.0;
		}

		int getID( void ) 
		{
			return iId;
		}

		void setID( int ID ) 
		{
			iId = ID;
		}
		
	};

	class CSpotLight : public CLight {
	
		float direction[3];
		float cutoff;
		float exponent;
	public:
		CSpotLight(int iID) : CLight(iID)
		{
			cutoff = 0.0;
			exponent = 0.0;
		}

		virtual ~CSpotLight()
		{

		}
		
		virtual void place( int iIndex = -1) 
		{
			iIndex = (iIndex == -1)?iId:iIndex;
			CLight::place(iIndex);	

 			glLightfv(GL_LIGHT0 + iIndex, GL_SPOT_DIRECTION, direction);
 			glLightf(GL_LIGHT0 + iIndex, GL_SPOT_CUTOFF, cutoff);
 			glLightf(GL_LIGHT0 + iIndex, GL_SPOT_EXPONENT, exponent);
		}

		virtual void setDirection( double a0, double a1, double a2 )
		{
			direction[0] = a0;
			direction[1] = a1;
			direction[2] = a2;
		}

		void setCutOff( double c ) 
		{
			cutoff = c;
		}

		void setExponent( double e ) 
		{
			exponent = e;
		}

		void draw( void ) 
		{
			bool Enable = false;

// 			if ( useLight ) {
// 				glDisable(GL_LIGHTING);
// 				Enable = true;
// 			}
			Enable = glIsEnabled(GL_LIGHTING);

			CVector3D v(direction[0], direction[1], direction[2]);
			CVector3D j(0,0,1);

			double theta = vector_angle(v,j);

			CVector3D n = v * j;

			glPushMatrix();
				glTranslatef(position[0], position[1], position[2]);				
				
				if ( n.abs() != 0 ) {
					glRotatef(theta, n.x, n.y, n.z);
				} else {
					 if( v.z != 0 ) 
						glScalef(1.0f,1.0f,v.z);
				}

				glColor3f(diffuse[0],diffuse[1],diffuse[2]);
				glutSolidCone(3,5,5,5);
				glColor3f(0.0f,0.0f,0.0f);
				glutWireCone(3,5,5,5);
			glPopMatrix();
			
			if ( Enable ) {
				glEnable(GL_LIGHTING);
			}	
		}
	};

};

#endif
