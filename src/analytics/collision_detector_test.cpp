#include "analytics/collision_detector.h"

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
using soccer::CollisionDetector;
using soccer::CollisionDetectorSample;
using soccer::CollisionParticipant;
using soccer::CollisionSummary;

static int fail(const char *message)
{
	std::cerr << "collision_detector_test: " << message << "\n";
	return 1;
}

static CollisionParticipant player(const std::string &team,
				   int number,
				   double x,
				   double z,
				   double vx,
				   double vz)
{
	CollisionParticipant result;
	result.team = team;
	result.number = number;
	result.name = "Player";
	result.pos = CVector3D(x, 0, z);
	result.vel = CVector3D(vx, 0, vz);
	result.radius = 1.0;
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

	CollisionDetector detector;
	detector.setContactTolerance(0.1);

	CollisionDetectorSample first;
	first.tick = 1;
	first.matchTime = 0.0;
	first.ballPos = CVector3D(8, 0, 0);
	first.ballVel = CVector3D(0, 0, 0);
	first.ballRadius = 0.5;
	first.players.push_back(player("Team A", 7, 0, 0, 2, 0));
	first.players.push_back(player("Team B", 9, 1.8, 0, -2, 0));

	if(!detector.record(first)) {
		return fail("player-player collision was not detected");
	}

	if(detector.record(first)) {
		return fail("standing contact produced duplicate collision");
	}

	CollisionDetectorSample separated = first;
	separated.tick = 2;
	separated.matchTime = 1.0;
	separated.players[1].pos = CVector3D(5, 0, 0);
	if(detector.record(separated)) {
		return fail("separation produced collision event");
	}

	CollisionDetectorSample ballContact = separated;
	ballContact.tick = 3;
	ballContact.matchTime = 2.0;
	ballContact.ballPos = CVector3D(0.5, 0, 0);
	ballContact.ballVel = CVector3D(-3, 0, 0);
	if(!detector.record(ballContact)) {
		return fail("player-ball collision was not detected");
	}

	CollisionSummary summary = detector.getSummary();
	if(summary.totalCollisions != 2 ||
	   summary.playerPlayerCollisions != 1 ||
	   summary.playerBallCollisions != 1) {
		return fail("collision summary counts were wrong");
	}

	if(summary.lastEventType != "player_ball" ||
	   summary.lastNumberA != 7 ||
	   summary.lastRelativeSpeed < 4.9 ||
	   summary.lastRelativeSpeed > 5.1) {
		return fail("collision summary last event was wrong");
	}

	std::string path = "telemetry/collision_detector_test.csv";
	if(!detector.writeCsv(path)) {
		return fail("could not write collision csv");
	}

	if(!fileContains(path, "player_player") ||
	   !fileContains(path, "player_ball")) {
		return fail("collision csv did not contain expected events");
	}

	std::remove(path.c_str());
	return 0;
}
