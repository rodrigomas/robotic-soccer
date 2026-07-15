#include "analytics/pass_lane_tracker.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

using soccer::CVector3D;
using soccer::PassLanePlayer;
using soccer::PassLaneSample;
using soccer::PassLaneSummary;
using soccer::PassLaneTracker;

static int fail(const char *message)
{
	std::cerr << "pass_lane_tracker_test: " << message << "\n";
	return 1;
}

static PassLanePlayer player(const std::string &team,
			     int number,
			     const std::string &name,
			     double x,
			     double z)
{
	PassLanePlayer result;
	result.team = team;
	result.number = number;
	result.name = name;
	result.pos = CVector3D(x, 0, z);
	return result;
}

static bool fileContains(const std::string &path, const std::string &needle)
{
	std::ifstream in(path.c_str());
	if(!in.is_open()) {
		return false;
	}

	std::string body;
	std::string line;
	while(std::getline(in, line)) {
		body += line;
		body += "\n";
	}

	return body.find(needle) != std::string::npos;
}

int main(void)
{
#ifdef _WIN32
	_mkdir("telemetry");
#else
	mkdir("telemetry", 0755);
#endif

	PassLaneTracker tracker;
	tracker.setMaxOptionsPerSample(2);
	tracker.setWeights(1.0, 0.2, 0.1);

	PassLaneSample sample;
	sample.tick = 1;
	sample.matchTime = 0.0;
	sample.possessionTeam = "Team A";
	sample.carrierNumber = 7;
	sample.carrierName = "Seven";
	sample.carrierPos = CVector3D(0, 0, 0);
	sample.ballPos = CVector3D(0, 0, 0);
	sample.targetGoalZ = 60.0;
	sample.teammates.push_back(player("Team A", 7, "Seven", 0, 0));
	sample.teammates.push_back(player("Team A", 9, "Nine", 0, 20));
	sample.teammates.push_back(player("Team A", 11, "Eleven", 20, 0));
	sample.teammates.push_back(player("Team A", 5, "Five", 0, -20));
	sample.opponents.push_back(player("Team B", 3, "Three", 10, 20));
	sample.opponents.push_back(player("Team B", 4, "Four", 30, 0));
	sample.opponents.push_back(player("Team B", 6, "Six", 0, -19));

	if(!tracker.record(sample)) {
		return fail("pass lane sample produced no options");
	}

	const std::vector<soccer::PassLaneOption> &options = tracker.getOptions();
	if(options.size() != 2) {
		return fail("pass lane tracker did not keep top two options");
	}

	if(options[0].rank != 1 ||
	   options[0].targetNumber != 9 ||
	   options[1].rank != 2) {
		return fail("pass lane ranking was unexpected");
	}

	PassLaneSummary summary = tracker.getSummary();
	if(summary.samples != 1 ||
	   summary.options != 2 ||
	   summary.lastOptionCount != 2 ||
	   summary.lastBestTargetNumber != 9) {
		return fail("pass lane summary was wrong");
	}

	std::string path = "telemetry/pass_lane_tracker_test.csv";
	if(!tracker.writeCsv(path)) {
		return fail("could not write pass lane csv");
	}

	if(!fileContains(path, "forward_progress") ||
	   !fileContains(path, "\"Nine\"")) {
		return fail("pass lane csv did not contain expected data");
	}

	std::remove(path.c_str());
	return 0;
}
