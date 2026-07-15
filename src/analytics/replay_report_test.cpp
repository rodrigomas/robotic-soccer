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
using soccer::ReplayReportSummary;
using soccer::formatReplaySummaryText;
using soccer::loadReplaySummary;
using soccer::parseReplaySummary;
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
		"  \"format_version\": 1,\n"
		"  \"run_id\": \"replay_report_test\",\n"
		"  \"random_seed\": 1234,\n"
		"  \"teams\": {\n"
		"    \"team01\": \"Team A\",\n"
		"    \"team02\": \"Team B\"\n"
		"  },\n"
		"  \"samples\": 24,\n"
		"  \"ticks\": 120,\n"
		"  \"last_match_time\": 12.500,\n"
		"  \"passes\": {\n"
		"    \"derived_events\": 5,\n"
		"    \"completed\": 3,\n"
		"    \"possession_changes\": 2,\n"
		"    \"longest_distance\": 18.25000\n"
		"  },\n"
		"  \"pressure\": {\n"
		"    \"samples\": 20,\n"
		"    \"high_pressure_samples\": 4,\n"
		"    \"average_distance\": 7.50000,\n"
		"    \"min_distance\": 2.75000,\n"
		"    \"last_distance\": 6.00000\n"
		"  },\n"
		"  \"shots\": {\n"
		"    \"total\": 2,\n"
		"    \"team01\": 1,\n"
		"    \"team02\": 1,\n"
		"    \"last_team\": \"Team B\",\n"
		"    \"last_shooter_number\": 10,\n"
		"    \"last_speed\": 14.75000\n"
		"  },\n"
		"  \"collisions\": {\n"
		"    \"total\": 7,\n"
		"    \"player_player\": 3,\n"
		"    \"player_ball\": 4,\n"
		"    \"last_relative_speed\": 5.25000\n"
		"  },\n"
		"  \"pass_lanes\": {\n"
		"    \"samples\": 8,\n"
		"    \"options\": 16,\n"
		"    \"last_option_count\": 2,\n"
		"    \"last_best_target_number\": 9,\n"
		"    \"last_best_score\": 11.25000,\n"
		"    \"last_best_pass_distance\": 21.50000\n"
		"  }\n"
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

	ReplayReportSummary summary;
	if(!parseReplaySummary(body, &summary, &error)) {
		removeFile(summaryPath);
		removeFile(manifestPath);
		return fail("could not parse replay summary body");
	}

	if(summary.runId != "replay_report_test" ||
	   summary.team01Name != "Team A" ||
	   summary.team02Name != "Team B" ||
	   summary.completedPasses != 3 ||
	   summary.highPressureSamples != 4 ||
	   summary.totalShots != 2 ||
	   summary.totalCollisions != 7 ||
	   summary.lastBestTargetNumber != 9) {
		removeFile(summaryPath);
		removeFile(manifestPath);
		return fail("parsed replay summary did not contain expected metrics");
	}

	ReplayReportSummary loadedSummary;
	if(!loadReplaySummary(manifestPath, &loadedSummary, &error)) {
		removeFile(summaryPath);
		removeFile(manifestPath);
		return fail("could not load typed replay summary through manifest");
	}

	std::string text = formatReplaySummaryText(loadedSummary);
	if(text.find("Replay replay_report_test") == std::string::npos ||
	   text.find("Team A vs Team B") == std::string::npos ||
	   text.find("Passes: 3 completed") == std::string::npos ||
	   text.find("Pass lanes: 16 options") == std::string::npos) {
		removeFile(summaryPath);
		removeFile(manifestPath);
		return fail("text replay report did not contain expected fields");
	}

	removeFile(summaryPath);
	removeFile(manifestPath);
	return 0;
}
