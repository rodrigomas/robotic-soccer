#include "analytics/shot_detector.h"

#include <cmath>
#include <fstream>
#include <iomanip>

namespace soccer {

	ShotDetector::ShotDetector() :
		activeShot(false),
		minShotSpeed(12.0),
		minForwardSpeed(7.0),
		resetShotSpeed(5.0),
		maxShootingLaneX(35.0)
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

	double ShotDetector::speedXZ(const CVector3D &v)
	{
		return std::sqrt(v.x * v.x + v.z * v.z);
	}

	void ShotDetector::reset(void)
	{
		activeShot = false;
		activeTeam = "";
		events.clear();
	}

	void ShotDetector::setThresholds(double shotSpeed,
					 double forwardSpeed,
					 double resetSpeed,
					 double shootingLaneX)
	{
		if(shotSpeed > 0.0) {
			minShotSpeed = shotSpeed;
		}
		if(forwardSpeed > 0.0) {
			minForwardSpeed = forwardSpeed;
		}
		if(resetSpeed > 0.0) {
			resetShotSpeed = resetSpeed;
		}
		if(shootingLaneX > 0.0) {
			maxShootingLaneX = shootingLaneX;
		}
	}

	bool ShotDetector::record(const ShotDetectorSample &sample)
	{
		if(sample.team == "" || sample.shooterNumber <= 0) {
			return false;
		}

		double speed = speedXZ(sample.ballVel);
		double goalDirection = sample.targetGoalZ >= sample.ballPos.z ? 1.0 : -1.0;
		double forwardSpeed = sample.ballVel.z * goalDirection;
		bool inShootingLane = std::fabs(sample.ballPos.x) <= maxShootingLaneX;
		bool shotLike = speed >= minShotSpeed &&
			forwardSpeed >= minForwardSpeed &&
			inShootingLane;

		if(activeShot &&
		   (speed <= resetShotSpeed || forwardSpeed <= 0.0 || sample.team != activeTeam)) {
			activeShot = false;
			activeTeam = "";
		}

		if(!shotLike || activeShot) {
			return false;
		}

		ShotEvent event;
		event.tick = sample.tick;
		event.matchTime = sample.matchTime;
		event.team = sample.team;
		event.shooterNumber = sample.shooterNumber;
		event.shooterName = sample.shooterName;
		event.shotSpeed = speed;
		event.forwardSpeed = forwardSpeed;
		event.goalDistance = std::fabs(sample.targetGoalZ - sample.ballPos.z);
		event.targetGoalZ = sample.targetGoalZ;
		event.shooterPos = sample.shooterPos;
		event.ballPos = sample.ballPos;
		event.ballVel = sample.ballVel;
		events.push_back(event);

		activeShot = true;
		activeTeam = sample.team;
		return true;
	}

	const std::vector<ShotEvent> &ShotDetector::getEvents(void) const
	{
		return events;
	}

	int ShotDetector::getTeamShotCount(const std::string &teamName) const
	{
		int count = 0;

		for(std::vector<ShotEvent>::const_iterator it = events.begin();
		    it != events.end(); ++it) {
			if(it->team == teamName) {
				count++;
			}
		}

		return count;
	}

	ShotSummary ShotDetector::getSummary(void) const
	{
		ShotSummary summary;
		summary.totalShots = static_cast<int>(events.size());
		summary.lastShotTeam = "";
		summary.lastShooterNumber = 0;
		summary.lastShooterName = "";
		summary.lastShotSpeed = 0.0;
		summary.lastGoalDistance = 0.0;

		if(!events.empty()) {
			const ShotEvent &last = events[events.size() - 1];
			summary.lastShotTeam = last.team;
			summary.lastShooterNumber = last.shooterNumber;
			summary.lastShooterName = last.shooterName;
			summary.lastShotSpeed = last.shotSpeed;
			summary.lastGoalDistance = last.goalDistance;
		}

		return summary;
	}

	bool ShotDetector::writeCsv(const std::string &path) const
	{
		std::ofstream out(path.c_str());

		if(!out.is_open()) {
			return false;
		}

		out << "tick,match_time,team,shooter_number,shooter_name,shot_speed,"
			<< "forward_speed,goal_distance,target_goal_z,shooter_x,shooter_z,"
			<< "ball_x,ball_y,ball_z,ball_vx,ball_vy,ball_vz\n";

		for(std::vector<ShotEvent>::const_iterator it = events.begin();
		    it != events.end(); ++it) {
			out << it->tick << ","
				<< std::fixed << std::setprecision(3)
				<< it->matchTime << ","
				<< csv(it->team) << ","
				<< it->shooterNumber << ","
				<< csv(it->shooterName) << ","
				<< std::fixed << std::setprecision(5)
				<< it->shotSpeed << ","
				<< it->forwardSpeed << ","
				<< it->goalDistance << ","
				<< it->targetGoalZ << ","
				<< it->shooterPos.x << ","
				<< it->shooterPos.z << ","
				<< it->ballPos.x << ","
				<< it->ballPos.y << ","
				<< it->ballPos.z << ","
				<< it->ballVel.x << ","
				<< it->ballVel.y << ","
				<< it->ballVel.z << "\n";
		}

		return true;
	}

};
