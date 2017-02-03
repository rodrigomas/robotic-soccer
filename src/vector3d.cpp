#include "vector3d.h"

namespace soccer {	
	double vector_angle( CVector3D &v1, CVector3D &v2 ) 
	{

		if( v2.abs() == 0 || v1.abs() == 0 )
			return 0;

		double result = (v1 | v2) / ( (v1.abs()) * (v2.abs()) );

		result = TO_DEGREES(acos( result ));
		return result;
	}
}
