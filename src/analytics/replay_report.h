#ifndef REPLAY_REPORT_H
#define REPLAY_REPORT_H

#include <string>
#include <stdint.h>

namespace soccer {

	struct ReplayReportSummary {
		std::string runId;
		uint32_t randomSeed;
		std::string team01Name;
		std::string team02Name;
		int samples;
		int ticks;
		double lastMatchTime;
		int derivedEvents;
		int completedPasses;
		int possessionChanges;
		double longestPassDistance;
		int pressureSamples;
		int highPressureSamples;
		double averagePressureDistance;
		double minPressureDistance;
		double lastPressureDistance;
		int totalShots;
		int team01Shots;
		int team02Shots;
		std::string lastShotTeam;
		int lastShooterNumber;
		double lastShotSpeed;
		int totalCollisions;
		int playerPlayerCollisions;
		int playerBallCollisions;
		double lastRelativeSpeed;
		int passLaneSamples;
		int passLaneOptions;
		int lastPassLaneOptionCount;
		int lastBestTargetNumber;
		double lastBestScore;
		double lastBestPassDistance;

		ReplayReportSummary();
	};

	bool readReplaySummary(const std::string &manifestPath,
			       std::string *summaryBody,
			       std::string *errorMessage);
	bool parseReplaySummary(const std::string &summaryBody,
				ReplayReportSummary *summary,
				std::string *errorMessage);
	bool loadReplaySummary(const std::string &manifestPath,
			       ReplayReportSummary *summary,
			       std::string *errorMessage);
	std::string formatReplaySummaryText(const ReplayReportSummary &summary);

};

#endif
