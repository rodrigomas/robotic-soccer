#include "analytics/pass_lane_tracker.h"

#include <cmath>
#include <fstream>
#include <iomanip>

namespace soccer {

	PassLaneTracker::PassLaneTracker() :
		maxOptionsPerSample(3),
		pressureWeight(0.65),
		passDistanceWeight(0.35),
		goalDistanceWeight(0.20)
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

	double PassLaneTracker::distanceXZ(const CVector3D &a, const CVector3D &b)
	{
		double dx = a.x - b.x;
		double dz = a.z - b.z;
		return std::sqrt(dx * dx + dz * dz);
	}

	void PassLaneTracker::insertRanked(std::vector<PassLaneOption> *ranked,
					   const PassLaneOption &option,
					   int maxOptions)
	{
		if(!ranked || maxOptions <= 0) {
			return;
		}

		if(static_cast<int>(ranked->size()) == maxOptions &&
		   option.score <= (*ranked)[ranked->size() - 1].score) {
			return;
		}

		ranked->push_back(option);
		int index = static_cast<int>(ranked->size()) - 1;
		while(index > 0 && (*ranked)[index - 1].score < option.score) {
			(*ranked)[index] = (*ranked)[index - 1];
			index--;
		}
		(*ranked)[index] = option;

		if(static_cast<int>(ranked->size()) > maxOptions) {
			ranked->pop_back();
		}
	}

	double PassLaneTracker::nearestOpponentDistance(const PassLaneSample &sample,
							const CVector3D &pos) const
	{
		double bestDistance = 1000000.0;

		for(std::vector<PassLanePlayer>::const_iterator it = sample.opponents.begin();
		    it != sample.opponents.end(); ++it) {
			double distance = distanceXZ(pos, it->pos);
			if(distance < bestDistance) {
				bestDistance = distance;
			}
		}

		return bestDistance;
	}

	void PassLaneTracker::reset(void)
	{
		options.clear();
	}

	void PassLaneTracker::setMaxOptionsPerSample(int maxOptions)
	{
		if(maxOptions > 0) {
			maxOptionsPerSample = maxOptions;
		}
	}

	void PassLaneTracker::setWeights(double pressure,
					 double passDistance,
					 double goalDistance)
	{
		pressureWeight = pressure;
		passDistanceWeight = passDistance;
		goalDistanceWeight = goalDistance;
	}

	bool PassLaneTracker::record(const PassLaneSample &sample)
	{
		if(sample.possessionTeam == "" ||
		   sample.carrierNumber <= 0 ||
		   sample.teammates.empty()) {
			return false;
		}

		CVector3D goal(0, 0, sample.targetGoalZ);
		std::vector<PassLaneOption> ranked;

		for(std::vector<PassLanePlayer>::const_iterator it = sample.teammates.begin();
		    it != sample.teammates.end(); ++it) {
			if(it->number == sample.carrierNumber &&
			   it->team == sample.possessionTeam) {
				continue;
			}

			double passDistance = distanceXZ(sample.carrierPos, it->pos);
			double targetGoalDistance = distanceXZ(it->pos, goal);
			double pressure = nearestOpponentDistance(sample, it->pos);
			double score = pressure * pressureWeight -
				passDistance * passDistanceWeight -
				targetGoalDistance * goalDistanceWeight;

			PassLaneOption option;
			option.tick = sample.tick;
			option.matchTime = sample.matchTime;
			option.possessionTeam = sample.possessionTeam;
			option.carrierNumber = sample.carrierNumber;
			option.carrierName = sample.carrierName;
			option.rank = 0;
			option.targetNumber = it->number;
			option.targetName = it->name;
			option.score = score;
			option.passDistance = passDistance;
			option.targetPressure = pressure;
			option.targetGoalDistance = targetGoalDistance;
			option.forwardProgress = std::fabs(sample.targetGoalZ - sample.carrierPos.z) -
				std::fabs(sample.targetGoalZ - it->pos.z);
			option.carrierPos = sample.carrierPos;
			option.targetPos = it->pos;
			option.ballPos = sample.ballPos;
			insertRanked(&ranked, option, maxOptionsPerSample);
		}

		for(std::vector<PassLaneOption>::iterator it = ranked.begin();
		    it != ranked.end(); ++it) {
			it->rank = static_cast<int>(it - ranked.begin()) + 1;
			options.push_back(*it);
		}

		return !ranked.empty();
	}

	const std::vector<PassLaneOption> &PassLaneTracker::getOptions(void) const
	{
		return options;
	}

	PassLaneSummary PassLaneTracker::getSummary(void) const
	{
		PassLaneSummary summary;
		summary.samples = 0;
		summary.options = static_cast<int>(options.size());
		summary.lastOptionCount = 0;
		summary.lastBestTargetNumber = 0;
		summary.lastBestTargetName = "";
		summary.lastBestScore = 0.0;
		summary.lastBestPassDistance = 0.0;

		int lastTick = -1;
		for(std::vector<PassLaneOption>::const_iterator it = options.begin();
		    it != options.end(); ++it) {
			if(it->tick != lastTick) {
				summary.samples++;
				lastTick = it->tick;
			}
		}

		if(!options.empty()) {
			const PassLaneOption &last = options[options.size() - 1];
			int lastSampleTick = last.tick;
			for(std::vector<PassLaneOption>::const_reverse_iterator it = options.rbegin();
			    it != options.rend(); ++it) {
				if(it->tick != lastSampleTick) {
					break;
				}
				summary.lastOptionCount++;
				if(it->rank == 1) {
					summary.lastBestTargetNumber = it->targetNumber;
					summary.lastBestTargetName = it->targetName;
					summary.lastBestScore = it->score;
					summary.lastBestPassDistance = it->passDistance;
				}
			}
		}

		return summary;
	}

	bool PassLaneTracker::writeCsv(const std::string &path) const
	{
		std::ofstream out(path.c_str());

		if(!out.is_open()) {
			return false;
		}

		out << "tick,match_time,possession_team,carrier_number,carrier_name,"
			<< "rank,target_number,target_name,score,pass_distance,"
			<< "target_pressure,target_goal_distance,forward_progress,"
			<< "carrier_x,carrier_z,target_x,target_z,ball_x,ball_z\n";

		for(std::vector<PassLaneOption>::const_iterator it = options.begin();
		    it != options.end(); ++it) {
			out << it->tick << ","
				<< std::fixed << std::setprecision(3)
				<< it->matchTime << ","
				<< csv(it->possessionTeam) << ","
				<< it->carrierNumber << ","
				<< csv(it->carrierName) << ","
				<< it->rank << ","
				<< it->targetNumber << ","
				<< csv(it->targetName) << ","
				<< std::fixed << std::setprecision(5)
				<< it->score << ","
				<< it->passDistance << ","
				<< it->targetPressure << ","
				<< it->targetGoalDistance << ","
				<< it->forwardProgress << ","
				<< it->carrierPos.x << ","
				<< it->carrierPos.z << ","
				<< it->targetPos.x << ","
				<< it->targetPos.z << ","
				<< it->ballPos.x << ","
				<< it->ballPos.z << "\n";
		}

		return true;
	}

};
