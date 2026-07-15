#ifndef COLLISION_DETECTOR_H
#define COLLISION_DETECTOR_H

#include <set>
#include <string>
#include <vector>

#include "vector3d.h"

namespace soccer {

	typedef struct {
		std::string team;
		int number;
		std::string name;
		CVector3D pos;
		CVector3D vel;
		double radius;
	} CollisionParticipant;

	typedef struct {
		int tick;
		double matchTime;
		CVector3D ballPos;
		CVector3D ballVel;
		double ballRadius;
		std::vector<CollisionParticipant> players;
	} CollisionDetectorSample;

	typedef struct {
		int tick;
		double matchTime;
		std::string eventType;
		std::string teamA;
		int numberA;
		std::string nameA;
		std::string teamB;
		int numberB;
		std::string nameB;
		double distance;
		double relativeSpeed;
		CVector3D posA;
		CVector3D posB;
	} CollisionEvent;

	typedef struct {
		int totalCollisions;
		int playerPlayerCollisions;
		int playerBallCollisions;
		std::string lastEventType;
		std::string lastTeamA;
		int lastNumberA;
		std::string lastTeamB;
		int lastNumberB;
		double lastRelativeSpeed;
	} CollisionSummary;

	class CollisionDetector {

		double contactTolerance;
		std::set<std::string> activeContacts;
		std::vector<CollisionEvent> events;

		static double distanceXZ(const CVector3D &a, const CVector3D &b);
		static double relativeSpeedXZ(const CVector3D &a, const CVector3D &b);
		static std::string playerKey(const CollisionParticipant &player);
		static std::string pairKey(const CollisionParticipant &a,
					   const CollisionParticipant &b);
		static std::string ballKey(const CollisionParticipant &player);

	public:
		CollisionDetector();

		void reset(void);
		void setContactTolerance(double tolerance);
		bool record(const CollisionDetectorSample &sample);
		const std::vector<CollisionEvent> &getEvents(void) const;
		CollisionSummary getSummary(void) const;
		bool writeCsv(const std::string &path) const;
	};

};

#endif
