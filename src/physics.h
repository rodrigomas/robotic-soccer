#ifndef PHYSICS_H
#define PHYSICS_H

#include "ball.h"
#include "player.h"
#include "flagman.h"
#include "common.h"
#include "vector3d.h"
#include "gamedata.h"

#ifdef USE_SOUND
#include "sound.h"
#endif

#include <stdlib.h>

namespace soccer {

class CPlayer;

void initPhysics( SGameData* gdata );

void updatePhysicsWind( void );

void applyForceBall( CBall *b, double fx, double fy, double fz, double px, double py, double pz );

void applyPhysics4Ball( CBall *b, double elapsed );

int testPhysicsCollision4Player( CPlayer *p1, CPlayer *p2, CSoccerField *f, double elapsed, bool team1 );

void testPhysicsCollision4Ball( CPlayer *p, CBall *b, bool *team01 );

void applyPhysics4Player( CPlayer *p, CSoccerField *f, double elapsed );

void applyPhysics4FlagMan( CFlagMan *p, CSoccerField *f, double elapsed );

int setNewCurrPlayer( CPlayer **ps, int n, CBall *b );

CPlayer *nearestPlayer( CPlayer **ps, int n, CVector3D pos );

void applyForcePlayer( CPlayer *p, CVector3D f );

void playerClickHL( CPlayer *p, CBall *b, int k);

void playerClickHLA( CPlayer *p, CBall *b, int k);

void kickForce(  CPlayer *p,  CBall *b, double fx, double fy, double fz );

void kickToVel(  CPlayer *p,  CBall *b, double px, double pz, double vel );

void passTo(  CPlayer *p,  CBall *b, double px, double pz);

};
#endif
