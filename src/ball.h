#ifndef BALL_H
#define BALL_H

#include "vector3d.h"
#include "libsoccer.h"
#include "common.h"

namespace soccer {

	class CBall {
	
// 		GLfloat matColorDiffuse[4]; //= {1.0f,1.0f,1.0f, 1.0f};
// 		GLfloat matColorEmission[4]; //= {0.0f,1.0f,0.0f, 1.0f};
// 		GLfloat matColorEspecular[4]; //= {1.0f,1.0f,1.0f, 1.0f};
		Color color;

		TEX *texture;

		int nSlices;
		int nStacks;

// 		GLfloat texgen_s[4];
// 		GLfloat texgen_t[4];

// 		GLUquadric *bola;

		unsigned int listID;

	public:	
		double r;

		double mass;
		double massInv;
		double moment;
		double momentInv;

		CVector3D oldforce;
		CVector3D force;
		CVector3D torque;
		CVector3D oldtorque;

		CVector3D pos;
		CVector3D vel;
		CVector3D w;
		CVector3D rot;

		void zero(void) {
			vel = CVector3D(0,0,0);
			w = CVector3D(0,0,0);
			force = CVector3D(0,0,0);
			torque = CVector3D(0,0,0);
			oldtorque = CVector3D(0,0,0);
			oldforce = CVector3D(0,0,0);
		}

		CBall() : color(1,1,1,1), force(0,0,0), torque(0,0,0), pos(0,0,0), vel(0,0,0), w(0,0,0), rot(0,0,0)
		{
			r = 0.3;
			nSlices = 20;
			nStacks = 20;
			//mass = 0.5;
			mass = 0.9;
			massInv = 1.0 / mass;

			moment = 2.0 / 5.0 * mass * POW2(r);
			momentInv = 1.0 / moment;

// 			matColorDiffuse[0] = 1.0f; matColorDiffuse[1] = 1.0f; 
// 			matColorDiffuse[2] = 1.0f; matColorDiffuse[3] = 1.0f; 
// 
// 			matColorEmission[0] = 0.3f; matColorEmission[1] = 0.3f; 
// 			matColorEmission[2] = 0.3f; matColorEmission[3] = 0.3f; 
// 
// 			matColorEspecular[0] = 1.0f; matColorEspecular[1] = 1.0f; 
// 			matColorEspecular[2] = 1.0f; matColorEspecular[3] = 1.0f; 

			LoadMaterialsFile("materials/ball.mtl");


			//Esfera
			texture = LoadTexture("textures/ball.jpg",true);	

// 			texgen_s[0] = 1.0; texgen_s[1] = 0.0; texgen_s[2] = 0.0; texgen_s[3] = 0.5;
// 			texgen_t[0] = 0.0; texgen_t[1] = 1.0; texgen_t[2] = 0.0; texgen_t[3] = 0.5;

//glCallList(listID+2);

			listID = getlistID();

// 			bola = gluNewQuadric();
// 
// 			gluQuadricDrawStyle(bola,GLU_FILL);
// 			gluQuadricNormals(bola,GLU_SMOOTH);
// 			gluQuadricOrientation(bola,GLU_OUTSIDE);
// 			gluQuadricTexture(bola,GL_TRUE);
		}

		void Draw( int mode = 1 ) 
		{
			glPushMatrix();

			// Sombra R = r * (lpos/(lpos - bpos))
			glColor4f(0.0, 0.0, 0.0, fmin( 30 * r / pos.y, 1.0 ) );
			glutCircleF( r * ( 60.0 / (60.0 - pos.y + r) ) ,pos.x,0.01,pos.z);
			
			glTranslatef(pos.x,pos.y,pos.z);
			glRotatef(TO_DEGREES(rot.x),0,0,1);
			glRotatef(TO_DEGREES(rot.y),0,1,0);
			glRotatef(TO_DEGREES(rot.z),1,0,0);

			if ( mode == 0 || mode == 2 ) {

				if( useMaterial ) {
					glEnable(GL_COLOR_MATERIAL);	
					resetMaterial();
					applyMaterial("Ball",true);
				} 

				glBindTexture(GL_TEXTURE_2D, texture->texid);

				// Seleciona o modo de sphere mapping
				//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR ); //GL_REFLECTION_MAP
				//glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );//GL_SPHERE_MAP, GL_OBJECT_LINEAR
				
				// Define os filtros de magnifica�o e minifica�o
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_NEAREST
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

				//glTexGenfv(GL_S, GL_OBJECT_PLANE, texgen_s );
				//glTexGenfv(GL_T, GL_OBJECT_PLANE, texgen_t );			
				
				// Seleciona o modo de aplica�o da textura
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_CLAMP);//GL_CLAMP_TO_EDGE	
				glColor4f(color.R, color.G, color.B, color.A);

//  				glMatrixMode( GL_TEXTURE );
//  				glPushMatrix();
//  				glLoadIdentity();
//  				glScalef( 1.0, 1.0, 0 );
//  				glTranslatef( -5.0, -5.0, 0.0f );
//  				glMatrixMode( GL_MODELVIEW );

				glEnable(GL_TEXTURE_2D);
				//glEnable(GL_TEXTURE_GEN_S);
				//glEnable(GL_TEXTURE_GEN_T);	

				//glutSolidSphere(r,nSlices,nStacks);
				//gluSphere(bola,r,nSlices,nStacks);
				glPushMatrix();
				glScalef(r,r,r);
				glCallList(listID);
				glPopMatrix();

				//glDisable(GL_TEXTURE_GEN_S);
				//glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_COLOR_MATERIAL);
			}

			if ( mode == 1 || mode == 2 ) {
				glColor3f(1.0f,1.0f,1.0f);
				glutWireSphere(r,nSlices,nStacks);
			}
			
			//resetMaterial();
			glPopMatrix();
// 			glMatrixMode( GL_TEXTURE );
// 			glPopMatrix();
// 			glMatrixMode( GL_MODELVIEW );
		}

		~CBall()
		{
// 			gluDeleteQuadric(bola);
		}

	};

};

#endif
