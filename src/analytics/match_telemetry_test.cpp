#include "analytics/match_telemetry.h"
#include "analytics/replay_manifest.h"
#include "engine/core/deterministic_random.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

using soccer::MatchTelemetry;
using soccer::ReplayManifest;
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

	if(!fileContains(telemetry.getMetadataPath(), "derived_events_path,")) {
		return fail("metadata did not include derived events path");
	}

	if(!fileContains(telemetry.getMetadataPath(), "pressure_path,")) {
		return fail("metadata did not include pressure path");
	}

	if(!fileContains(telemetry.getMetadataPath(), "shots_path,")) {
		return fail("metadata did not include shots path");
	}

	if(!fileContains(telemetry.getMetadataPath(), "collisions_path,")) {
		return fail("metadata did not include collisions path");
	}

	ReplayManifest manifest;
	if(!ReplayManifest::load(telemetry.getManifestPath(), &manifest)) {
		return fail("manifest loader could not read generated manifest");
	}

	if(manifest.runId != telemetry.getRunId()) {
		return fail("manifest run id did not match telemetry run id");
	}

	if(manifest.randomSeed != 4242) {
		return fail("manifest loader did not read deterministic seed");
	}

	if(manifest.team01Name != "Test Team A" ||
	   manifest.team02Name != "Test Team B") {
		return fail("manifest loader did not read teams");
	}

	if(manifest.sampleStride != 3) {
		return fail("manifest loader did not read sample stride");
	}

	if(manifest.derivedEventsPath != telemetry.getDerivedEventsPath()) {
		return fail("manifest loader did not read derived events path");
	}

	if(manifest.pressurePath != telemetry.getPressurePath()) {
		return fail("manifest loader did not read pressure path");
	}

	if(manifest.shotsPath != telemetry.getShotsPath()) {
		return fail("manifest loader did not read shots path");
	}

	if(manifest.collisionsPath != telemetry.getCollisionsPath()) {
		return fail("manifest loader did not read collisions path");
	}

	telemetry.finish();

	removeFile(telemetry.getManifestPath());
	removeFile(telemetry.getMetadataPath());
	removeFile(telemetry.getSnapshotsPath());
	removeFile(telemetry.getEventsPath());
	removeFile(telemetry.getDerivedEventsPath());
	removeFile(telemetry.getHeatmapPath());
	removeFile(telemetry.getMetricsPath());
	removeFile(telemetry.getPressurePath());
	removeFile(telemetry.getShotsPath());
	removeFile(telemetry.getCollisionsPath());

	return 0;
}
