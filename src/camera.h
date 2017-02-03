#ifndef CAMERA_H
#define CAMERA_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

#include "defs.h"
#include "vector3d.h"

namespace soccer {

	class CCamera {						

	protected:
		CVector3D pos; // Posição da Camera
		CVector3D target; // Alvo da Camera
		CVector3D up; // Vetor de UP


	public:

		CCamera(void) : pos(), target(), up()
		{
			return;		
		}

		CCamera(double ox, double oy, double oz, 
			double tx, double ty, double tz,
			double ux, double uy, double uz) : pos(ox,oy,oz), target(tx,ty,tz), up(ux,uy,uz)
		{

			return;		
		}

		void display(void) 
		{	
			glMatrixMode(GL_MODELVIEW);
			// Inicializa sistema de coordenadas do modelo
			glLoadIdentity();
			// Especifica posição do observador e do alvo
			gluLookAt(pos.x, pos.y, pos.z, target.x, target.y, target.z, up.x, up.y, up.z);
		}

		void set(double ox, double oy, double oz, 
		    double tx, double ty, double tz,
		    double ux, double uy, double uz )
		{
			setPos(ox,oy,oz);
			setTarget(tx,ty,tz);
			setUp(ux,uy,uz);
		}

		void setPos(double ox, double oy, double oz) 
		{
			pos.set(ox,oy,oz);
		}

		void setPos(CVector3D &v) 
		{
			pos.set(v);
		}

		void setTarget(CVector3D &v)
		{
			target.set(v);
		}

		void setTarget(double tx, double ty, double tz) 
		{
			target.set(tx,ty,tz);
		}

		void setUp(double ux, double uy, double uz) 
		{
			up.set(ux,uy,uz);
		}

		void setUp(CVector3D &v)
		{
			up.set(v);
		}

		CVector3D getPos(void)
		{
			return pos;
		}

		CVector3D getTarget(void)
		{
			return target;
		}

	};

	class CFocusCamera : public CCamera {

		double pan;

		double roll;

		double tilt;

		double distance;

		void calcTarget( void )
		{
			CVector3D t(0,0,0);
			
			double d1 = distance * cos(TO_RADIANS(tilt));

			t.y = distance * sin(TO_RADIANS(tilt)) + pos.y;

			t.z = d1 * sin(TO_RADIANS(pan)) + pos.z;

			t.x = d1 * cos(TO_RADIANS(pan)) + pos.x;

			setTarget(t);
		}

		void calcUp(void)
		{
			CVector3D t(0,1,0);

			t.x = cos(TO_RADIANS(roll));
			t.y = sin(TO_RADIANS(roll));
			t.z = 0;
				
			setUp(t);
		}

	public:
		CFocusCamera() : CCamera() {
			distance = tilt = pan = 0.0;	

			roll = 90.0;

			calcUp();
			calcTarget();
		}

		CFocusCamera(double ox, double oy, double oz, double dist ) : CCamera() 
		{
			setPos(ox, oy, oz);
			
			distance = dist;

			tilt = pan = 90.0;	

			roll = 90.0;

			calcTarget();
			calcUp();
			// Calculado o novo posicionamento;
		}

		void setX( double x ) 
		{
			pos.x = x;
			calcTarget();
		}

		void setY( double y ) 
		{
			pos.y = y;
			calcTarget();
		}

		void setZ( double z ) 
		{
			pos.z = z;
			calcTarget();
		}

		void incX( double x ) 
		{
			pos.x += x;
			target.x += x;			
		}

		void incY( double y ) 
		{
			pos.y += y;
			target.y += y;			
		}

		void incZ( double z ) 
		{
			pos.z += z;
			target.z += z;
		}
			
		double getX( void )
		{
			return pos.x;
		}

		double getY( void )
		{
			return pos.y;
		}

		double getZ( void )
		{
			return pos.z;
		}

		void setPan( double x ) 
		{
			pan = x;
			calcTarget();
		}

		void setTilt( double y ) 
		{
			tilt = y;
			calcTarget();
		}

		void setRoll( double z ) 
		{
			roll = z;
			calcUp();
		}
			
		double getPan( void )
		{
			return pan;
		}

		double getTilt( void )
		{
			return tilt;
		}

		double getRoll( void )
		{
			return roll;
		}

		void lookTarget(double tx, double ty, double tz) 
		{
			double dx = tx - pos.x;
			double dy = ty - pos.y;
			double dz = tz - pos.z;

			distance = sqrt( POW2(dx) + POW2(dy) + POW2(dz) );

			tilt = TO_DEGREES(asin(dy / distance));
			pan = TO_DEGREES(atan(dz / dx));

			//calcTarget();	

			setTarget(tx,ty,tz);
		}

		void walk( double dist )
		{
			
			//CVector3D vf = pos * ( 1 - dist / pos.abs() );
			CVector3D vf = pos + ( target - pos  ).normalize() * ( dist );
			
			setPos(vf);
		}
		

	};

	class CWalkCamera : public CCamera {

		CVector3D dir;

		double dist;

		void calcTarget( void )
		{
			CVector3D t;
			
			t = pos + dir * dist;

			setTarget(t);
		}

	public:
	
		CWalkCamera() : CCamera() {
			dist = 10.0;	

			setUp(0,1,0);
			setDir(0,0,1);
			calcTarget();
		}

		CWalkCamera(GLdouble ox, GLdouble oy, GLdouble oz, GLdouble dist ) : CCamera() 
		{
			setPos(ox, oy, oz);
			
			this->dist = dist;

			calcTarget();
			setUp(0,1,0);
			setDir(0,0,1);
			// Calculado o novo posicionamento;
		}

		void setDir(double ox, double oy, double oz) 
		{
			dir.set(ox,oy,oz);
			dir.normalize();
			calcTarget();
		}

		void setDir(CVector3D &v) 
		{
			dir.set(v);
			dir.normalize();
			calcTarget();
		}

		void setPos(double ox, double oy, double oz) 
		{
			pos.set(ox,oy,oz);
			calcTarget();
		}

		void Pan( double angle )
		{
			dir.rotateY(angle);
			calcTarget();
		}

		void Tilt( double angle )
		{
			
			CVector3D r = CVector3D(0,1,0) * dir.normal();

			dir.rotateR(angle,r);
			calcTarget();
		}

		void Roll( double angle )
		{
			up.rotateZ(angle);
			calcTarget();
		}

		void resetUp( void )
		{
			setUp(0,1,0);
		}

		void walk( double pdist )
		{			
			pos = pos + dir * ( pdist );

			calcTarget();
		}
		

	};

};

#endif
