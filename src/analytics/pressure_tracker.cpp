#include "analytics/pressure_tracker.h"

#include <cmath>
#include <fstream>
#include <iomanip>

namespace soccer {

	PressureTracker::PressureTracker() :
		highPressureDistance(8.0)
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

	double PressureTracker::distanceXZ(const CVector3D &a, const CVector3D &b)
	{
		double dx = a.x - b.x;
		double dz = a.z - b.z;
		return std::sqrt(dx * dx + dz * dz);
	}

	void PressureTracker::reset(void)
	{
		frames.clear();
	}

	void PressureTracker::setHighPressureDistance(double distance)
	{
		if(distance > 0.0) {
			highPressureDistance = distance;
		}
	}

	bool PressureTracker::record(const PressureTrackerSample &sample)
	{
		if(sample.possessionTeam == "" ||
		   sample.carrierNumber <= 0 ||
		   sample.opponents.empty()) {
			return false;
		}

		int nearestIndex = -1;
		double nearestDistance = 1000000.0;

		for(std::vector<PressureOpponent>::const_iterator it = sample.opponents.begin();
		    it != sample.opponents.end(); ++it) {
			double distance = distanceXZ(sample.carrierPos, it->pos);
			if(distance < nearestDistance) {
				nearestDistance = distance;
				nearestIndex = static_cast<int>(it - sample.opponents.begin());
			}
		}

		if(nearestIndex < 0) {
			return false;
		}

		const PressureOpponent &opponent = sample.opponents[nearestIndex];
		PressureFrame frame;
		frame.tick = sample.tick;
		frame.matchTime = sample.matchTime;
		frame.possessionTeam = sample.possessionTeam;
		frame.carrierNumber = sample.carrierNumber;
		frame.carrierName = sample.carrierName;
		frame.opponentTeam = opponent.team;
		frame.opponentNumber = opponent.number;
		frame.opponentName = opponent.name;
		frame.pressureDistance = nearestDistance;
		frame.highPressure = nearestDistance <= highPressureDistance;
		frame.carrierPos = sample.carrierPos;
		frame.opponentPos = opponent.pos;
		frames.push_back(frame);

		return true;
	}

	const std::vector<PressureFrame> &PressureTracker::getFrames(void) const
	{
		return frames;
	}

	PressureSummary PressureTracker::getSummary(void) const
	{
		PressureSummary summary;
		summary.samples = static_cast<int>(frames.size());
		summary.highPressureSamples = 0;
		summary.averagePressureDistance = 0.0;
		summary.minPressureDistance = 0.0;
		summary.lastPressureDistance = 0.0;
		summary.lastPossessionTeam = "";
		summary.lastCarrierNumber = 0;
		summary.lastOpponentTeam = "";
		summary.lastOpponentNumber = 0;

		double totalDistance = 0.0;
		for(std::vector<PressureFrame>::const_iterator it = frames.begin();
		    it != frames.end(); ++it) {
			totalDistance += it->pressureDistance;
			if(it == frames.begin() ||
			   it->pressureDistance < summary.minPressureDistance) {
				summary.minPressureDistance = it->pressureDistance;
			}
			if(it->highPressure) {
				summary.highPressureSamples++;
			}

			summary.lastPressureDistance = it->pressureDistance;
			summary.lastPossessionTeam = it->possessionTeam;
			summary.lastCarrierNumber = it->carrierNumber;
			summary.lastOpponentTeam = it->opponentTeam;
			summary.lastOpponentNumber = it->opponentNumber;
		}

		if(summary.samples > 0) {
			summary.averagePressureDistance = totalDistance /
				static_cast<double>(summary.samples);
		}

		return summary;
	}

	bool PressureTracker::writeCsv(const std::string &path) const
	{
		std::ofstream out(path.c_str());

		if(!out.is_open()) {
			return false;
		}

		out << "tick,match_time,possession_team,carrier_number,carrier_name,"
			<< "opponent_team,opponent_number,opponent_name,pressure_distance,"
			<< "high_pressure,carrier_x,carrier_z,opponent_x,opponent_z\n";

		for(std::vector<PressureFrame>::const_iterator it = frames.begin();
		    it != frames.end(); ++it) {
			out << it->tick << ","
				<< std::fixed << std::setprecision(3)
				<< it->matchTime << ","
				<< csv(it->possessionTeam) << ","
				<< it->carrierNumber << ","
				<< csv(it->carrierName) << ","
				<< csv(it->opponentTeam) << ","
				<< it->opponentNumber << ","
				<< csv(it->opponentName) << ","
				<< std::fixed << std::setprecision(5)
				<< it->pressureDistance << ","
				<< (it->highPressure ? 1 : 0) << ","
				<< it->carrierPos.x << ","
				<< it->carrierPos.z << ","
				<< it->opponentPos.x << ","
				<< it->opponentPos.z << "\n";
		}

		return true;
	}

};
