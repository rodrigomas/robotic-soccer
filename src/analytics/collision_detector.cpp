#include "analytics/collision_detector.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace soccer {

	CollisionDetector::CollisionDetector() :
		contactTolerance(0.2)
	{
	}

	static std::string csv(const std::string &value)
	{
		std::string result = "\"";

		for(std::string::const_iterator it = value.begin(); it != value.end(); ++it) {
			if(*it == '"') {
				result += "\"\"";
			} else {
				result += *it;
			}
		}

		result += "\"";
		return result;
	}

	double CollisionDetector::distanceXZ(const CVector3D &a, const CVector3D &b)
	{
		double dx = a.x - b.x;
		double dz = a.z - b.z;
		return std::sqrt(dx * dx + dz * dz);
	}

	double CollisionDetector::relativeSpeedXZ(const CVector3D &a, const CVector3D &b)
	{
		double dx = a.x - b.x;
		double dz = a.z - b.z;
		return std::sqrt(dx * dx + dz * dz);
	}

	std::string CollisionDetector::playerKey(const CollisionParticipant &player)
	{
		std::ostringstream out;
		out << player.team << "#" << player.number;
		return out.str();
	}

	std::string CollisionDetector::pairKey(const CollisionParticipant &a,
					       const CollisionParticipant &b)
	{
		std::string keyA = playerKey(a);
		std::string keyB = playerKey(b);

		if(keyA < keyB) {
			return "pp|" + keyA + "|" + keyB;
		}

		return "pp|" + keyB + "|" + keyA;
	}

	std::string CollisionDetector::ballKey(const CollisionParticipant &player)
	{
		return "pb|" + playerKey(player);
	}

	void CollisionDetector::reset(void)
	{
		activeContacts.clear();
		events.clear();
	}

	void CollisionDetector::setContactTolerance(double tolerance)
	{
		if(tolerance >= 0.0) {
			contactTolerance = tolerance;
		}
	}

	bool CollisionDetector::record(const CollisionDetectorSample &sample)
	{
		std::set<std::string> currentContacts;
		bool createdEvent = false;

		for(std::vector<CollisionParticipant>::const_iterator a = sample.players.begin();
		    a != sample.players.end(); ++a) {
			for(std::vector<CollisionParticipant>::const_iterator b = a + 1;
			    b != sample.players.end(); ++b) {
				double distance = distanceXZ(a->pos, b->pos);
				if(distance > a->radius + b->radius + contactTolerance) {
					continue;
				}

				std::string key = pairKey(*a, *b);
				currentContacts.insert(key);
				if(activeContacts.find(key) != activeContacts.end()) {
					continue;
				}

				CollisionEvent event;
				event.tick = sample.tick;
				event.matchTime = sample.matchTime;
				event.eventType = "player_player";
				event.teamA = a->team;
				event.numberA = a->number;
				event.nameA = a->name;
				event.teamB = b->team;
				event.numberB = b->number;
				event.nameB = b->name;
				event.distance = distance;
				event.relativeSpeed = relativeSpeedXZ(a->vel, b->vel);
				event.posA = a->pos;
				event.posB = b->pos;
				events.push_back(event);
				createdEvent = true;
			}
		}

		for(std::vector<CollisionParticipant>::const_iterator it = sample.players.begin();
		    it != sample.players.end(); ++it) {
			double distance = distanceXZ(it->pos, sample.ballPos);
			if(distance > it->radius + sample.ballRadius + contactTolerance) {
				continue;
			}

			std::string key = ballKey(*it);
			currentContacts.insert(key);
			if(activeContacts.find(key) != activeContacts.end()) {
				continue;
			}

			CollisionEvent event;
			event.tick = sample.tick;
			event.matchTime = sample.matchTime;
			event.eventType = "player_ball";
			event.teamA = it->team;
			event.numberA = it->number;
			event.nameA = it->name;
			event.teamB = "";
			event.numberB = 0;
			event.nameB = "Ball";
			event.distance = distance;
			event.relativeSpeed = relativeSpeedXZ(it->vel, sample.ballVel);
			event.posA = it->pos;
			event.posB = sample.ballPos;
			events.push_back(event);
			createdEvent = true;
		}

		activeContacts = currentContacts;
		return createdEvent;
	}

	const std::vector<CollisionEvent> &CollisionDetector::getEvents(void) const
	{
		return events;
	}

	CollisionSummary CollisionDetector::getSummary(void) const
	{
		CollisionSummary summary;
		summary.totalCollisions = static_cast<int>(events.size());
		summary.playerPlayerCollisions = 0;
		summary.playerBallCollisions = 0;
		summary.lastEventType = "";
		summary.lastTeamA = "";
		summary.lastNumberA = 0;
		summary.lastTeamB = "";
		summary.lastNumberB = 0;
		summary.lastRelativeSpeed = 0.0;

		for(std::vector<CollisionEvent>::const_iterator it = events.begin();
		    it != events.end(); ++it) {
			if(it->eventType == "player_player") {
				summary.playerPlayerCollisions++;
			} else if(it->eventType == "player_ball") {
				summary.playerBallCollisions++;
			}

			summary.lastEventType = it->eventType;
			summary.lastTeamA = it->teamA;
			summary.lastNumberA = it->numberA;
			summary.lastTeamB = it->teamB;
			summary.lastNumberB = it->numberB;
			summary.lastRelativeSpeed = it->relativeSpeed;
		}

		return summary;
	}

	bool CollisionDetector::writeCsv(const std::string &path) const
	{
		std::ofstream out(path.c_str());

		if(!out.is_open()) {
			return false;
		}

		out << "tick,match_time,event_type,team_a,number_a,name_a,"
			<< "team_b,number_b,name_b,distance,relative_speed,"
			<< "a_x,a_z,b_x,b_z\n";

		for(std::vector<CollisionEvent>::const_iterator it = events.begin();
		    it != events.end(); ++it) {
			out << it->tick << ","
				<< std::fixed << std::setprecision(3)
				<< it->matchTime << ","
				<< csv(it->eventType) << ","
				<< csv(it->teamA) << ","
				<< it->numberA << ","
				<< csv(it->nameA) << ","
				<< csv(it->teamB) << ","
				<< it->numberB << ","
				<< csv(it->nameB) << ","
				<< std::fixed << std::setprecision(5)
				<< it->distance << ","
				<< it->relativeSpeed << ","
				<< it->posA.x << ","
				<< it->posA.z << ","
				<< it->posB.x << ","
				<< it->posB.z << "\n";
		}

		return true;
	}

};
