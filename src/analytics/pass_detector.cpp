#include "analytics/pass_detector.h"

#include <cmath>
#include <fstream>
#include <iomanip>

namespace soccer {

	PassDetector::PassDetector() :
		hasPrevious(false)
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

	double PassDetector::distanceXZ(const CVector3D &a, const CVector3D &b)
	{
		double dx = a.x - b.x;
		double dz = a.z - b.z;
		return std::sqrt(dx * dx + dz * dz);
	}

	void PassDetector::reset(void)
	{
		hasPrevious = false;
		events.clear();
	}

	bool PassDetector::record(const PassDetectorSample &sample)
	{
		if(sample.possessionTeam == "" || sample.carrierNumber <= 0) {
			return false;
		}

		if(!hasPrevious) {
			previous = sample;
			hasPrevious = true;
			return false;
		}

		if(previous.possessionTeam == sample.possessionTeam &&
		   previous.carrierNumber == sample.carrierNumber) {
			previous = sample;
			return false;
		}

		PassDetectorEvent event;
		event.tick = sample.tick;
		event.matchTime = sample.matchTime;
		event.eventType = previous.possessionTeam == sample.possessionTeam ?
			"pass_completed" : "possession_change";
		event.fromTeam = previous.possessionTeam;
		event.fromNumber = previous.carrierNumber;
		event.fromName = previous.carrierName;
		event.toTeam = sample.possessionTeam;
		event.toNumber = sample.carrierNumber;
		event.toName = sample.carrierName;
		event.passDistance = distanceXZ(previous.carrierPos, sample.carrierPos);
		event.ballPos = sample.ballPos;
		events.push_back(event);

		previous = sample;
		return true;
	}

	const std::vector<PassDetectorEvent> &PassDetector::getEvents(void) const
	{
		return events;
	}

	bool PassDetector::writeCsv(const std::string &path) const
	{
		std::ofstream out(path.c_str());

		if(!out.is_open()) {
			return false;
		}

		out << "tick,match_time,event_type,from_team,from_number,from_name,"
			<< "to_team,to_number,to_name,pass_distance,ball_x,ball_y,ball_z\n";

		for(std::vector<PassDetectorEvent>::const_iterator it = events.begin();
		    it != events.end(); ++it) {
			out << it->tick << ","
				<< std::fixed << std::setprecision(3)
				<< it->matchTime << ","
				<< csv(it->eventType) << ","
				<< csv(it->fromTeam) << ","
				<< it->fromNumber << ","
				<< csv(it->fromName) << ","
				<< csv(it->toTeam) << ","
				<< it->toNumber << ","
				<< csv(it->toName) << ","
				<< std::fixed << std::setprecision(5)
				<< it->passDistance << ","
				<< it->ballPos.x << ","
				<< it->ballPos.y << ","
				<< it->ballPos.z << "\n";
		}

		return true;
	}

};
