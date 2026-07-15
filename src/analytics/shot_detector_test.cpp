#include "analytics/shot_detector.h"

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
using soccer::ShotDetector;
using soccer::ShotDetectorSample;
using soccer::ShotSummary;

static int fail(const char *message)
{
	std::cerr << "shot_detector_test: " << message << "\n";
	return 1;
}

static ShotDetectorSample sample(int tick,
				 double matchTime,
				 const std::string &team,
				 double ballZ,
				 double ballVz)
{
	ShotDetectorSample result;
	result.tick = tick;
	result.matchTime = matchTime;
	result.team = team;
	result.shooterNumber = 9;
	result.shooterName = "Nine";
	result.shooterPos = CVector3D(1, 0, ballZ - 2);
	result.ballPos = CVector3D(1, 0, ballZ);
	result.ballVel = CVector3D(1, 0, ballVz);
	result.targetGoalZ = 60.0;
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

	ShotDetector detector;
	detector.setThresholds(10.0, 6.0, 4.0, 20.0);

	if(detector.record(sample(1, 0.0, "Team A", 0, 3))) {
		return fail("slow forward ball was detected as a shot");
	}

	if(!detector.record(sample(2, 1.0, "Team A", 0, 12))) {
		return fail("fast forward ball was not detected as a shot");
	}

	if(detector.record(sample(3, 1.5, "Team A", 8, 14))) {
		return fail("active shot produced a duplicate event");
	}

	if(detector.record(sample(4, 2.0, "Team A", 10, 2))) {
		return fail("reset sample produced an event");
	}

	ShotDetectorSample wide = sample(5, 2.5, "Team A", 10, 12);
	wide.ballPos.x = 35.0;
	if(detector.record(wide)) {
		return fail("wide ball was detected as a shot");
	}

	if(!detector.record(sample(6, 3.0, "Team A", 12, 13))) {
		return fail("second shot after reset was not detected");
	}

	if(detector.getTeamShotCount("Team A") != 2 ||
	   detector.getTeamShotCount("Team B") != 0) {
		return fail("team shot counts were wrong");
	}

	ShotSummary summary = detector.getSummary();
	if(summary.totalShots != 2 ||
	   summary.lastShotTeam != "Team A" ||
	   summary.lastShooterNumber != 9 ||
	   summary.lastShotSpeed < 12.9 ||
	   summary.lastShotSpeed > 13.1) {
		return fail("shot summary was wrong");
	}

	std::string path = "telemetry/shot_detector_test.csv";
	if(!detector.writeCsv(path)) {
		return fail("could not write shot csv");
	}

	if(!fileContains(path, "shot_speed") ||
	   !fileContains(path, "\"Team A\"")) {
		return fail("shot csv did not contain expected data");
	}

	std::remove(path.c_str());
	return 0;
}
