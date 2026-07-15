#include "tactical_advisor.h"

#include <cmath>

namespace soccer {

	TacticalAdvisor::TacticalAdvisor() :
		pressureWeight(0.65),
		passDistanceWeight(0.35),
		goalDistanceWeight(0.20)
	{
	}

	void TacticalAdvisor::setWeights(double pressure,
					 double passDistance,
					 double goalDistance)
	{
		pressureWeight = pressure;
		passDistanceWeight = passDistance;
		goalDistanceWeight = goalDistance;
	}

	double TacticalAdvisor::distanceXZ(const CVector3D &a, const CVector3D &b)
	{
		double dx = a.x - b.x;
		double dz = a.z - b.z;
		return std::sqrt(dx * dx + dz * dz);
	}

	double TacticalAdvisor::nearestOpponentDistance(CPlayer **players,
							int nplayers,
							const CVector3D &pos)
	{
		double bestDistance = 1000000.0;

		for(register int i = 0; i < nplayers; i++) {
			if(players[i]->ncards >= 2) {
				continue;
			}

			double d = distanceXZ(players[i]->pos, pos);
			if(d < bestDistance) {
				bestDistance = d;
			}
		}

		return bestDistance;
	}

	CPlayer *TacticalAdvisor::nearestPlayerToBall(CPlayer **players,
						      int nplayers,
						      const CBall &ball)
	{
		CPlayer *best = NULL;
		double bestDistance = 1000000.0;

		for(register int i = 0; i < nplayers; i++) {
			if(players[i]->ncards >= 2) {
				continue;
			}

			double d = distanceXZ(players[i]->pos, ball.pos);
			if(d < bestDistance) {
				best = players[i];
				bestDistance = d;
			}
		}

		return best;
	}

	void TacticalAdvisor::insertRankedOption(TacticalSuggestion *suggestion,
						 const TacticalPassOption &option)
	{
		int optionCount = suggestion->optionCount;
		if(optionCount == TACTICAL_MAX_OPTIONS &&
		   option.score <= suggestion->options[optionCount - 1].score) {
			return;
		}

		if(optionCount < TACTICAL_MAX_OPTIONS) {
			optionCount++;
			suggestion->optionCount = optionCount;
		}

		int index = optionCount - 1;
		while(index > 0 && suggestion->options[index - 1].score < option.score) {
			suggestion->options[index] = suggestion->options[index - 1];
			index--;
		}

		suggestion->options[index] = option;
	}

	TacticalSuggestion TacticalAdvisor::suggestPass(CPlayer **teamPlayers,
							int teamCount,
							CPlayer **opponentPlayers,
							int opponentCount,
							const CBall &ball,
							bool team01,
							bool secondHalf) const
	{
		TacticalSuggestion suggestion;
		suggestion.carrier = nearestPlayerToBall(teamPlayers, teamCount, ball);
		suggestion.target = NULL;
		suggestion.score = -1000000.0;
		suggestion.passDistance = 0.0;
		suggestion.targetPressure = 0.0;
		suggestion.targetGoalDistance = 0.0;
		suggestion.optionCount = 0;

		if(!suggestion.carrier) {
			return suggestion;
		}

		double goalZ = team01 ? (secondHalf ? 60.0 : -60.0)
				      : (secondHalf ? -60.0 : 60.0);
		CVector3D goal(0, 0, goalZ);

		for(register int i = 0; i < teamCount; i++) {
			CPlayer *candidate = teamPlayers[i];

			if(candidate == suggestion.carrier || candidate->ncards >= 2) {
				continue;
			}

			double passDistance = distanceXZ(suggestion.carrier->pos, candidate->pos);
			double goalDistance = distanceXZ(candidate->pos, goal);
			double pressure = nearestOpponentDistance(opponentPlayers, opponentCount, candidate->pos);
			double score = pressure * pressureWeight -
				passDistance * passDistanceWeight -
				goalDistance * goalDistanceWeight;
			TacticalPassOption option;
			option.target = candidate;
			option.score = score;
			option.passDistance = passDistance;
			option.targetPressure = pressure;
			option.targetGoalDistance = goalDistance;

			insertRankedOption(&suggestion, option);

			if(score > suggestion.score) {
				suggestion.target = candidate;
				suggestion.score = score;
				suggestion.passDistance = passDistance;
				suggestion.targetPressure = pressure;
				suggestion.targetGoalDistance = goalDistance;
			}
		}

		return suggestion;
	}

};
