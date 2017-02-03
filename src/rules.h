#ifndef RULES_H
#define RULES_H

#include "ball.h"
#include "soccerfield.h"
#include "player.h"
#include "common.h"
#include "vector3d.h"
#ifdef USE_SOUND
#include "sound.h"
#endif

namespace soccer {

typedef enum {
	eNone = 0,
	eCartaoAmar,
	eCartaoVerm,
	eFalta,
	eLateral,
	eEscanteio,
	eGolPos,
	eGolNeg,
	ePenalty,
	eTiroDeMeta	

} ERuleApplyed;

typedef enum {
	esNone,
	esLateral,
	esEscanteio,
	esFalta,
	esPenalty	

} ERuleState;

extern ERuleState ruleState;

ERuleApplyed applySoccerRules4Faults( CBall *b, bool team01, bool posTeam01, CVector3D &pos, CPlayer *p1, CPlayer *p2 );

ERuleApplyed applySoccerRules4Ball( CBall *b, CSoccerField *f, bool team01, bool posTeam01, CVector3D &pos, double playerR );

};
#endif
