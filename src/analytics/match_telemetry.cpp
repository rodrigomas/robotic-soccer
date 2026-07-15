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

	MatchTelemetry::MatchTelemetry() :
		active(false),
		tick(0),
		sampleIndex(0),
		sampleStride(5)
	{
	}

	MatchTelemetry::~MatchTelemetry()
	{
		finish();
	}

	std::string MatchTelemetry::csv(const std::string &value)
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

	std::string MatchTelemetry::fileSafe(const std::string &value)
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

	bool MatchTelemetry::begin(const std::string &team01,
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

		snapshotsPath = std::string("telemetry/match_") + stamp + "_" +
			fileSafe(team01Name) + "_vs_" + fileSafe(team02Name) + ".csv";
		eventsPath = std::string("telemetry/events_") + stamp + "_" +
			fileSafe(team01Name) + "_vs_" + fileSafe(team02Name) + ".csv";
		heatmapPath = std::string("telemetry/heatmap_") + stamp + "_" +
			fileSafe(team01Name) + "_vs_" + fileSafe(team02Name) + ".csv";
		heatmap.reset(-45.0, 45.0, -60.0, 60.0, 18, 24);

		snapshotsOut.open(snapshotsPath.c_str());
		eventsOut.open(eventsPath.c_str());
		active = snapshotsOut.is_open() && eventsOut.is_open();

		if(active) {
			snapshotsOut << "sample,tick,match_time,entity_type,team,number,name,"
				<< "x,y,z,vx,vy,vz,team_in_possession\n";
			eventsOut << "tick,match_time,event_type,team,number,name,"
				<< "x,y,z,team_in_possession,detail\n";
		}

		return active;
	}

	void MatchTelemetry::writeEntity(double matchTime,
					 const std::string &entityType,
					 const std::string &teamName,
					 int number,
					 const std::string &name,
					 const CVector3D &pos,
					 const CVector3D &vel,
					 bool team01Ball)
	{
		snapshotsOut << sampleIndex << ","
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

	void MatchTelemetry::sample(double clockMin, double clockSec,
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

		heatmap.record("ball", "", 0, "Ball", ball.pos);
		writeEntity(matchTime, "ball", "", 0, "Ball", ball.pos, ball.vel, team01Ball);

		for(register int i = 0; i < nTeam01Players; i++) {
			if(team01Players[i] != NULL) {
				heatmap.record("player", team01Name,
					       team01Players[i]->num,
					       team01Players[i]->name,
					       team01Players[i]->pos);
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
				heatmap.record("player", team02Name,
					       team02Players[i]->num,
					       team02Players[i]->name,
					       team02Players[i]->pos);
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

	void MatchTelemetry::recordEvent(double clockMin, double clockSec,
					 const std::string &eventType,
					 const std::string &teamName,
					 int number,
					 const std::string &name,
					 const CVector3D &pos,
					 bool team01Ball,
					 const std::string &detail)
	{
		if(!active) {
			return;
		}

		double matchTime = clockMin * 60.0 + clockSec;

		eventsOut << tick << ","
			<< std::fixed << std::setprecision(3) << matchTime << ","
			<< csv(eventType) << ","
			<< csv(teamName) << ","
			<< number << ","
			<< csv(name) << ","
			<< std::fixed << std::setprecision(5)
			<< pos.x << "," << pos.y << "," << pos.z << ","
			<< csv(team01Ball ? team01Name : team02Name) << ","
			<< csv(detail) << "\n";
	}

	void MatchTelemetry::finish(void)
	{
		if(active && heatmapPath != "") {
			heatmap.writeCsv(heatmapPath);
		}

		if(snapshotsOut.is_open()) {
			snapshotsOut.flush();
			snapshotsOut.close();
		}

		if(eventsOut.is_open()) {
			eventsOut.flush();
			eventsOut.close();
		}

		active = false;
	}

	const std::string &MatchTelemetry::getSnapshotsPath(void) const
	{
		return snapshotsPath;
	}

	const std::string &MatchTelemetry::getEventsPath(void) const
	{
		return eventsPath;
	}

	const std::string &MatchTelemetry::getHeatmapPath(void) const
	{
		return heatmapPath;
	}

};
