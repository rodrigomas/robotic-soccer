#ifndef PASS_LANE_TRACKER_H
#define PASS_LANE_TRACKER_H

#include <string>
#include <vector>

#include "vector3d.h"

namespace soccer {

	typedef struct {
		std::string team;
		int number;
		std::string name;
		CVector3D pos;
	} PassLanePlayer;

	typedef struct {
		int tick;
		double matchTime;
		std::string possessionTeam;
		int carrierNumber;
		std::string carrierName;
		CVector3D carrierPos;
		CVector3D ballPos;
		double targetGoalZ;
		std::vector<PassLanePlayer> teammates;
		std::vector<PassLanePlayer> opponents;
	} PassLaneSample;

	typedef struct {
		int tick;
		double matchTime;
		std::string possessionTeam;
		int carrierNumber;
		std::string carrierName;
		int rank;
		int targetNumber;
		std::string targetName;
		double score;
		double passDistance;
		double targetPressure;
		double targetGoalDistance;
		double forwardProgress;
		CVector3D carrierPos;
		CVector3D targetPos;
		CVector3D ballPos;
	} PassLaneOption;

	typedef struct {
		int samples;
		int options;
		int lastOptionCount;
		int lastBestTargetNumber;
		std::string lastBestTargetName;
		double lastBestScore;
		double lastBestPassDistance;
	} PassLaneSummary;

	class PassLaneTracker {

		int maxOptionsPerSample;
		double pressureWeight;
		double passDistanceWeight;
		double goalDistanceWeight;
		std::vector<PassLaneOption> options;

		static double distanceXZ(const CVector3D &a, const CVector3D &b);
		static void insertRanked(std::vector<PassLaneOption> *ranked,
					 const PassLaneOption &option,
					 int maxOptions);
		double nearestOpponentDistance(const PassLaneSample &sample,
					       const CVector3D &pos) const;

	public:
		PassLaneTracker();

		void reset(void);
		void setMaxOptionsPerSample(int maxOptions);
		void setWeights(double pressure, double passDistance, double goalDistance);
		bool record(const PassLaneSample &sample);
		const std::vector<PassLaneOption> &getOptions(void) const;
		PassLaneSummary getSummary(void) const;
		bool writeCsv(const std::string &path) const;
	};

};

#endif
