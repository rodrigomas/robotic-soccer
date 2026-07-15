#ifndef TACTICAL_ADVISOR_H
#define TACTICAL_ADVISOR_H

#include "ball.h"
#include "player.h"

namespace soccer {

	typedef struct {
		CPlayer *carrier;
		CPlayer *target;
		double score;
		double passDistance;
		double targetPressure;
		double targetGoalDistance;
	} TacticalSuggestion;

	class TacticalAdvisor {

		double pressureWeight;
		double passDistanceWeight;
		double goalDistanceWeight;

		static double distanceXZ(const CVector3D &a, const CVector3D &b);
		static double nearestOpponentDistance(CPlayer **players, int nplayers,
						      const CVector3D &pos);
		static CPlayer *nearestPlayerToBall(CPlayer **players, int nplayers,
						    const CBall &ball);

	public:
		TacticalAdvisor();

		void setWeights(double pressure, double passDistance, double goalDistance);
		TacticalSuggestion suggestPass(CPlayer **teamPlayers, int teamCount,
					       CPlayer **opponentPlayers, int opponentCount,
					       const CBall &ball, bool team01,
					       bool secondHalf) const;
	};

};

#endif
