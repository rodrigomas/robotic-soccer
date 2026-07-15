#include "match_telemetry.h"

#include <cctype>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

namespace soccer {

	CMatchTelemetry::CMatchTelemetry() :
		active(false),
		tick(0),
		sampleIndex(0),
		sampleStride(5)
	{
	}

	CMatchTelemetry::~CMatchTelemetry()
	{
		finish();
	}

	std::string CMatchTelemetry::csv(const std::string &value)
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

	std::string CMatchTelemetry::fileSafe(const std::string &value)
	{
		std::string result;

		for(std::string::const_iterator it = value.begin(); it != value.end(); ++it) {
			unsigned char c = static_cast<unsigned char>(*it);
			if(std::isalnum(c)) {
				result += static_cast<char>(std::tolower(c));
			} else if(!result.empty() && result[result.size() - 1] != '_') {
				result += '_';
			}
		}

		if(result.empty()) {
			result = "team";
		}

		if(result[result.size() - 1] == '_') {
			result.erase(result.size() - 1);
		}

		return result;
	}

	bool CMatchTelemetry::begin(const std::string &team01,
				    const std::string &team02,
				    int stride)
	{
		finish();

		team01Name = team01;
		team02Name = team02;
		sampleStride = stride > 0 ? stride : 1;
		tick = 0;
		sampleIndex = 0;

#ifdef _WIN32
		_mkdir("telemetry");
#else
		mkdir("telemetry", 0755);
#endif

		std::time_t now = std::time(NULL);
		std::tm *local = std::localtime(&now);
		char stamp[32];

		if(local != NULL) {
			std::strftime(stamp, sizeof(stamp), "%Y%m%d_%H%M%S", local);
		} else {
			std::snprintf(stamp, sizeof(stamp), "unknown_time");
		}

		outputPath = std::string("telemetry/match_") + stamp + "_" +
			fileSafe(team01Name) + "_vs_" + fileSafe(team02Name) + ".csv";

		out.open(outputPath.c_str());
		active = out.is_open();

		if(active) {
			out << "sample,tick,match_time,entity_type,team,number,name,"
				<< "x,y,z,vx,vy,vz,team_in_possession\n";
		}

		return active;
	}

	void CMatchTelemetry::writeEntity(double matchTime,
					  const std::string &entityType,
					  const std::string &teamName,
					  int number,
					  const std::string &name,
					  const CVector3D &pos,
					  const CVector3D &vel,
					  bool team01Ball)
	{
		out << sampleIndex << ","
			<< tick << ","
			<< std::fixed << std::setprecision(3) << matchTime << ","
			<< csv(entityType) << ","
			<< csv(teamName) << ","
			<< number << ","
			<< csv(name) << ","
			<< std::fixed << std::setprecision(5)
			<< pos.x << "," << pos.y << "," << pos.z << ","
			<< vel.x << "," << vel.y << "," << vel.z << ","
			<< csv(team01Ball ? team01Name : team02Name) << "\n";
	}

	void CMatchTelemetry::sample(double clockMin, double clockSec,
				     const CBall &ball,
				     CPlayer **team01Players,
				     int nTeam01Players,
				     CPlayer **team02Players,
				     int nTeam02Players,
				     bool team01Ball)
	{
		if(!active) {
			return;
		}

		tick++;
		if((tick - 1) % sampleStride != 0) {
			return;
		}

		double matchTime = clockMin * 60.0 + clockSec;

		writeEntity(matchTime, "ball", "", 0, "Ball", ball.pos, ball.vel, team01Ball);

		for(register int i = 0; i < nTeam01Players; i++) {
			if(team01Players[i] != NULL) {
				writeEntity(matchTime, "player", team01Name,
					    team01Players[i]->num,
					    team01Players[i]->name,
					    team01Players[i]->pos,
					    team01Players[i]->vel,
					    team01Ball);
			}
		}

		for(register int i = 0; i < nTeam02Players; i++) {
			if(team02Players[i] != NULL) {
				writeEntity(matchTime, "player", team02Name,
					    team02Players[i]->num,
					    team02Players[i]->name,
					    team02Players[i]->pos,
					    team02Players[i]->vel,
					    team01Ball);
			}
		}

		sampleIndex++;
	}

	void CMatchTelemetry::finish(void)
	{
		if(out.is_open()) {
			out.flush();
			out.close();
		}

		active = false;
	}

	const std::string &CMatchTelemetry::getOutputPath(void) const
	{
		return outputPath;
	}

};
