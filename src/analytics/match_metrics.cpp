#include "match_metrics.h"

#include <cmath>
#include <fstream>
#include <iomanip>

namespace soccer {

	static double vectorLength(const CVector3D &v)
	{
		return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}

	MatchMetrics::MatchMetrics()
	{
	}

	std::string MatchMetrics::csv(const std::string &value)
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

	void MatchMetrics::reset(void)
	{
		series.clear();
	}

	MatchMetrics::Series *MatchMetrics::findSeries(const std::string &entityType,
						       const std::string &teamName,
						       int number,
						       const std::string &name)
	{
		for(std::vector<Series>::iterator it = series.begin(); it != series.end(); ++it) {
			if(it->entityType == entityType &&
			   it->teamName == teamName &&
			   it->number == number &&
			   it->name == name) {
				return &(*it);
			}
		}

		Series next;
		next.entityType = entityType;
		next.teamName = teamName;
		next.number = number;
		next.name = name;
		next.samples = 0;
		next.distance = 0.0;
		next.speedSum = 0.0;
		next.maxSpeed = 0.0;
		next.hasPrevious = false;
		next.previousPos = CVector3D(0,0,0);
		series.push_back(next);

		return &series[series.size() - 1];
	}

	void MatchMetrics::record(const std::string &entityType,
				  const std::string &teamName,
				  int number,
				  const std::string &name,
				  const CVector3D &pos,
				  const CVector3D &vel)
	{
		Series *target = findSeries(entityType, teamName, number, name);
		double speed = vectorLength(vel);

		if(target->hasPrevious) {
			CVector3D delta(
				pos.x - target->previousPos.x,
				pos.y - target->previousPos.y,
				pos.z - target->previousPos.z);
			target->distance += vectorLength(delta);
		}

		target->samples++;
		target->speedSum += speed;

		if(speed > target->maxSpeed) {
			target->maxSpeed = speed;
		}

		target->previousPos = pos;
		target->hasPrevious = true;
	}

	bool MatchMetrics::writeCsv(const std::string &path) const
	{
		std::ofstream out(path.c_str());

		if(!out.is_open()) {
			return false;
		}

		out << "entity_type,team,number,name,samples,distance,average_speed,max_speed\n";

		for(std::vector<Series>::const_iterator it = series.begin(); it != series.end(); ++it) {
			double averageSpeed = it->samples > 0 ? it->speedSum / it->samples : 0.0;

			out << csv(it->entityType) << ","
				<< csv(it->teamName) << ","
				<< it->number << ","
				<< csv(it->name) << ","
				<< it->samples << ","
				<< std::fixed << std::setprecision(5)
				<< it->distance << ","
				<< averageSpeed << ","
				<< it->maxSpeed << "\n";
		}

		return true;
	}

};
