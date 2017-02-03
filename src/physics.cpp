#include "physics.h"

extern int LastCollisionIndex;

namespace soccer {

double VMAX = 30.0;

double gravity = 9.7;
double floorResY = 0.65;
double floorResXZ = 0.97;
double atrito_player = 0.3;
double rest_playerBall = 0.99;
double resti_coef = 0.5f;
double pentol = 0.2f;

double air_viscosidade = 1.83E-5;
double air_density = 1.244;

CVector3D g = CVector3D(0,-gravity,0);
CVector3D wind = CVector3D(0,0,0);

bool magnus = true;
bool drag = true ;
bool usewind = true ;
bool rand_wind = true;

string iter_method = "euler";

void initPhysics( SGameData* gdata )
{

	magnus = gdata->physics.magnus;
	drag = gdata->physics.drag;
	usewind = gdata->physics.wind;
	rand_wind = gdata->physics.rand_wind;

	VMAX = gdata->physics.maxspeed;

	gravity = gdata->physics.gravity;
	floorResY = gdata->physics.floorResY;
	floorResXZ = gdata->physics.floorResXZ;
	atrito_player = gdata->physics.friction_player;
	rest_playerBall = gdata->physics.rest_playerBall;
	resti_coef = gdata->physics.resti_coef;
	pentol = gdata->physics.pentol;

	air_viscosidade = gdata->physics.air_viscosity;
	air_density = gdata->physics.air_density;

	iter_method = gdata->physics.method;

	g = CVector3D(0,-gravity,0);

	wind = CVector3D(gdata->physics.wind_dir[0],gdata->physics.wind_dir[1],gdata->physics.wind_dir[2]);
}

void updatePhysicsWind( void )
{
	if( rand_wind )
		wind = wind * 0.1  + CVector3D( ((rand() - rand())%100)/2000.0 , 0.0, ((rand() - rand())%100)/2000.0) * 0.01;
}

void applyForceBall( CBall *b, double fx, double fy, double fz, double px, double py, double pz )
{

	b->force += CVector3D(fx,fy,fz);

	b->torque += CVector3D(px,py,pz) * CVector3D(fx,fy,fz);

}

void setSpeedBall( CBall *b, double vx, double vy, double vz )
{
	b->vel = CVector3D(vx,vy,vz);
}

/*void applyPhysics4Ball( CBall *b, double elapsed )
{
	// Integração
// 	if( iter_method == "euler" ) {

		b->vel += b->force * b->massInv * elapsed;

		b->pos += b->vel * elapsed;

		b->w = b->w * 0.98;

		b->w += b->torque * b->momentInv * elapsed;

		b->rot += b->w * elapsed;

// 	} else if( iter_method == "runge2" ) {
//
// 		CVector3D dfF = (b->force - b->oldforce) / elapsed;
// 		CVector3D dfT = (b->torque - b->oldtorque) / elapsed;
//
// 		b->vel += b->force * b->massInv * elapsed + dfF * POW2(elapsed) / 2.0;
//
// 		b->pos += b->vel * elapsed + b->vel * POW2(elapsed) / 2.0;
//
// 		b->w = b->w * 0.98;
//
// 		b->w += b->torque * b->momentInv * elapsed + dfT * POW2(elapsed) / 2.0;
//
// 		b->rot += b->w * elapsed + b->w * POW2(elapsed) / 2.0;
//
// 	}

	//b->oldforce = b->force;
	//b->oldtorque = b->torque;

	b->force = CVector3D(0,0,0);

	b->torque = CVector3D(0,0,0);

	double Re, Ca; // Reinolds, Arrasto
	double vel = b->vel.abs();

	Re = 2 * ( air_density / air_viscosidade ) * b->r * vel;

	if( Re > 24 && b->pos.y > 10.0 ) {

		if( Re < 100 ) {
			Ca = 24.0 / Re;
		} else if ( Re < 3E5 ) {
			Ca = 0.24;
		} else {
			Ca = 0.1;
		}

		double k = 0.5 * air_density * M_PI * POW2(b->r);

		//b->force += b->vel * ( - Ca * 1 / 2.0 * air_density * M_PI * POW2(b->r) * vel );
		if( drag )
			b->force += b->vel * ( - Ca * k * vel ); // Arrasto

		if( magnus )
			b->force += ( b->w * b->vel ) * ( - k * b->r );

	}

	if( usewind && b->pos.y > 10.0 ) {
		b->force += wind;
	}

	if ( b->pos.y > b->r ) {

		b->force += g * b->mass;

	} else if ( b->pos.y <= b->r ) { // Colisão com o chao

		b->pos.y = b->r;
		b->vel.y = -(  floorResY ) * b->vel.y;

		b->torque = CVector3D(0,-b->r,0) * ( CVector3D(-b->vel.x, 0, -b->vel.z) * floorResXZ );

		b->force += CVector3D(-b->vel.x, 0, -b->vel.z) * floorResXZ;

		#ifdef USE_SOUND
		if ( b->vel.y > 1.0 ) {
			setSourcePos(CHUTE, b->pos.x,b->pos.y,b->pos.z);
			setSourceVel(CHUTE, b->vel.x,b->vel.y,b->vel.z);
			playSound(CHUTE);
		}
		#endif

	}
}*/

void applyPhysics4Ball( CBall *b, double elapsed )
{
	// Integracao


	double Re, Ca; // Reinolds, Arrasto
	double vel = b->vel.abs();
	bool colide = false;

	Re = 2 * ( air_density / air_viscosidade ) * b->r * vel;

	CVector3D kv[4], kw[4], V, W;
	int cnt = 0;
	int divf[3][4] = { {1,1,1,1}, {1,1,1,1}, {2,2,1,1} };

    if ( iter_method == "euler" ) {
        cnt = 1;
    } else if ( iter_method == "runge2" ) {
        cnt = 2;
    } else if ( iter_method == "runge4" ) {
        cnt = 4;
    }

    V = b->vel;
    W = b->w;


    for ( register int i = 0; i < cnt ; i++ ) {

        vel = V.abs();

        kv[i] = b->force;
        kw[i] = b->torque;

        if( Re > 24 && b->pos.y > 10.0 ) {

                if( Re < 100 ) {
                    Ca = 24.0 / Re;
                } else if ( Re < 3E5 ) {
                    Ca = 0.24;
                } else {
                    Ca = 0.1;
                }

                double k = 0.5 * air_density * M_PI * POW2(b->r);

                if( drag ) {
                    kv[i] += V * ( - Ca * k * vel ); // Arrasto
                }

                if( magnus ) {
                    kv[i] += ( W * V ) * ( - k * b->r );
                }
        }

        if( usewind && b->pos.y > 10.0 ) {
            kv[i] += wind;
        }

        if ( b->pos.y > b->r ) {

            kv[i] += g * b->mass;

        } else if ( b->pos.y <= b->r ) {

            V.y = -(  floorResY ) * V.y;

            kw[i] += CVector3D(0,-b->r,0) * ( CVector3D(-V.x, 0, -V.z) * floorResXZ ); // ???

            kv[i] += CVector3D(-V.x, 0, -V.z) * floorResXZ;

            colide = true;

        }

        kw[i] += W * -0.1;

        kv[i] = kv[i] * b->massInv * elapsed;
        kw[i] = kw[i] * b->momentInv * elapsed;

        V = b->vel + kv[i] / (double)divf[cnt-1][i] ;
        W = b->w + kw[i] / (double)divf[cnt-1][i];
    }

    if ( iter_method == "euler" ) {

        b->vel = b->vel + kv[0];
        b->w = b->w + kw[0];

    } else if ( iter_method == "runge2" ) {

        b->vel = b->vel + (kv[0] + kv[1]) * 0.5;
        b->w = b->w + (kw[0] + kw[1]) * 0.5;

    } else if ( iter_method == "runge4" ) {

        b->vel = b->vel + (kv[0] + kv[1] * 2 + kv[2] * 2 + kv[3]) / 6.0;
        b->w = b->w + (kw[0] + kw[1] * 2 + kw[2] * 2 + kw[3]) / 6.0;

    }

    if ( b->pos.y <= b->r ) { // Colisão com o chao

		b->pos.y = b->r;
		b->vel.y = -(  floorResY ) * b->vel.y;

		b->w += CVector3D(0,-b->r,0) * ( CVector3D(-b->vel.x, 0, -b->vel.z) * 2 * floorResXZ ) * elapsed;
	}


	b->pos += b->vel * elapsed;

    b->rot += b->w * elapsed;

	b->force = CVector3D(0,0,0);

	b->torque = CVector3D(0,0,0);

	if ( colide ) {
		#ifdef USE_SOUND
		if ( b->vel.y > 1.0 ) {
			setSourcePos(CHUTE, b->pos.x,b->pos.y,b->pos.z);
			setSourceVel(CHUTE, b->vel.x,b->vel.y,b->vel.z);
			playSound(CHUTE);
		}
		#endif
	}
}

int testPhysicsCollision4Player( CPlayer *p1, CPlayer *p2, CSoccerField *f, double elapsed, bool team1 )
{

	if( p1->ncards >= 2 || p1->ncards >= 2  )
		return 0;

	CVector3D dist = p1->pos - p2->pos;
	dist.y = 0.0;

	double r = p1->r + p2->r;
	double j;
	CVector3D vrel = p1->vel - p2->vel;

	double s = dist.abs() - r;

	dist.normalize();

	double vrn;

	vrn = vrel | dist;

	bool isColide = false;
	// Cilindro Cilindro ( colisao com perda, usando a restituicao )

	if ( fabs(s) <= pentol && ( vrn < 0.0 ) ) {

		j = ( -(1 + resti_coef) * (vrn) ) / ( (dist | dist) * ( p1->massInv + p2->massInv));

		p1->vel += ( dist * j ) * p1->massInv;
		p2->vel -= ( dist * j ) * p2->massInv;

		isColide = true;
	} else {
		int nIterations = 10;
		while( s < -pentol && nIterations != 0 ) {

			j = ( -(1 + resti_coef) * (vrn) ) / ( (dist | dist) * ( p1->massInv + p2->massInv));

			p1->vel += ( dist * j ) * p1->massInv;
			p2->vel -= ( dist * j ) * p2->massInv;

			applyPhysics4Player( p1, f, elapsed / 2.0);
			applyPhysics4Player( p2, f, elapsed / 2.0);

			dist = p1->pos - p2->pos; dist.y = 0.0;

			s = dist.abs() - r;
			dist.normalize();
			vrn = vrel | dist;
			vrel = p1->vel - p2->vel;

			isColide = true;

			nIterations--;
		}
	}

	if ( isColide )
	{
		#ifdef USE_SOUND
		setSourcePos(COLISAO_JxJ, p1->pos.x,p1->pos.y,p1->pos.z);
		setSourceVel(COLISAO_JxJ, p1->vel.x,p1->vel.y,p1->vel.z);
		playSound(COLISAO_JxJ);

		setSourcePos(COLISAO_JxJ, p2->pos.x,p2->pos.y,p2->pos.z);
		setSourceVel(COLISAO_JxJ, p2->vel.x,p2->vel.y,p2->vel.z);
		playSound(COLISAO_JxJ);
		#endif

		if( p1->team == p2->team ) {
			return 0;
		}


		int pfalta = (int)(( 2 * vrel.abs() / ( VMAX ) ) * (double)((rand() % 100)));

		if ( pfalta > 60 ) {

			if ( pfalta > 92 ) {
				return 2;
			}

			if ( pfalta > 76 ) {
				return 1;
			}

			return 3;
		}
	}

	return 0;
}

void testPhysicsCollision4Ball( CPlayer *p, CBall *b, bool *team01 )
{

	if( p->ncards >= 2 )
		return;

	CVector3D dist = b->pos - p->pos;

	dist.y = 0.0;
	// Esfera Cilindro
	if( b->pos.y < p->h && dist.abs() <= b->r + p->r) {
		// Conservação da Quantidade de Movimento
		if( p->vel.x == 0 ) {
			b->vel.x = -b->vel.x;
		} else {
			b->vel.x += b->massInv * p->mass * (1 - rest_playerBall) * p->vel.x;
		}

		if( p->vel.z == 0 ) {
			b->vel.z = -b->vel.z;
		} else {
			b->vel.z += b->massInv * p->mass * (1 - rest_playerBall) * p->vel.z;
		}

		*team01 = ( p->team == 0 );

		#ifdef USE_SOUND
		setSourcePos(CHUTE, b->pos.x,b->pos.y,b->pos.z);
		setSourceVel(CHUTE, b->vel.x,b->vel.y,b->vel.z);
		playSound(CHUTE);
		#endif

		LastCollisionIndex = p->num;
	}

}

void applyPhysics4Player( CPlayer *p, CSoccerField *f, double elapsed )
{

	if( p->ncards >= 2 )
		return;

 	if( iter_method == "euler" || iter_method == "runge2" || iter_method == "runge4") {
		p->vel += p->force * p->massInv * elapsed;

		if ( p->vel.abs() > VMAX ) {
			p->vel = p->vel.normalize();
			p->vel *= VMAX;
		}

		p->pos += p->vel * elapsed;
 	} else {
        return;
 	}

	p->force = CVector3D(0,0,0);
    int dir = 1;
	if ( fabs(p->pos.x) >= f->width / 2.0 -1.0 ) {
		//p->vel.x = -p->vel.x;
		dir = ( p->vel.x > 0 ) ? (1):(-1);
		p->vel = CVector3D(0,0,0);
		p->vel.x = -dir;
	}

	if ( fabs(p->pos.z) >= f->lenght / 2.0 -1.0 ) {
		//p->vel.z = -p->vel.z;
		dir = ( p->vel.z > 0 ) ? (1):(-1);
		p->vel = CVector3D(0,0,0);
		p->vel.z = -dir;
		//p->vel.z = 0;
	}

	if ( p->vel.abs() != 0 ) {
	// Atrito Chao
		p->force += p->vel.normal() * (atrito_player * p->mass * gravity * -1);
	}

}

void applyPhysics4FlagMan( CFlagMan *p, CSoccerField *f, double elapsed )
{

	if( iter_method == "euler" || iter_method == "runge2" || iter_method == "runge4") {

		p->vel += p->force * p->massInv * elapsed;

		if ( p->vel.abs() > VMAX ) {
			p->vel = p->vel.normalize();
			p->vel *= VMAX;
		}

		p->pos += p->vel * elapsed;

	} else {
        return;
	}

	p->force = CVector3D(0,0,0);

	if ( fabs(p->pos.x) >= f->width / 2.0 -1.0 ) {
		p->vel.x = -p->vel.x;
	}

	if ( fabs(p->pos.z) >= f->lenght / 2.0 -1.0 ) {
		p->vel.z = -p->vel.z;
	}

	if ( p->vel.abs() != 0 ) {
	// Atrito Chao
		p->force += p->vel.normal() * (atrito_player * p->mass * gravity * -1);
	}

}

void playerClickHL( CPlayer *p, CBall *b, int k)
{
	if( p->ncards >= 2 )
		return;

	CVector3D dist = b->pos - p->pos;

	CVector3D f;

	dist.y = 0.0;
	// Esfera Cilindro
	if( b->pos.y < p->h && dist.abs() <= b->r + p->r + 2.0) {

		f = (p->front.normal()) * ( k + p->vel.abs() / 10.0 );

		applyForceBall( b, f.x, 0, f.z, 0, 0, 0);
	}
}

void playerClickHLA( CPlayer *p, CBall *b, int k)
{
	if( p->ncards >= 2 )
		return;

	CVector3D dist = b->pos - p->pos;

	CVector3D f;

	dist.y = 0.0;
	// Esfera Cilindro
	if( b->pos.y < 5.0 * b->r && dist.abs() <= b->r + p->r + 2.0) {

		f = (p->front.normal()) * ( k + p->vel.abs() / 10.0 );

		f.y = ( 60.0  + ( k + p->vel.abs()) / 10.0 );

		applyForceBall( b, f.x, f.y, f.z, 0, 0, 0);
	}
}

int setNewCurrPlayer( CPlayer **ps, int n, CBall *b )
{

	CVector3D d;

	double MinDist = 100000.0, SelDist = 0;

	int Min = 0, Sel = 0;

	for( register int i = 0 ; i < n ; i++ ) {

		d = ps[i]->pos - b->pos;

		if( d.abs() < MinDist && ps[i]->ncards < 2 ) {
			Min = i;
			MinDist = d.abs();
		}

		if( ps[i]->isSelected ) {
			SelDist = d.abs();
			Sel = i;
		}
 	}

	if( MinDist < SelDist - 10 || ps[Sel]->ncards >= 2 )
		return Min;
	else
		return Sel;
}

CPlayer *nearestPlayer( CPlayer **ps, int n, CVector3D pos )
{

	CVector3D d;

	double MinDist = 100000.0;

	int Min = 0;

	for( register int i = 0 ; i < n ; i++ ) {

		if( ps[i]->ncards >= 2 )
			continue;

		d = ps[i]->pos - pos;

		if( d.abs() < MinDist ) {
			Min = i;
			MinDist = d.abs();
		}
 	}

	return ps[Min];
}

void applyForcePlayer( CPlayer *p, CVector3D f )
{
	if( p->ncards >= 2 )
		return;

	p->force += CVector3D(f.x,f.y,f.z);

}

void kickForce(  CPlayer *p, CBall *b, double fx, double fy, double fz )
{
	if( p->ncards >= 2 )
		return;

	CVector3D dist = b->pos - p->pos;

	dist.y = 0.0;
	// Esfera Cilindro
	if( b->pos.y < 5.0 * b->r && dist.abs() <= b->r + p->r + 2.0) {

		applyForceBall( b, fx, fy, fz, 0, 0, 0);
	}
}

void kickToVel(  CPlayer *p, CBall *b, double px, double pz, double vel )
{
	if( p->ncards >= 2 )
		return;

	CVector3D dist = b->pos - p->pos;
	CVector3D V;

	dist.y = 0.0;
	// Esfera Cilindro
	if( b->pos.y < 5.0 * b->r && dist.abs() <= b->r + p->r + 2.0) {

		dist = CVector3D(px,0,pz) - p->pos;
		dist.y = 0.0;

		V = dist.normal() * sqrt(vel);

		V.y = dist.abs() * 0.5 * gravity / vel ;

		setSpeedBall( b, V.x, V.y, V.z);

	}
}

void passTo(  CPlayer *p, CBall *b, double px, double pz)
{
	if( p->ncards >= 2 )
		return;

	CVector3D dist = b->pos - p->pos;
	CVector3D V;

	dist.y = 0.0;
	// Esfera Cilindro
	if( b->pos.y < 5.0 * b->r && dist.abs() <= b->r + p->r + 2.0) {

		dist = CVector3D(px,0,pz) - p->pos;
		dist.y = 0.0;

		double K, a;

		a = b->mass * gravity * floorResXZ;

		K = sqrt( 2.0f * a * dist.abs() );

		V = dist.normal() * K;

		setSpeedBall( b, V.x, 0, V.z);
	}
}

};
