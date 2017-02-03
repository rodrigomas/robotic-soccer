#ifndef VECTOR3D_H
#define VECTOR3D_H

#include "defs.h"

#include <math.h>

namespace soccer {	
	
	class CVector3D;

	double vector_angle( CVector3D &v1, CVector3D &v2 );

	class CVector3D {
		
	public:

		double x; // Camera X
		double y; // Camera X
		double z; // Camera X

		CVector3D(void)
		{
			x = y = z = 0.0;
		}

		CVector3D(double x, double y, double z)
		{
			this->set(x,y,z);
		}

		CVector3D( CVector3D &Vector )
		{
			this->x = Vector.x; this->y = Vector.y; this->z = Vector.z;
		}


		CVector3D( const CVector3D &Vector )
		{
			this->x = Vector.x; this->y = Vector.y; this->z = Vector.z;
		}

		double abs( CVector3D &v ) {
			return sqrt( POW2(v.x) + POW2(v.y) + POW2(v.z) );
		}

		double abs( void ) {
			return sqrt( POW2(x) + POW2(y) + POW2(z) );
		}

		void set( double x, double y, double z ) 		
		{
			this->x = x; this->y = y; this->z = z;
		}

		void set( CVector3D &Vector )
		{
			this->x = Vector.x; this->y = Vector.y; this->z = Vector.z;
		}

		void set( const CVector3D &Vector )
		{
			this->x = Vector.x; this->y = Vector.y; this->z = Vector.z;
		}

		CVector3D normal(void)
		{
			CVector3D v;
	
			double m = this->abs();

			v.x = x / m;
			v.y = y / m;
			v.z = z / m;
			
			return v;
		}

		CVector3D &normalize(void)
		{
			double m = this->abs();
			
			x /= m;
			y /= m;
			z /= m;
			
			return *this;
		}

		CVector3D &operator=(const CVector3D& v)
		{
			this->set(v);

			return *this;
		}

		CVector3D &operator+=(const CVector3D& v)
		{
			x += v.x; 
			y += v.y;
			z += v.z;

			return *this;
		}
		
		CVector3D &operator-=(const CVector3D& v)
		{
			x -= v.x; 
			y -= v.y;
			z -= v.z;

			return *this;
		}

		CVector3D operator+(const CVector3D& v)
		{
			return CVector3D(x + v.x, y + v.y, z + v.z );
		}

		CVector3D operator-(const CVector3D& v)
		{
			return CVector3D(x - v.x, y - v.y, z - v.z );
		}


		double operator|(const CVector3D& v)
		{
			return x * v.x + y * v.y + z * v.z;
		}

		CVector3D &operator*=(const double val)
		{
			x *= val; 
			y *= val;
			z *= val;

			return *this;
		}

		CVector3D operator*(const double val)
		{
			return CVector3D(x * val, y * val, z * val );
		}

		CVector3D operator/(const double val)
		{
			if( val == 0.0) 
				return CVector3D(x, y, z );
			else
				return CVector3D(x / val, y / val, z / val );
		}

		CVector3D &operator/=(const double val)
		{
			if( val != 0.0) {
				x /= val; 
				y /= val;
				z /= val;
			}

			return *this;
		}

		CVector3D &operator*=(const CVector3D &v)
		{
			x = y * v.z - v.y * z; 
			y = z * v.x - v.z * x;
			z = x * v.y - v.x * y;

			return *this;
		}

		CVector3D operator*(const CVector3D &v)
		{
			return CVector3D(y * v.z - v.y * z, z * v.x - v.z * x, x * v.y - v.x * y);
		}

		double angle(CVector3D &v)
		{
			return vector_angle(*this,v);
		}

		CVector3D &rotateY( double angle ) 
		{
			double a = cos(TO_RADIANS(angle));
			double b = sin(TO_RADIANS(angle));

			x = x * a + z * b; 
			z = z * a - x * b; 

			return *this;			
		}

		CVector3D &rotateZ( double angle ) 
		{
			double a = cos(TO_RADIANS(angle));
			double b = sin(TO_RADIANS(angle));

			x = x * a - y * b; 
			y = x * b + y * a; 

			return *this;			
		}

		CVector3D &rotateX( double angle ) 
		{
			double a = cos(TO_RADIANS(angle));
			double b = sin(TO_RADIANS(angle));

			y = y * a - z * b; 
			z = z * a + y * b; 

			return *this;			
		}

		CVector3D &rotateR( double angle, CVector3D &r ) 
		{
			double a = cos(TO_RADIANS(angle));
			double b = sin(TO_RADIANS(angle));

			CVector3D v = *this;

			v = v * a + r * ( (1-a) * (r | v) ) + ( r * v ) * b;

			x = v.x;
			y = v.y;
			z = v.z;

			return *this;			
		}

	};

	double vector_angle( CVector3D &v1, CVector3D &v2 );
};

#endif
