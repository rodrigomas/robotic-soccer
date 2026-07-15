#include "match_telemetry.h"

#include "analytics/replay_manifest.h"
#include "engine/core/deterministic_random.h"

#include <cctype>
#include <cmath>
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

	bool MatchTelemetry::writeMetadata(const std::string &stamp)
	{
		std::ofstream metadataOut(metadataPath.c_str());

		if(!metadataOut.is_open()) {
			return false;
		}

		metadataOut << "key,value\n";
		metadataOut << "format_version,1\n";
		metadataOut << "run_id," << csv(runId) << "\n";
		metadataOut << "created_at," << csv(stamp) << "\n";
		metadataOut << "random_seed," << getDeterministicRandomSeed() << "\n";
		metadataOut << "team01," << csv(team01Name) << "\n";
		metadataOut << "team02," << csv(team02Name) << "\n";
		metadataOut << "sample_stride," << sampleStride << "\n";
		metadataOut << "manifest_path," << csv(manifestPath) << "\n";
		metadataOut << "snapshots_path," << csv(snapshotsPath) << "\n";
		metadataOut << "events_path," << csv(eventsPath) << "\n";
		metadataOut << "derived_events_path," << csv(derivedEventsPath) << "\n";
		metadataOut << "heatmap_path," << csv(heatmapPath) << "\n";
		metadataOut << "metrics_path," << csv(metricsPath) << "\n";

		return true;
	}

	bool MatchTelemetry::writeReplayManifest(const std::string &stamp)
	{
		ReplayManifest manifest;
		manifest.runId = runId;
		manifest.createdAt = stamp;
		manifest.randomSeed = getDeterministicRandomSeed();
		manifest.team01Name = team01Name;
		manifest.team02Name = team02Name;
		manifest.sampleStride = sampleStride;
		manifest.metadataPath = metadataPath;
		manifest.snapshotsPath = snapshotsPath;
		manifest.eventsPath = eventsPath;
		manifest.derivedEventsPath = derivedEventsPath;
		manifest.heatmapPath = heatmapPath;
		manifest.metricsPath = metricsPath;

		return manifest.write(manifestPath);
	}

	CPlayer *MatchTelemetry::nearestCarrier(CPlayer **players,
						int nplayers,
						const CVector3D &ballPos) const
	{
		CPlayer *best = NULL;
		double bestDistance = 1000000.0;

		for(register int i = 0; i < nplayers; i++) {
			if(players[i] == NULL || players[i]->ncards >= 2) {
				continue;
			}

			double dx = players[i]->pos.x - ballPos.x;
			double dz = players[i]->pos.z - ballPos.z;
			double distance = std::sqrt(dx * dx + dz * dz);
			if(distance < bestDistance) {
				best = players[i];
				bestDistance = distance;
			}
		}

		return best;
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

		runId = std::string(stamp) + "_" + fileSafe(team01Name) +
			"_vs_" + fileSafe(team02Name);
		manifestPath = std::string("telemetry/replay_") + runId + ".json";
		metadataPath = std::string("telemetry/metadata_") + runId + ".csv";
		snapshotsPath = std::string("telemetry/match_") + runId + ".csv";
		eventsPath = std::string("telemetry/events_") + runId + ".csv";
		derivedEventsPath = std::string("telemetry/derived_events_") + runId + ".csv";
		heatmapPath = std::string("telemetry/heatmap_") + runId + ".csv";
		metricsPath = std::string("telemetry/metrics_") + runId + ".csv";
		heatmap.reset(-45.0, 45.0, -60.0, 60.0, 18, 24);
		metrics.reset();
		passDetector.reset();

		snapshotsOut.open(snapshotsPath.c_str());
		eventsOut.open(eventsPath.c_str());
		active = snapshotsOut.is_open() && eventsOut.is_open();

		if(active) {
			writeReplayManifest(stamp);
			writeMetadata(stamp);
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
		metrics.record("ball", "", 0, "Ball", ball.pos, ball.vel);
		writeEntity(matchTime, "ball", "", 0, "Ball", ball.pos, ball.vel, team01Ball);

		CPlayer **possessionPlayers = team01Ball ? team01Players : team02Players;
		int possessionPlayerCount = team01Ball ? nTeam01Players : nTeam02Players;
		CPlayer *carrier = nearestCarrier(possessionPlayers, possessionPlayerCount,
						  ball.pos);
		if(carrier != NULL) {
			PassDetectorSample passSample;
			passSample.tick = tick;
			passSample.matchTime = matchTime;
			passSample.possessionTeam = team01Ball ? team01Name : team02Name;
			passSample.carrierNumber = carrier->num;
			passSample.carrierName = carrier->name;
			passSample.carrierPos = carrier->pos;
			passSample.ballPos = ball.pos;
			passDetector.record(passSample);
		}

		for(register int i = 0; i < nTeam01Players; i++) {
			if(team01Players[i] != NULL) {
				heatmap.record("player", team01Name,
					       team01Players[i]->num,
					       team01Players[i]->name,
					       team01Players[i]->pos);
				metrics.record("player", team01Name,
					       team01Players[i]->num,
					       team01Players[i]->name,
					       team01Players[i]->pos,
					       team01Players[i]->vel);
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
				metrics.record("player", team02Name,
					       team02Players[i]->num,
					       team02Players[i]->name,
					       team02Players[i]->pos,
					       team02Players[i]->vel);
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

		if(active && metricsPath != "") {
			metrics.writeCsv(metricsPath);
		}

		if(active && derivedEventsPath != "") {
			passDetector.writeCsv(derivedEventsPath);
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

	const std::string &MatchTelemetry::getRunId(void) const
	{
		return runId;
	}

	const std::string &MatchTelemetry::getManifestPath(void) const
	{
		return manifestPath;
	}

	const std::string &MatchTelemetry::getMetadataPath(void) const
	{
		return metadataPath;
	}

	const std::string &MatchTelemetry::getSnapshotsPath(void) const
	{
		return snapshotsPath;
	}

	const std::string &MatchTelemetry::getEventsPath(void) const
	{
		return eventsPath;
	}

	const std::string &MatchTelemetry::getDerivedEventsPath(void) const
	{
		return derivedEventsPath;
	}

	const std::string &MatchTelemetry::getHeatmapPath(void) const
	{
		return heatmapPath;
	}

	const std::string &MatchTelemetry::getMetricsPath(void) const
	{
		return metricsPath;
	}

	int MatchTelemetry::getDerivedEventCount(void) const
	{
		return static_cast<int>(passDetector.getEvents().size());
	}

	PassDetectorSummary MatchTelemetry::getDerivedEventSummary(void) const
	{
		return passDetector.getSummary();
	}

	int MatchTelemetry::getHeatmapColumns(void) const
	{
		return heatmap.getColumns();
	}

	int MatchTelemetry::getHeatmapRows(void) const
	{
		return heatmap.getRows();
	}

	int MatchTelemetry::getTeamHeatmapSamples(const std::string &teamName,
						  int column,
						  int row) const
	{
		return heatmap.getTeamSamples(teamName, column, row);
	}

	int MatchTelemetry::getTeamHeatmapMaxSamples(const std::string &teamName) const
	{
		return heatmap.getTeamMaxSamples(teamName);
	}

	int MatchTelemetry::getPlayerHeatmapSamples(const std::string &teamName,
						    int number,
						    int column,
						    int row) const
	{
		return heatmap.getPlayerSamples(teamName, number, column, row);
	}

	int MatchTelemetry::getPlayerHeatmapMaxSamples(const std::string &teamName,
						       int number) const
	{
		return heatmap.getPlayerMaxSamples(teamName, number);
	}

};
