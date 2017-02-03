#ifndef FLAGMAN_H
#define FLAGMAN_H

#include "vector3d.h"
#include "ball.h"
#include "soccerfield.h"
#include "rigidbody.h"
#include "common.h"
#include <string>

using std::string;
namespace soccer {

	typedef enum {
		efRunning,
		efWaiting,
		efDone

	} eFlagManState;

	class CFlagMan : public CRigidBody {

		TEX *texture;
		TEX *skin;
		TEX *body;

		unsigned int listID;

		double Scale;

		bool increaseAplha0, increaseAplha1, increaseTheta0, increaseTheta1, incAplhaColor;

		double alphaColor;

		void gotoPos( CVector3D pf, CVector3D po )
		{
			CVector3D d = pf - po, ref = CVector3D(0,0,1);
			double a, angle;
			d.y = 0;
			a = d.abs();
			d.normalize();

			front = d;

			vel = d * (vel.abs());

			angle = vector_angle( ref, front );

			if( front.x > 0 )
				rot = angle;
			else
				rot = -angle;

			vel = front * 5.0;
		}

		void pointsTo( CVector3D pf )
		{
			CVector3D po = this->pos;
			CVector3D d = pf - po, ref = CVector3D(0,0,1);
			double a, angle;
			d.y = 0;
			a = d.abs();
			d.normalize();

			front = d;

			angle = vector_angle( ref, front );

			if( front.x > 0 )
				rot = angle;
			else
				rot = -angle;
		}

		void cIA_Running( CVector3D &target, CBall *b, bool posTeam01 )
		{

			double d = fabs(target.z - pos.z);

 			if ( d < 10.0 ) {

				state = efWaiting;

			} else {
				CVector3D newtarget = CVector3D(pos.x, 0.0f, target.z);

				gotoPos(newtarget,pos);
			}
		}

		void cIA_Wait( CVector3D &targetPos, CBall *b, bool posTeam01 )
		{
			double d = fabs(targetPos.z - pos.z);

 			if ( d > 15.0 ) {

				state = efRunning;

			}
		}


	public:

		eFlagManState state;

		CVector3D p0;

		string name;

		double beta0, aplha0, beta1, aplha1, theta0, theta1;

		double headY;

		void rotateY( double angle )
		{
			front.rotateY(angle);
			vel.rotateY(angle);
			rot += angle;
		}

		~CFlagMan()
		{
		}

		CFlagMan( const char *name ) : CRigidBody(), p0(0,0,0)
		{
			//Scale = 1.0 / 100.0;
			texture = LoadTexture("textures/face02.jpg",true);

			skin = LoadTexture("textures/skin.jpg",true);
			body = LoadTexture("textures/body.jpg",true);

			state = efWaiting;

			LoadMaterialsFile("materials/player.mtl");

			Scale = 2.0;

			alphaColor = 0.3f;

			listID = getlistID();

			this->name = name;

			l = Scale * 0.35;
			w = Scale * 0.35;
			h = Scale * 1.6f;

			r = Scale * l * sqrt(2) / 2.0;

			pos.y = h / 2.0;
			headY = pos.y + Scale * 0.5;
			beta0 = 0;
			aplha0 = 0;
			theta0 = 0;

			beta1 = 0;
			aplha1 = 0;
			theta1 = 0;

			incAplhaColor = true;

			increaseAplha0 = true;
			increaseTheta0 = true;
			increaseAplha1 = false;
			increaseTheta1 = false;

			mass = 70.0;

 			massInv = 1.0 / mass;

			rot = 0.0;
		}

		void animate(double elapsed)
		{

			int count = 1 + (int)(vel.abs() / 6.0);

			if( vel.abs() > 1.0 ) {
				for( register int i = 0 ; i < count ; i++ ) {
					animateLeftHand(elapsed);
					animateRigthHand(elapsed);
					animateLeftLeg(elapsed);
					animateRigthLeg(elapsed);
				}
			} else {
				animateLeftHand(elapsed,true);
				animateRigthHand(elapsed,true);
				animateLeftLeg(elapsed,true);
				animateRigthLeg(elapsed,true);
			}

			alphaColor += (incAplhaColor) ? 0.005 : -0.005;

			if ( alphaColor >= 1.0 )
				incAplhaColor = false;
			else if ( alphaColor <= 0.3 )
				incAplhaColor = true;
		}

		void animateLeftHand(double elapsed, bool seek = false )
		{

			if ( seek ) {
				if ( aplha0 == 0 ) {
					increaseAplha0 = true;
					return;
				}
			}

			if ( aplha0 >= 60 ) {
				increaseAplha0 = false;
			}

			if ( aplha0 <= -60 ) {
				increaseAplha0 = true;
			}

			if ( increaseAplha0 ) {
				aplha0 += ANIM_INC * elapsed;
			} else {
				aplha0 -= ANIM_INC * elapsed;
			}

		}

		void animateLeftLeg(double elapsed, bool seek = false )
		{
			if ( seek ) {
				if ( theta0 == 0 ) {
					increaseTheta0 = true;
					return;
				}
			}

			if ( theta0 >= 30 ) {
				increaseTheta0 = false;
			}

			if ( theta0 <= -30 ) {
				increaseTheta0 = true;
			}

			if ( increaseTheta0 ) {
				theta0 += 0.5 * ANIM_INC * elapsed;
			} else {
				theta0 -= 0.5 * ANIM_INC * elapsed;
			}
		}

		void animateRigthLeg(double elapsed, bool seek = false )
		{
			if ( seek ) {
				if ( theta1 == 0 ) {
					increaseTheta1 = false;
					return;
				}
			}

			if ( theta1 >= 30 ) {
				increaseTheta1 = false;
			}

			if ( theta1 <= -30 ) {
				increaseTheta1 = true;
			}

			if ( increaseTheta1 ) {
				theta1 += 0.5 * ANIM_INC * elapsed;
			} else {
				theta1 -= 0.5 * ANIM_INC * elapsed;
			}
		}

		void animateRigthHand(double elapsed, bool seek = false )
		{

			if ( seek ) {
				if ( aplha1 == 0 ){
					increaseAplha1 = false;
					return;
				}
			}

			if ( aplha1 >= 60 ) {
				increaseAplha1 = false;
			}

			if ( aplha1 <= -60 ) {
				increaseAplha1 = true;
			}

			if ( increaseAplha1 ) {
				aplha1 += ANIM_INC * elapsed;
			} else {
				aplha1 -= ANIM_INC * elapsed;
			}

		}

		void runIA( bool canRun, CBall *b, bool posTeam01)
		{

			CVector3D targetPos = b->pos;

			if( !canRun )
				return;

			switch(state) {
				case efRunning: cIA_Running(targetPos,b,posTeam01); break;
				case efWaiting:
				default:  cIA_Wait(targetPos,b,posTeam01); break;
			}
		}

		virtual void Draw( int mode = 1 )
		{
			if( useBodyView )
				CRigidBody::Draw(1);
			resetMaterial();
			glPushMatrix();
			glTranslatef(pos.x,pos.y,pos.z);
			glRotatef(rot,0,1,0);
			glScalef(Scale,Scale,Scale);
			glTranslatef(0.0,0.2,0.0);

			// Define os filtros de magnifica�o e minifica�o
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_NEAREST
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			// Seleciona o modo de aplica�o da textura
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_CLAMP);//GL_CLAMP_TO_EDGE

			if( useMaterial ) {
				glEnable(GL_COLOR_MATERIAL);
				resetMaterial();
				applyMaterial("Player",true);
			}

			glEnable(GL_TEXTURE_2D);
			// Associa a textura aos comandos seguintes
			glBindTexture(GL_TEXTURE_2D, body->texid);
			// Corpo
 			glPushMatrix();
				glScalef(0.35,0.5,0.35);
				glCallList(listID+2);
 			glPopMatrix();
			glDisable(GL_TEXTURE_2D);


			glEnable(GL_TEXTURE_2D);
			// Associa a textura aos comandos seguintes
			glBindTexture(GL_TEXTURE_2D, skin->texid);

			// Pescoço
 			glPushMatrix();
				glTranslatef(0.0,0.3,0.0);
				CubeList(0.1f);
 			glPopMatrix();
			glDisable(GL_TEXTURE_2D);

			glEnable(GL_TEXTURE_2D);
			// Associa a textura aos comandos seguintes
			glBindTexture(GL_TEXTURE_2D, texture->texid);

			glPushMatrix();
				glTranslatef(0.0,0.45,0.0);
				glRotatef( -90, 1, 0, 0 );
				glScalef(0.15f,0.15f,0.15f);
				glCallList(listID);
			glPopMatrix();
			glDisable(GL_TEXTURE_2D);

			//Braço 1
			glPushMatrix();

				glTranslatef(0.175,0.125,0.0);

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, skin->texid);
				CubeList(0.05f);
				glDisable(GL_TEXTURE_2D);

				glTranslatef(0.05,0.0,0.0);

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, skin->texid);
				SphereList(0.05f);
				glDisable(GL_TEXTURE_2D);


				glPushMatrix();
					glRotatef(beta0,0,0,1);
					glRotatef(aplha0,-1,0,0);
					glTranslatef(0.0,-0.175,0.0);

					glPushMatrix();
						glScalef(0.05,0.3,0.05);
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, skin->texid);
						glCallList(listID+1);
						glDisable(GL_TEXTURE_2D);
					glPopMatrix();

					glTranslatef(0.0,-0.175,0.0);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, skin->texid);
					SphereList(0.05f);
					glDisable(GL_TEXTURE_2D);

					glRotatef(45,-1,0,0);
					glTranslatef(0.0,-0.125,0.0);
					glPushMatrix();
						glScalef(0.05,0.2,0.05);
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, skin->texid);
						glCallList(listID+1);
						glDisable(GL_TEXTURE_2D);
					glPopMatrix();

					glTranslatef(0.0,-0.125,0.0);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, skin->texid);
					SphereList(0.05f);
					glDisable(GL_TEXTURE_2D);

				glPopMatrix();
 			glPopMatrix();

			//Braço 2
			glPushMatrix();
				glTranslatef(-0.175,0.125,0.0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, skin->texid);
				CubeList(0.05f);
				glDisable(GL_TEXTURE_2D);

				glTranslatef(-0.05,0.0,0.0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, skin->texid);
				SphereList(0.05f);
				glDisable(GL_TEXTURE_2D);

				glPushMatrix();
					glRotatef(beta1,0,0,-1);
					glRotatef(aplha1,-1,0,0);
					glTranslatef(0.0,-0.175,0.0);
					glPushMatrix();
						glScalef(0.05,0.3,0.05);
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, skin->texid);
						glCallList(listID+1);
						glDisable(GL_TEXTURE_2D);
					glPopMatrix();
					glTranslatef(0.0,-0.175,0.0);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, skin->texid);
					SphereList(0.05f);
					glDisable(GL_TEXTURE_2D);

					glRotatef(45,-1,0,0);
					glTranslatef(0.0,-0.125,0.0);
					glPushMatrix();
						glScalef(0.05,0.2,0.05);
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, skin->texid);
						glCallList(listID+1);
						glDisable(GL_TEXTURE_2D);
					glPopMatrix();

					glTranslatef(0.0,-0.125,0.0);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, skin->texid);
					SphereList(0.05f);
					glDisable(GL_TEXTURE_2D);
				glPopMatrix();

 			glPopMatrix();

			// Perna 1
			glPushMatrix();
				glTranslatef(0.075,-0.25,0.0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, skin->texid);
				SphereList(0.05f);
				glDisable(GL_TEXTURE_2D);

				glPushMatrix();
					glRotatef(theta0,-1,0,0);
					glTranslatef(0.0,-0.35,0.0);
					glPushMatrix();
						glScalef(0.1,0.6,0.1);
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, skin->texid);
						glCallList(listID+1);
						glDisable(GL_TEXTURE_2D);
					glPopMatrix();

					glTranslatef(0.0,-0.35,0.0);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, skin->texid);
					SphereList(0.05f);
					glDisable(GL_TEXTURE_2D);

					glTranslatef(0.0,0.0,0.125);
					glPushMatrix();
						glScalef(0.1,0.1,0.15);
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, skin->texid);
						glCallList(listID+1);
						glDisable(GL_TEXTURE_2D);
					glPopMatrix();

				glPopMatrix();
 			glPopMatrix();


			// Perna 2
			glPushMatrix();
				glTranslatef(-0.075,-0.25,0.0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, skin->texid);
				SphereList(0.05f);
				glDisable(GL_TEXTURE_2D);

				glPushMatrix();
					glRotatef(theta1,-1,0,0);
					glTranslatef(0.0,-0.35,0.0);
					glPushMatrix();
						glScalef(0.1,0.6,0.1);
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, skin->texid);
						glCallList(listID+1);
						glDisable(GL_TEXTURE_2D);
					glPopMatrix();

					glTranslatef(0.0,-0.35,0.0);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, skin->texid);
					SphereList(0.05f);
					glDisable(GL_TEXTURE_2D);

					glTranslatef(0.0,0.0,0.125);
					glPushMatrix();
						glScalef(0.1,0.1,0.15);
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, skin->texid);
						glCallList(listID+1);
						glDisable(GL_TEXTURE_2D);
					glPopMatrix();

				glPopMatrix();
 			glPopMatrix();

			glDisable(GL_COLOR_MATERIAL);
			resetMaterial();
			glPopMatrix();
		}
	};

};

#endif
