#include "analytics/pressure_tracker.h"

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
using soccer::PressureOpponent;
using soccer::PressureSummary;
using soccer::PressureTracker;
using soccer::PressureTrackerSample;

static int fail(const char *message)
{
	std::cerr << "pressure_tracker_test: " << message << "\n";
	return 1;
}

static PressureOpponent opponent(const std::string &team,
				 int number,
				 const std::string &name,
				 double x,
				 double z)
{
	PressureOpponent result;
	result.team = team;
	result.number = number;
	result.name = name;
	result.pos = CVector3D(x, 0, z);
	return result;
}

static PressureTrackerSample sample(int tick,
				    double matchTime,
				    const std::string &team,
				    int carrierNumber,
				    double x,
				    double z)
{
	PressureTrackerSample result;
	result.tick = tick;
	result.matchTime = matchTime;
	result.possessionTeam = team;
	result.carrierNumber = carrierNumber;
	result.carrierName = "Carrier";
	result.carrierPos = CVector3D(x, 0, z);
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

	PressureTracker tracker;
	tracker.setHighPressureDistance(6.0);

	PressureTrackerSample first = sample(1, 0.0, "Team A", 7, 0, 0);
	first.opponents.push_back(opponent("Team B", 3, "Near", 3, 4));
	first.opponents.push_back(opponent("Team B", 8, "Far", 20, 0));

	if(!tracker.record(first)) {
		return fail("first pressure sample was not recorded");
	}

	PressureTrackerSample second = sample(2, 1.0, "Team A", 7, 0, 0);
	second.opponents.push_back(opponent("Team B", 3, "Near", 9, 0));

	if(!tracker.record(second)) {
		return fail("second pressure sample was not recorded");
	}

	PressureSummary summary = tracker.getSummary();
	if(summary.samples != 2 || summary.highPressureSamples != 1) {
		return fail("pressure summary sample counts were wrong");
	}

	if(summary.minPressureDistance < 4.9 ||
	   summary.minPressureDistance > 5.1 ||
	   summary.averagePressureDistance < 6.9 ||
	   summary.averagePressureDistance > 7.1) {
		return fail("pressure summary distances were wrong");
	}

	if(summary.lastCarrierNumber != 7 ||
	   summary.lastOpponentNumber != 3 ||
	   summary.lastPressureDistance < 8.9 ||
	   summary.lastPressureDistance > 9.1) {
		return fail("pressure summary last frame was wrong");
	}

	std::string path = "telemetry/pressure_tracker_test.csv";
	if(!tracker.writeCsv(path)) {
		return fail("could not write pressure csv");
	}

	if(!fileContains(path, "pressure_distance") ||
	   !fileContains(path, "\"Team B\"")) {
		return fail("pressure csv did not contain expected data");
	}

	std::remove(path.c_str());
	return 0;
}
