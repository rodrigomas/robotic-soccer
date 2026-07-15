#ifndef SHOT_DETECTOR_H
#define SHOT_DETECTOR_H

#include <string>
#include <vector>

#include "vector3d.h"

namespace soccer {

	typedef struct {
		int tick;
		double matchTime;
		std::string team;
		int shooterNumber;
		std::string shooterName;
		CVector3D shooterPos;
		CVector3D ballPos;
		CVector3D ballVel;
		double targetGoalZ;
	} ShotDetectorSample;

	typedef struct {
		int tick;
		double matchTime;
		std::string team;
		int shooterNumber;
		std::string shooterName;
		double shotSpeed;
		double forwardSpeed;
		double goalDistance;
		double targetGoalZ;
		CVector3D shooterPos;
		CVector3D ballPos;
		CVector3D ballVel;
	} ShotEvent;

	typedef struct {
		int totalShots;
		std::string lastShotTeam;
		int lastShooterNumber;
		std::string lastShooterName;
		double lastShotSpeed;
		double lastGoalDistance;
	} ShotSummary;

	class ShotDetector {

		bool activeShot;
		std::string activeTeam;
		double minShotSpeed;
		double minForwardSpeed;
		double resetShotSpeed;
		double maxShootingLaneX;
		std::vector<ShotEvent> events;

		static double speedXZ(const CVector3D &v);

	public:
		ShotDetector();

		void reset(void);
		void setThresholds(double shotSpeed,
				   double forwardSpeed,
				   double resetSpeed,
				   double shootingLaneX);
		bool record(const ShotDetectorSample &sample);
		const std::vector<ShotEvent> &getEvents(void) const;
		int getTeamShotCount(const std::string &teamName) const;
		ShotSummary getSummary(void) const;
		bool writeCsv(const std::string &path) const;
	};

};

#endif
