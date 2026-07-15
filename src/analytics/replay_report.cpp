#include "analytics/replay_report.h"

#include "analytics/replay_manifest.h"

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

namespace soccer {

	ReplayReportSummary::ReplayReportSummary() :
		randomSeed(0),
		samples(0),
		ticks(0),
		lastMatchTime(0.0),
		derivedEvents(0),
		completedPasses(0),
		possessionChanges(0),
		longestPassDistance(0.0),
		pressureSamples(0),
		highPressureSamples(0),
		averagePressureDistance(0.0),
		minPressureDistance(0.0),
		lastPressureDistance(0.0),
		totalShots(0),
		team01Shots(0),
		team02Shots(0),
		lastShooterNumber(0),
		lastShotSpeed(0.0),
		totalCollisions(0),
		playerPlayerCollisions(0),
		playerBallCollisions(0),
		lastRelativeSpeed(0.0),
		passLaneSamples(0),
		passLaneOptions(0),
		lastPassLaneOptionCount(0),
		lastBestTargetNumber(0),
		lastBestScore(0.0),
		lastBestPassDistance(0.0)
	{
	}

	static bool readFile(const std::string &path, std::string *body)
	{
		if(!body) {
			return false;
		}

		std::ifstream in(path.c_str());
		if(!in.is_open()) {
			return false;
		}

		std::ostringstream out;
		out << in.rdbuf();
		*body = out.str();
		return true;
	}

	static std::string directoryName(const std::string &path)
	{
		std::string::size_type slash = path.find_last_of("/\\");
		if(slash == std::string::npos) {
			return "";
		}

		return path.substr(0, slash);
	}

	static std::string parentDirectory(const std::string &path)
	{
		std::string dir = directoryName(path);
		if(dir == path) {
			return "";
		}

		return dir;
	}

	static std::string joinPath(const std::string &base, const std::string &leaf)
	{
		if(base == "") {
			return leaf;
		}

		char last = base[base.size() - 1];
		if(last == '/' || last == '\\') {
			return base + leaf;
		}

		return base + "/" + leaf;
	}

	static bool readStringField(const std::string &body,
				    const std::string &name,
				    std::string *value)
	{
		if(!value) {
			return false;
		}

		std::string key = "\"" + name + "\"";
		std::string::size_type keyPos = body.find(key);
		if(keyPos == std::string::npos) {
			return false;
		}

		std::string::size_type colon = body.find(':', keyPos + key.length());
		if(colon == std::string::npos) {
			return false;
		}

		std::string::size_type quote = body.find('"', colon + 1);
		if(quote == std::string::npos) {
			return false;
		}

		std::string result;
		bool escaped = false;
		for(std::string::size_type i = quote + 1; i < body.length(); i++) {
			char c = body[i];
			if(escaped) {
				switch(c) {
					case 'b':
						result += '\b';
						break;
					case 'f':
						result += '\f';
						break;
					case 'n':
						result += '\n';
						break;
					case 'r':
						result += '\r';
						break;
					case 't':
						result += '\t';
						break;
					default:
						result += c;
						break;
				}
				escaped = false;
			} else if(c == '\\') {
				escaped = true;
			} else if(c == '"') {
				*value = result;
				return true;
			} else {
				result += c;
			}
		}

		return false;
	}

	static bool readDoubleField(const std::string &body,
				    const std::string &name,
				    double *value)
	{
		if(!value) {
			return false;
		}

		std::string key = "\"" + name + "\"";
		std::string::size_type keyPos = body.find(key);
		if(keyPos == std::string::npos) {
			return false;
		}

		std::string::size_type colon = body.find(':', keyPos + key.length());
		if(colon == std::string::npos) {
			return false;
		}

		const char *start = body.c_str() + colon + 1;
		char *end = NULL;
		double parsed = std::strtod(start, &end);
		if(end == start) {
			return false;
		}

		*value = parsed;
		return true;
	}

	static bool readIntField(const std::string &body,
				 const std::string &name,
				 int *value)
	{
		double parsed = 0.0;
		if(!value || !readDoubleField(body, name, &parsed)) {
			return false;
		}

		*value = static_cast<int>(parsed);
		return true;
	}

	static bool readSeedField(const std::string &body,
				  const std::string &name,
				  uint32_t *value)
	{
		double parsed = 0.0;
		if(!value || !readDoubleField(body, name, &parsed)) {
			return false;
		}

		*value = static_cast<uint32_t>(parsed);
		return true;
	}

	static bool readObjectField(const std::string &body,
				    const std::string &name,
				    std::string *objectBody)
	{
		if(!objectBody) {
			return false;
		}

		std::string key = "\"" + name + "\"";
		std::string::size_type keyPos = body.find(key);
		if(keyPos == std::string::npos) {
			return false;
		}

		std::string::size_type colon = body.find(':', keyPos + key.length());
		if(colon == std::string::npos) {
			return false;
		}

		std::string::size_type start = body.find('{', colon + 1);
		if(start == std::string::npos) {
			return false;
		}

		bool inString = false;
		bool escaped = false;
		int depth = 0;
		for(std::string::size_type i = start; i < body.length(); i++) {
			char c = body[i];
			if(inString) {
				if(escaped) {
					escaped = false;
				} else if(c == '\\') {
					escaped = true;
				} else if(c == '"') {
					inString = false;
				}
			} else if(c == '"') {
				inString = true;
			} else if(c == '{') {
				depth++;
			} else if(c == '}') {
				depth--;
				if(depth == 0) {
					*objectBody = body.substr(start, i - start + 1);
					return true;
				}
			}
		}

		return false;
	}

	bool readReplaySummary(const std::string &manifestPath,
			       std::string *summaryBody,
			       std::string *errorMessage)
	{
		if(!summaryBody) {
			if(errorMessage) {
				*errorMessage = "summary output pointer was null";
			}
			return false;
		}

		ReplayManifest manifest;
		if(!ReplayManifest::load(manifestPath, &manifest)) {
			if(errorMessage) {
				*errorMessage = "could not load replay manifest: " + manifestPath;
			}
			return false;
		}

		if(manifest.summaryPath == "") {
			if(errorMessage) {
				*errorMessage = "replay manifest does not include a summary path";
			}
			return false;
		}

		std::vector<std::string> candidates;
		candidates.push_back(manifest.summaryPath);

		std::string manifestDir = directoryName(manifestPath);
		if(manifestDir != "") {
			candidates.push_back(joinPath(manifestDir, manifest.summaryPath));
			std::string manifestParent = parentDirectory(manifestDir);
			if(manifestParent != "") {
				candidates.push_back(joinPath(manifestParent, manifest.summaryPath));
			}
		}

		for(std::vector<std::string>::const_iterator it = candidates.begin();
		    it != candidates.end(); ++it) {
			if(readFile(*it, summaryBody)) {
				return true;
			}
		}

		if(errorMessage) {
			*errorMessage = "could not read replay summary: " + manifest.summaryPath;
		}
		return false;
	}

	bool parseReplaySummary(const std::string &summaryBody,
				ReplayReportSummary *summary,
				std::string *errorMessage)
	{
		if(!summary) {
			if(errorMessage) {
				*errorMessage = "summary output pointer was null";
			}
			return false;
		}

		std::string format;
		int formatVersion = 0;
		if(!readStringField(summaryBody, "format", &format) ||
		   !readIntField(summaryBody, "format_version", &formatVersion) ||
		   format != "robotic-soccer-match-summary" ||
		   formatVersion != 1) {
			if(errorMessage) {
				*errorMessage = "summary format was not recognized";
			}
			return false;
		}

		ReplayReportSummary parsed;
		std::string teams;
		std::string passes;
		std::string pressure;
		std::string shots;
		std::string collisions;
		std::string passLanes;
		if(!readStringField(summaryBody, "run_id", &parsed.runId) ||
		   !readSeedField(summaryBody, "random_seed", &parsed.randomSeed) ||
		   !readObjectField(summaryBody, "teams", &teams) ||
		   !readStringField(teams, "team01", &parsed.team01Name) ||
		   !readStringField(teams, "team02", &parsed.team02Name) ||
		   !readIntField(summaryBody, "samples", &parsed.samples) ||
		   !readIntField(summaryBody, "ticks", &parsed.ticks) ||
		   !readDoubleField(summaryBody, "last_match_time", &parsed.lastMatchTime) ||
		   !readObjectField(summaryBody, "passes", &passes) ||
		   !readObjectField(summaryBody, "pressure", &pressure) ||
		   !readObjectField(summaryBody, "shots", &shots) ||
		   !readObjectField(summaryBody, "collisions", &collisions) ||
		   !readObjectField(summaryBody, "pass_lanes", &passLanes)) {
			if(errorMessage) {
				*errorMessage = "summary did not include the expected top-level fields";
			}
			return false;
		}

		if(!readIntField(passes, "derived_events", &parsed.derivedEvents) ||
		   !readIntField(passes, "completed", &parsed.completedPasses) ||
		   !readIntField(passes, "possession_changes", &parsed.possessionChanges) ||
		   !readDoubleField(passes, "longest_distance", &parsed.longestPassDistance) ||
		   !readIntField(pressure, "samples", &parsed.pressureSamples) ||
		   !readIntField(pressure, "high_pressure_samples", &parsed.highPressureSamples) ||
		   !readDoubleField(pressure, "average_distance", &parsed.averagePressureDistance) ||
		   !readDoubleField(pressure, "min_distance", &parsed.minPressureDistance) ||
		   !readDoubleField(pressure, "last_distance", &parsed.lastPressureDistance) ||
		   !readIntField(shots, "total", &parsed.totalShots) ||
		   !readIntField(shots, "team01", &parsed.team01Shots) ||
		   !readIntField(shots, "team02", &parsed.team02Shots) ||
		   !readStringField(shots, "last_team", &parsed.lastShotTeam) ||
		   !readIntField(shots, "last_shooter_number", &parsed.lastShooterNumber) ||
		   !readDoubleField(shots, "last_speed", &parsed.lastShotSpeed) ||
		   !readIntField(collisions, "total", &parsed.totalCollisions) ||
		   !readIntField(collisions, "player_player", &parsed.playerPlayerCollisions) ||
		   !readIntField(collisions, "player_ball", &parsed.playerBallCollisions) ||
		   !readDoubleField(collisions, "last_relative_speed", &parsed.lastRelativeSpeed) ||
		   !readIntField(passLanes, "samples", &parsed.passLaneSamples) ||
		   !readIntField(passLanes, "options", &parsed.passLaneOptions) ||
		   !readIntField(passLanes, "last_option_count", &parsed.lastPassLaneOptionCount) ||
		   !readIntField(passLanes, "last_best_target_number", &parsed.lastBestTargetNumber) ||
		   !readDoubleField(passLanes, "last_best_score", &parsed.lastBestScore) ||
		   !readDoubleField(passLanes, "last_best_pass_distance", &parsed.lastBestPassDistance)) {
			if(errorMessage) {
				*errorMessage = "summary did not include the expected metric fields";
			}
			return false;
		}

		*summary = parsed;
		return true;
	}

	bool loadReplaySummary(const std::string &manifestPath,
			       ReplayReportSummary *summary,
			       std::string *errorMessage)
	{
		std::string body;
		if(!readReplaySummary(manifestPath, &body, errorMessage)) {
			return false;
		}

		return parseReplaySummary(body, summary, errorMessage);
	}

	std::string formatReplaySummaryText(const ReplayReportSummary &summary)
	{
		std::ostringstream out;
		out << std::fixed << std::setprecision(2);
		out << "Replay " << summary.runId << "\n";
		out << "Teams: " << summary.team01Name << " vs " << summary.team02Name << "\n";
		out << "Seed: " << summary.randomSeed
		    << "  Samples: " << summary.samples
		    << "  Ticks: " << summary.ticks
		    << "  Match time: " << summary.lastMatchTime << "s\n";
		out << "Passes: " << summary.completedPasses << " completed, "
		    << summary.possessionChanges << " possession changes, longest "
		    << summary.longestPassDistance << "\n";
		out << "Pressure: " << summary.highPressureSamples << "/"
		    << summary.pressureSamples << " high-pressure samples, avg "
		    << summary.averagePressureDistance << ", min "
		    << summary.minPressureDistance << "\n";
		out << "Shots: " << summary.totalShots << " total ("
		    << summary.team01Name << " " << summary.team01Shots << ", "
		    << summary.team02Name << " " << summary.team02Shots << ")";
		if(summary.lastShotTeam != "") {
			out << ", last " << summary.lastShotTeam
			    << " #" << summary.lastShooterNumber
			    << " speed " << summary.lastShotSpeed;
		}
		out << "\n";
		out << "Collisions: " << summary.totalCollisions << " total, "
		    << summary.playerPlayerCollisions << " player-player, "
		    << summary.playerBallCollisions << " player-ball, last relative speed "
		    << summary.lastRelativeSpeed << "\n";
		out << "Pass lanes: " << summary.passLaneOptions << " options across "
		    << summary.passLaneSamples << " samples, latest best target #"
		    << summary.lastBestTargetNumber << " score "
		    << summary.lastBestScore << " distance "
		    << summary.lastBestPassDistance << "\n";
		return out.str();
	}

};
