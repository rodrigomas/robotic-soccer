#include "analytics/replay_report.h"
#include "analytics/replay_manifest.h"

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

using soccer::ReplayManifest;
using soccer::readReplaySummary;

static int fail(const char *message)
{
	std::cerr << "replay_report_test: " << message << "\n";
	return 1;
}

static bool writeFile(const std::string &path, const std::string &body)
{
	std::ofstream out(path.c_str());
	if(!out.is_open()) {
		return false;
	}

	out << body;
	return true;
}

static void removeFile(const std::string &path)
{
	std::remove(path.c_str());
}

int main(void)
{
#ifdef _WIN32
	_mkdir("telemetry");
#else
	mkdir("telemetry", 0755);
#endif

	std::string summaryPath = "telemetry/summary_replay_report_test.json";
	std::string manifestPath = "telemetry/replay_report_test.json";
	std::string summaryBody =
		"{\n"
		"  \"format\": \"robotic-soccer-match-summary\",\n"
		"  \"passes\": {\"completed\": 3}\n"
		"}\n";

	if(!writeFile(summaryPath, summaryBody)) {
		return fail("could not write summary fixture");
	}

	ReplayManifest manifest;
	manifest.runId = "replay_report_test";
	manifest.createdAt = "test";
	manifest.randomSeed = 1234;
	manifest.team01Name = "Team A";
	manifest.team02Name = "Team B";
	manifest.sampleStride = 5;
	manifest.metadataPath = "telemetry/metadata_replay_report_test.csv";
	manifest.snapshotsPath = "telemetry/match_replay_report_test.csv";
	manifest.eventsPath = "telemetry/events_replay_report_test.csv";
	manifest.heatmapPath = "telemetry/heatmap_replay_report_test.csv";
	manifest.metricsPath = "telemetry/metrics_replay_report_test.csv";
	manifest.summaryPath = summaryPath;

	if(!manifest.write(manifestPath)) {
		removeFile(summaryPath);
		return fail("could not write manifest fixture");
	}

	std::string body;
	std::string error;
	if(!readReplaySummary(manifestPath, &body, &error)) {
		removeFile(summaryPath);
		removeFile(manifestPath);
		return fail("could not read replay summary through manifest");
	}

	if(body.find("\"robotic-soccer-match-summary\"") == std::string::npos ||
	   body.find("\"completed\": 3") == std::string::npos) {
		removeFile(summaryPath);
		removeFile(manifestPath);
		return fail("replay summary body did not contain expected fields");
	}

	removeFile(summaryPath);
	removeFile(manifestPath);
	return 0;
}
