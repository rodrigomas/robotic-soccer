#ifndef PRESSURE_TRACKER_H
#define PRESSURE_TRACKER_H

#include <string>
#include <vector>

#include "vector3d.h"

namespace soccer {

	typedef struct {
		std::string team;
		int number;
		std::string name;
		CVector3D pos;
	} PressureOpponent;

	typedef struct {
		int tick;
		double matchTime;
		std::string possessionTeam;
		int carrierNumber;
		std::string carrierName;
		CVector3D carrierPos;
		std::vector<PressureOpponent> opponents;
	} PressureTrackerSample;

	typedef struct {
		int tick;
		double matchTime;
		std::string possessionTeam;
		int carrierNumber;
		std::string carrierName;
		std::string opponentTeam;
		int opponentNumber;
		std::string opponentName;
		double pressureDistance;
		bool highPressure;
		CVector3D carrierPos;
		CVector3D opponentPos;
	} PressureFrame;

	typedef struct {
		int samples;
		int highPressureSamples;
		double averagePressureDistance;
		double minPressureDistance;
		double lastPressureDistance;
		std::string lastPossessionTeam;
		int lastCarrierNumber;
		std::string lastOpponentTeam;
		int lastOpponentNumber;
	} PressureSummary;

	class PressureTracker {

		double highPressureDistance;
		std::vector<PressureFrame> frames;

		static double distanceXZ(const CVector3D &a, const CVector3D &b);

	public:
		PressureTracker();

		void reset(void);
		void setHighPressureDistance(double distance);
		bool record(const PressureTrackerSample &sample);
		const std::vector<PressureFrame> &getFrames(void) const;
		PressureSummary getSummary(void) const;
		bool writeCsv(const std::string &path) const;
	};

};

#endif
