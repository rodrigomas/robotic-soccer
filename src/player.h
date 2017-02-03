#ifndef PLAYER_H
#define PLAYER_H

#include "vector3d.h"
#include "ball.h"
#include "soccerfield.h"
#include "rigidbody.h"
#include "common.h"
#include "physics.h"
//#include "LuaCall.h"
#include <string>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

using std::string;
namespace soccer {

	typedef enum {
		eRunning,
		eWaiting,
		eKick,
		eDone

	} ePlayerState;

	class CPlayer;
	void playerClickHL( CPlayer *p, CBall *b, int k);
	void playerClickHLA( CPlayer *p, CBall *b, int k);
	void kickForce(  CPlayer *p,  CBall *b, double fx, double fy, double fz );
	void kickToVel(  CPlayer *p,  CBall *b, double px, double pz, double vel );
	void passTo(  CPlayer *p,  CBall *b, double px, double pz);

	class CPlayer : public CRigidBody {

		TEX *texture;
		TEX *skin;
		TEX *body;

		lua_State *L;

		unsigned int listID;

		bool stupid;

		double Scale;

		bool increaseAplha0, increaseAplha1, increaseTheta0, increaseTheta1, incAplhaColor;

		double alphaColor;

		void luaUpdatePos( void )
		{
			lua_getglobal(L,"update_pos");

			if( !lua_isfunction(L,-1) ){
				fprintf(stderr,"[ERRO] %s\n", Language[LG_LUA_ERROR_FCN]);
				return;
			}

			lua_pushinteger(L, num);
			lua_pushnumber(L, pos.x);
			lua_pushnumber(L, pos.z);

			// 3 Argumentos, 0 resultados
			lua_call(L, 3, 0);
		}

		void luaUpdateVel( void )
		{
			lua_getglobal(L,"update_vel");

			if( !lua_isfunction(L,-1) ){
				fprintf(stderr,"[ERRO] %s\n", Language[LG_LUA_ERROR_FCN]);
				return;
			}

			lua_pushinteger(L, num);
			lua_pushnumber(L, vel.x);
			lua_pushnumber(L, vel.z);

			// 3 Argumentos, 0 resultados
			lua_call(L, 3, 0);
		}

		void luaUpdateBallVel( CBall *b )
		{
			lua_getglobal(L,"update_ball_vel");

			if( !lua_isfunction(L,-1) ){
				fprintf(stderr,"[ERRO] %s\n", Language[LG_LUA_ERROR_FCN]);
				return;
			}

			lua_pushnumber(L, b->vel.x);
			lua_pushnumber(L, b->vel.y);
			lua_pushnumber(L, b->vel.z);

			// 3 Argumentos, 0 resultados
			lua_call(L, 3, 0);
		}

		void luaUpdateBallPos( CBall *b )
		{
			lua_getglobal(L,"update_ball_pos");

			if( !lua_isfunction(L,-1) ){
				fprintf(stderr,"[ERRO] %s\n", Language[LG_LUA_ERROR_FCN]);
				return;
			}

			lua_pushnumber(L, b->pos.x);
			lua_pushnumber(L, b->pos.y);
			lua_pushnumber(L, b->pos.z);

			// 3 Argumentos, 0 resultados
			lua_call(L, 3, 0);
		}

		void luaUpdateGoalPos( bool fst_time )
		{
			lua_getglobal(L,"update_goal_pos");

			if( !lua_isfunction(L,-1) ){
				fprintf(stderr,"[ERRO] %s\n", Language[LG_LUA_ERROR_FCN]);
				return;
			}

			if( fst_time ) {
				lua_pushnumber(L, 60.0);
			} else {
				lua_pushnumber(L, -60.0);
			}

			// 1 Argumentos, 0 resultados
			lua_call(L, 1, 0);
		}

		void gotoPos( CVector3D pf, CVector3D po, double spd = 2.0 )
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

			vel = front * spd;
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

		void pIA_Running( bool EnemyGetTheBall, CVector3D target  )
		{

			if ( !EnemyGetTheBall ) {
				state = eWaiting;
				return;
			}


			CVector3D d = target - pos;
			d.y = 0;
 			if ( d.abs() > 30.0 ) {

				state = eWaiting;

			} else if( d.abs() > 3.0 ) {

				if( num == 1 ) { // Goleiro
					if( fabs(target.z) < 45.0 ) {
						target.z = (target.z < 0 ) ? -45.0 : 45.0;
					}

					if( fabs(target.z) < 18.0 ) {
						target.x = (target.x < 0 ) ? -18.0 : 18.0;
					}
				}

				gotoPos(target,pos);

			} else if ( d.abs() <= 3.0 ) {
				state = eDone;
			}
		}

		void pIA_Wait( CVector3D targetPos )
		{

			CVector3D d = targetPos - pos;
			d.y = 0;

 			if ( d.abs() < 20.0 ) {

				state = eRunning;

			} else if( (pos - p0).abs() > 3.0 ) {

				gotoPos(p0,pos);

				//std::cerr << p0.x << "  " << p0.z << std::endl;

			}
		}

		void cIA_Running( bool EnemyGetTheBall, CVector3D &target, CSoccerField *f, CBall *b, bool posTeam01 )
		{

			CVector3D d = target - pos;
			d.y = 0;
 			if ( d.abs() > 30.0 ) {

				state = eWaiting;

			} else if( d.abs() > 3.0 ) {

				if( num == 1 ) { // Goleiro
					if( fabs(target.z) < 45.0 ) {
						target.z = (target.z < 0 ) ? -45.0 : 45.0;
					}

					if( fabs(target.z) < 18.0 ) {
						target.x = (target.x < 0 ) ? -18.0 : 18.0;
					}
				}

				gotoPos(target,pos);

			} else if ( d.abs() <= 3.0 ) {

				CVector3D newtarget;

				if( team == 0 ) {
					if( posTeam01 ) { // Inimigo no negativo
						newtarget = CVector3D(0,0,-60.0);
					} else {
						newtarget = CVector3D(0,0,60.0);
					}
				} else {
					if( posTeam01 ) {
						newtarget = CVector3D(0,0,60.0);

					} else {
						newtarget = CVector3D(0,0,-60.0);
					}
				}

				pointsTo(newtarget);

				state = eKick;

				playerClickHL(this,b,140);

				state = eRunning;

				//gotoPos(target,pos);
			}

		}

		void cIA_Wait( CVector3D &targetPos, CSoccerField *f, CBall *b, bool posTeam01 )
		{
			CVector3D d = targetPos - pos;
			d.y = 0;

 			if ( d.abs() < 20.0 ) {

				state = eRunning;

			} else if( (pos - p0).abs() > 3.0 ) {

				gotoPos(p0,pos);

				//std::cerr << p0.x << "  " << p0.z << std::endl;

			}
		}

	public:

		ePlayerState state;

		Color colorBody;

		CVector3D p0, pf;

		int team;

		string name;
		string type;
		int num;

		int ncards;

		bool isCPU;

		double beta0, aplha0, beta1, aplha1, theta0, theta1;

		bool isSelected;

		double headY;

		void rotateY( double angle )
		{
			front.rotateY(angle);
			vel.rotateY(angle);
			rot += angle;
		}

		~CPlayer()
		{
		}

		CPlayer( const char *name, int num, const char *camisa, const char *ctype, int Team, lua_State *L, bool stupid ) : CRigidBody(), colorBody(0.7,0.7, 0.7,1.0f), p0(0,0,0), pf(0,0,0)
		{
			//Scale = 1.0 / 100.0;
			listID = getlistID();

			this->stupid = stupid;

			this->L = L;
			team = Team;
			if( rand() % 2 == 1 )  {
				texture = LoadTexture("textures/face01.jpg",true);
			} else {
				texture = LoadTexture("textures/face02.jpg",true);
			}

			skin = LoadTexture("textures/skin.jpg",true);

			body = LoadTexture((char*)camisa,true);

			type = ctype;

			state = eWaiting;

			//applyMaterial
			LoadMaterialsFile("materials/player.mtl");

			Scale = 2.0;

			ncards = 0;

			isCPU = false;

			alphaColor = 0.3f;

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

			isSelected = false;

			mass = 70.0;

			this->num = num;

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
				if ( aplha0 > -ANIM_SEEK && aplha0 < ANIM_SEEK ) {
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
				if ( theta0 > -ANIM_SEEK && theta0 < ANIM_SEEK ) {
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
				if ( theta1 > -ANIM_SEEK && theta1 < ANIM_SEEK ) {
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
				if ( aplha1 > -ANIM_SEEK && aplha1 < ANIM_SEEK ){
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

		void runIA( bool EnemyGetTheBall,bool canRun, CSoccerField *f, CBall *b, bool posTeam01)
		{

			if( ncards >= 2 )
				return;

			if( !canRun )
				return;

			if( stupid && isCPU) {

				CVector3D targetPos = b->pos;

				switch(state) {
					case eRunning: cIA_Running(EnemyGetTheBall,targetPos,f,b,posTeam01); break;
					case eWaiting:
					default:  cIA_Wait(targetPos,f,b,posTeam01); break;
				}

			} else if ( stupid &&  !isSelected ) {

				CVector3D targetPos = b->pos;

				switch(state) {
					case eRunning: pIA_Running(EnemyGetTheBall,targetPos); break;
					case eWaiting:
					case eDone:
					default: pIA_Wait(targetPos); break;
				}

			} else if( isCPU || !isSelected  ) {

				int result;
				double rets[3];

				//lua_settop(L,0);

				luaUpdatePos();
				luaUpdateVel();

				luaUpdateBallVel(b);
				luaUpdateBallPos(b);

				if( team == 0 ) {
					luaUpdateGoalPos(posTeam01);
				} else {
					luaUpdateGoalPos(!posTeam01);
				}

				lua_getglobal(L,"think");

				if( !lua_isfunction(L,-1) ){
					stupid = true;
					fprintf(stderr,"[ERRO] %s\n", Language[LG_LUA_ERROR_FCN]);
					return;
				}

				lua_pushinteger(L, num);
				lua_pushinteger(L, team);

				// 2 Argumentos, 4 resultados
				lua_call(L, 2, 4);

				rets[2] = (double)lua_tonumber(L, -1); lua_pop(L,1);
				rets[1] = (double)lua_tonumber(L, -1); lua_pop(L,1);
				rets[0] = (double)lua_tonumber(L, -1); lua_pop(L,1);
				result = lua_tointeger(L, -1); lua_pop(L,1);


				//fprintf(stderr,"[CPP] R1: %d %d\n", result, s);

				switch(result) {
					case 0:
					break;

					case 1:
						gotoPos(CVector3D(rets[0],0,rets[1]),pos,rets[2]);
					break;

					case 2:
						kickForce(this,b,rets[0],rets[1],rets[2]);
					break;

					case 3:
						kickToVel(this,b,rets[0],rets[1],rets[2]);
					break;

					case 4:
						passTo(this,b,rets[0],rets[1]);
					break;

					default:
					break;
				}
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
				//glutSolidCube(0.1f);
				CubeList(0.1f);
 			glPopMatrix();
			glDisable(GL_TEXTURE_2D);

			glEnable(GL_TEXTURE_2D);
			// Associa a textura aos comandos seguintes
			glBindTexture(GL_TEXTURE_2D, texture->texid);

/*			glMatrixMode( GL_TEXTURE );
			glPushMatrix();
			glLoadIdentity();*/
			//glScalef( 0.9, 0.9, 0.0f );
// 			glMatrixMode( GL_MODELVIEW );

			// Cabeça
 			/*glPushMatrix();
				glTranslatef(0.0,0.45,0.0);
				glRotatef( -90, 1, 0, 0 );
				glColor3f(0.8f, 0.8f, 0.8f);
				gluSphere(head,0.15f,10,10);
				glColor3f(0,0,0);
				//glutWireSphere(0.15f,10,10);
 			glPopMatrix();*/

			glPushMatrix();
				glTranslatef(0.0,0.45,0.0);
				glRotatef( -90, 1, 0, 0 );
				glScalef(0.15f,0.15f,0.15f);
				//glColor3f(0.8f, 0.8f, 0.8f);
				glCallList(listID);
				//glColor3f(0,0,0);
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
			//resetMaterial();
			glPopMatrix();

			if( isSelected ) {

				glDisable(GL_LIGHTING);

				double R = 3.0 * r;
				double L = sqrt(3) * R;

				glPushMatrix();
				glTranslatef(pos.x,0.01,pos.z);
				glRotatef( rot, 0,1,0);

				glColor4f(0.8f, 0.8f, 0.0f, alphaColor);

				glBegin(GL_TRIANGLES);
					glVertex3f( -L / 4.0 , 0.0, -R / 4.0 );
					glVertex3f( 0.0, 0.0, R );
					glVertex3f( L / 4.0 , 0.0, -R/ 4.0 );
				glEnd();

				glLineWidth(3.0f);
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				glBegin(GL_LINE_LOOP);
					glVertex3f( -L / 4.0 , 0.0, -R / 4.0 );
					glVertex3f( 0.0, 0.0, R );
					glVertex3f( L / 4.0 , 0.0, -R/ 4.0 );
// 					glVertex3f( -L / 2.0, 0.0, -R / 2.0 );
// 					glVertex3f( 0.0, 0.0, R );
// 					glVertex3f( L / 2.0, 0.0, -R/ 2.0 );
				glEnd();

				glPopMatrix();
				glLineWidth(1.0f);

				if( useLight )
					glEnable(GL_LIGHTING);

			}

			resetMaterial();
		}
	};

};

#endif
