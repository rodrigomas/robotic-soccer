#include "analytics/pass_detector.h"

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

using soccer::PassDetector;
using soccer::PassDetectorSample;
using soccer::PassDetectorSummary;
using soccer::CVector3D;

static int fail(const char *message)
{
	std::cerr << "pass_detector_test: " << message << "\n";
	return 1;
}

static PassDetectorSample sample(int tick, double matchTime,
				 const std::string &team,
				 int number,
				 const std::string &name,
				 double x,
				 double z)
{
	PassDetectorSample result;
	result.tick = tick;
	result.matchTime = matchTime;
	result.possessionTeam = team;
	result.carrierNumber = number;
	result.carrierName = name;
	result.carrierPos = CVector3D(x, 0, z);
	result.ballPos = CVector3D(x, 0, z);
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

	PassDetector detector;

	if(detector.record(sample(1, 0.0, "Team A", 7, "Seven", 0, 0))) {
		return fail("first carrier sample produced an event");
	}

	if(detector.record(sample(2, 1.0, "Team A", 7, "Seven", 1, 0))) {
		return fail("same carrier produced an event");
	}

	if(!detector.record(sample(3, 2.0, "Team A", 9, "Nine", 12, 0))) {
		return fail("same-team carrier change did not produce pass");
	}

	if(!detector.record(sample(4, 3.0, "Team B", 3, "Three", 15, 0))) {
		return fail("team change did not produce possession change");
	}

	if(detector.getEvents().size() != 2) {
		return fail("unexpected event count");
	}

	PassDetectorSummary summary = detector.getSummary();
	if(summary.totalEvents != 2 ||
	   summary.completedPasses != 1 ||
	   summary.possessionChanges != 1) {
		return fail("summary counts did not match detected events");
	}

	if(summary.longestPassDistance < 10.9 ||
	   summary.longestPassDistance > 11.1 ||
	   summary.longestPassFromNumber != 7 ||
	   summary.longestPassToNumber != 9) {
		return fail("summary longest pass did not match first pass");
	}

	if(summary.lastEventType != "possession_change" ||
	   summary.lastFromNumber != 9 ||
	   summary.lastToNumber != 3) {
		return fail("summary last event did not match final detector event");
	}

	if(detector.getEvents()[0].eventType != "pass_completed") {
		return fail("first event was not pass_completed");
	}

	if(detector.getEvents()[1].eventType != "possession_change") {
		return fail("second event was not possession_change");
	}

	if(detector.getEvents()[0].passDistance < 10.9 ||
	   detector.getEvents()[0].passDistance > 11.1) {
		return fail("pass distance was not measured from carriers");
	}

	std::string path = "telemetry/pass_detector_test.csv";
	if(!detector.writeCsv(path)) {
		return fail("could not write detector csv");
	}

	if(!fileContains(path, "pass_completed") ||
	   !fileContains(path, "possession_change")) {
		return fail("detector csv did not contain expected events");
	}

	std::remove(path.c_str());
	return 0;
}
