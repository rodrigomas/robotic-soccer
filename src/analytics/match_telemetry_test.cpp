#include "analytics/match_telemetry.h"
#include "engine/core/deterministic_random.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

using soccer::MatchTelemetry;
using soccer::seedDeterministicRandom;

static int fail(const char *message)
{
	std::cerr << "match_telemetry_test: " << message << "\n";
	return 1;
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

static void removeFile(const std::string &path)
{
	if(path != "") {
		std::remove(path.c_str());
	}
}

int main(void)
{
	seedDeterministicRandom(4242);

	MatchTelemetry telemetry;
	if(!telemetry.begin("Test Team A", "Test Team B", 3)) {
		return fail("telemetry did not start");
	}

	if(telemetry.getRunId() == "") {
		return fail("run id was empty");
	}

	if(telemetry.getManifestPath().find("telemetry/replay_") != 0) {
		return fail("manifest path did not use replay prefix");
	}

	if(!fileContains(telemetry.getManifestPath(),
			 "\"format\": \"robotic-soccer-replay-manifest\"")) {
		return fail("manifest did not include expected format");
	}

	if(!fileContains(telemetry.getManifestPath(), "\"random_seed\": 4242")) {
		return fail("manifest did not include deterministic seed");
	}

	if(!fileContains(telemetry.getMetadataPath(), "run_id,")) {
		return fail("metadata did not include run id");
	}

	if(!fileContains(telemetry.getMetadataPath(), "manifest_path,")) {
		return fail("metadata did not include manifest path");
	}

	telemetry.finish();

	removeFile(telemetry.getManifestPath());
	removeFile(telemetry.getMetadataPath());
	removeFile(telemetry.getSnapshotsPath());
	removeFile(telemetry.getEventsPath());
	removeFile(telemetry.getHeatmapPath());
	removeFile(telemetry.getMetricsPath());

	return 0;
}
