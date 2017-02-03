#include "rules.h"

namespace soccer {

ERuleApplyed applySoccerRules4Faults( CBall *b, bool team01, bool posTeam01, CVector3D &pos, CPlayer *p1, CPlayer *p2 )
{

	double cte = ( b->r + p1->r + 1.0 );

	if ( team01 ) {

		if( p1->pos.x > -20.15 && p1->pos.x < 20.15 ) {
			if( posTeam01 && ( p1->pos.z > -60.0 && p1->pos.z < -43.5 ) ) {

				b->pos.x = 0;
				b->pos.z = -49.0;
				b->pos.y = b->r;
				b->zero();
		
				pos.x = 0;
				pos.z = b->pos.z + cte;
				
				return ePenalty;
			} else if( !posTeam01 && ( p1->pos.z < 60.0 && p1->pos.z > 43.5 ) ) {

				b->pos.x = 0;
				b->pos.z = 49.0;
				b->pos.y = b->r;
				b->zero();
		
				pos.x = 0;
				pos.z = b->pos.z - cte;

				return ePenalty;
			}			
		}

		b->pos.x = p1->pos.x;
		b->pos.z = p1->pos.z;
		b->pos.y = b->r;
		b->zero();

		pos.x = b->pos.x;		
		
		if( posTeam01 ) {
			pos.z = b->pos.z + cte;
		} else {
			pos.z = b->pos.z - cte;
		}

	} else {

		if( p2->pos.x > -20.15 && p2->pos.x < 20.15 ) {
			if( !posTeam01 && ( p2->pos.z > -60.0 && p2->pos.z < -43.5 ) ) {

				b->pos.x = 0;
				b->pos.z = -49.0;
				b->pos.y = b->r;
				b->zero();
		
				pos.x = 0;
				pos.z = b->pos.z + cte;

				return ePenalty;
			} else if( posTeam01 && ( p2->pos.z < 60.0 && p2->pos.z > 43.5 ) ) {

				b->pos.x = 0;
				b->pos.z = 49.0;
				b->pos.y = b->r;
				b->zero();
		
				pos.x = 0;
				pos.z = b->pos.z - cte;

				return ePenalty;
			}			
		}

		b->pos.x = p2->pos.x;
		b->pos.z = p2->pos.z;
		b->pos.y = b->r;
		b->zero();

		pos.x = b->pos.x;
		
		if( !posTeam01 ) {
			pos.z = b->pos.z + cte;
		} else {
			pos.z = b->pos.z - cte;
		}
	}

	return eFalta;
}

ERuleApplyed applySoccerRules4Ball( CBall *b, CSoccerField *f, bool team01, bool posTeam01, CVector3D &pos, double playerR )
{	

	if( ruleState != esNone )
		return eNone;

	double cte = ( b->r + playerR+ 1.0 );

	//Gol
	if( b->pos.z > f->fieldlenght / 2.0 && b->pos.y < f->goalHeight -0.2 ) {
		if ( b->pos.x < f->goalWidth2 && b->pos.x > -f->goalWidth2 ) {
			b->pos.x = 0;
			b->pos.z = 0;
			b->pos.y = b->r;
			b->zero();
			#ifdef USE_SOUND
			playSound(GOL);
			#endif
			return eGolPos;
		}
	}

	if( b->pos.z < -f->fieldlenght / 2.0 && b->pos.y < f->goalHeight -0.2 ) {
		if ( b->pos.x < f->goalWidth2 && b->pos.x > -f->goalWidth2 ) {
			b->pos.x = 0;
			b->pos.z = 0;
			b->pos.y = b->r;
			b->zero();
			#ifdef USE_SOUND
			playSound(GOL);
			#endif
			return eGolNeg;
		}
	}

	// Laterais e Escanteio
	if( b->pos.x > f->fieldwidth / 2.0  ) {
		if( b->pos.z > f->fieldlenght / 2.0 ) {
			
			if( !(( team01 && posTeam01 ) || ( !team01 && !posTeam01 )) ) {

				// Tiro de meta
				b->pos.x = 0;
				b->pos.z = 49;
				b->pos.y = b->r;

				pos.x = 0;
				pos.z = b->pos.z + cte;

				b->zero();
				#ifdef USE_SOUND
				playSound(APITO);
				#endif

				return eTiroDeMeta;
			} 

			// Escanteio			

			b->pos.x = f->fieldwidth / 2.0;
			b->pos.z = f->fieldlenght / 2.0;
			b->pos.y = b->r;

			pos.x = b->pos.x + cte;
			pos.z = b->pos.z + cte;

			b->zero();
			#ifdef USE_SOUND
			playSound(APITO);
			#endif
			ruleState = esEscanteio;

			return eEscanteio;
		} else if( b->pos.z < -f->fieldlenght / 2.0 ) {

			if( !(( team01 && !posTeam01 ) || ( !team01 && posTeam01 )) ) {

				// Tiro de meta
				b->pos.x = 0;
				b->pos.z = -49;
				b->pos.y = b->r;

				pos.x = 0;
				pos.z = b->pos.z - cte;

				b->zero();
				#ifdef USE_SOUND
				playSound(APITO);
				#endif

				return eTiroDeMeta;
			} 

			// Escanteio

			b->pos.x = f->fieldwidth / 2.0;
			b->pos.z = -f->fieldlenght / 2.0;
			b->pos.y = b->r;

			pos.x = b->pos.x + cte;
			pos.z = b->pos.z - cte;

			b->zero();
			#ifdef USE_SOUND
			playSound(APITO);
			#endif
			ruleState = esEscanteio;
			return eEscanteio;
		}
		
		// Lateral
		b->pos.x = f->fieldwidth / 2.0;
		b->pos.y = b->r;

		pos.x = b->pos.x + cte;
		pos.z = b->pos.z;

		b->zero();
		#ifdef USE_SOUND
		playSound(APITO);
		#endif
		ruleState = esLateral;
		return eLateral;
	}

	// Laterais e Escanteio
	if( b->pos.x < - f->fieldwidth / 2.0  ) {

		if( b->pos.z > f->fieldlenght / 2.0 ) {

			if( !(( team01 && posTeam01 ) || ( !team01 && !posTeam01 )) ) {

				// Tiro de meta
				b->pos.x = 0;
				b->pos.z = 49;
				b->pos.y = b->r;

				pos.x = 0;
				pos.z = b->pos.z + cte;

				b->zero();
				#ifdef USE_SOUND
				playSound(APITO);
				#endif

				return eTiroDeMeta;
			} 

			// Escanteio

			b->pos.x = - f->fieldwidth / 2.0;
			b->pos.z = f->fieldlenght / 2.0;
			b->pos.y = b->r;

			pos.x = b->pos.x - cte;
			pos.z = b->pos.z + cte;

			b->zero();
			#ifdef USE_SOUND
			playSound(APITO);
			#endif
			ruleState = esEscanteio;
			return eEscanteio;
		} else if( b->pos.z < -f->fieldlenght / 2.0 ) {

			if( !(( team01 && !posTeam01 ) || ( !team01 && posTeam01 )) ) {

				// Tiro de meta
				b->pos.x = 0;
				b->pos.z = -49;
				b->pos.y = b->r;

				pos.x = 0;
				pos.z = b->pos.z - cte;

				b->zero();
				#ifdef USE_SOUND
				playSound(APITO);
				#endif

				return eTiroDeMeta;
			} 

			// Escanteio

			b->pos.x = -f->fieldwidth / 2.0;
			b->pos.z = -f->fieldlenght / 2.0;
			b->pos.y = b->r;

			pos.x = b->pos.x - cte;
			pos.z = b->pos.z - cte;
			
			b->zero();
			#ifdef USE_SOUND
			playSound(APITO);
			#endif
			ruleState = esEscanteio;
			return eEscanteio;
		}
		
		// Lateral

		b->pos.x = -f->fieldwidth / 2.0;
		b->pos.y = b->r;

		pos.x = b->pos.x - cte;
		pos.z = b->pos.z;

		#ifdef USE_SOUND
		playSound(APITO);
		#endif
		b->zero();
		ruleState = esLateral;
		return eLateral;
	}

	if( b->pos.z > f->fieldlenght / 2.0 ) {

		if( !(( team01 && posTeam01 ) || ( !team01 && !posTeam01 )) ) {

			// Tiro de meta
			b->pos.x = 0;
			b->pos.z = 49;
			b->pos.y = b->r;

			pos.x = 0;
			pos.z = b->pos.z + cte;

			b->zero();
			#ifdef USE_SOUND
			playSound(APITO);
			#endif

			return eTiroDeMeta;
		} 

		// Escanteio

		if( b->pos.x >= 0 ) {
			b->pos.x = f->fieldwidth / 2.0;

			pos.x = b->pos.x + cte;
		} else {
			b->pos.x = - f->fieldwidth / 2.0;

			pos.x = b->pos.x - cte;
		}		

		b->pos.z = f->fieldlenght / 2.0;
		b->pos.y = b->r;
		
		pos.z = b->pos.z + cte;

		b->zero();
		#ifdef USE_SOUND
		playSound(APITO);
		#endif
		ruleState = esEscanteio;
		return eEscanteio;
	} else if( b->pos.z < -f->fieldlenght / 2.0 ) {

		if( !(( team01 && !posTeam01 ) || ( !team01 && posTeam01 )) ) {

			// Tiro de meta
			b->pos.x = 0;
			b->pos.z = -49;
			b->pos.y = b->r;

			pos.x = 0;
			pos.z = b->pos.z - cte;

			b->zero();
			#ifdef USE_SOUND
			playSound(APITO);
			#endif

			return eTiroDeMeta;
		} 

		// Escanteio

		if( b->pos.x >= 0 ) {
			b->pos.x = f->fieldwidth / 2.0;
			pos.x = b->pos.x + cte;
		} else {
			b->pos.x = - f->fieldwidth / 2.0;
			pos.x = b->pos.x - cte;
		}		

		b->pos.z = -f->fieldlenght / 2.0;
		b->pos.y = b->r;
		
		pos.z = b->pos.z - cte;

		b->zero();
		#ifdef USE_SOUND
		playSound(APITO);
		#endif
		ruleState = esEscanteio;
		return eEscanteio;
	}

	return eNone;
}

};
