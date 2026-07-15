#ifndef MATCH_TELEMETRY_H
#define MATCH_TELEMETRY_H

#include <fstream>
#include <string>

#include "analytics/match_heatmap.h"
#include "analytics/match_metrics.h"
#include "analytics/pass_detector.h"
#include "ball.h"
#include "player.h"

namespace soccer {

	class MatchTelemetry {

		bool active;
		int tick;
		int sampleIndex;
		int sampleStride;
		std::ofstream snapshotsOut;
		std::ofstream eventsOut;
		std::string runId;
		std::string manifestPath;
		std::string metadataPath;
		std::string snapshotsPath;
		std::string eventsPath;
		std::string derivedEventsPath;
		std::string heatmapPath;
		std::string metricsPath;
		std::string team01Name;
		std::string team02Name;
		MatchHeatmap heatmap;
		MatchMetrics metrics;
		PassDetector passDetector;

		static std::string csv(const std::string &value);
		static std::string fileSafe(const std::string &value);
		bool writeMetadata(const std::string &stamp);
		bool writeReplayManifest(const std::string &stamp);
		CPlayer *nearestCarrier(CPlayer **players,
					int nplayers,
					const CVector3D &ballPos) const;
		void writeEntity(double matchTime, const std::string &entityType,
				 const std::string &teamName, int number,
				 const std::string &name, const CVector3D &pos,
				 const CVector3D &vel, bool team01Ball);

	public:
		MatchTelemetry();
		~MatchTelemetry();

		bool begin(const std::string &team01, const std::string &team02,
			   int stride = 5);
		void sample(double clockMin, double clockSec, const CBall &ball,
			    CPlayer **team01Players, int nTeam01Players,
			    CPlayer **team02Players, int nTeam02Players,
			    bool team01Ball);
		void recordEvent(double clockMin, double clockSec,
				 const std::string &eventType,
				 const std::string &teamName,
				 int number,
				 const std::string &name,
				 const CVector3D &pos,
				 bool team01Ball,
				 const std::string &detail = "");
		void finish(void);
		const std::string &getRunId(void) const;
		const std::string &getManifestPath(void) const;
		const std::string &getMetadataPath(void) const;
		const std::string &getSnapshotsPath(void) const;
		const std::string &getEventsPath(void) const;
		const std::string &getDerivedEventsPath(void) const;
		const std::string &getHeatmapPath(void) const;
		const std::string &getMetricsPath(void) const;
		int getHeatmapColumns(void) const;
		int getHeatmapRows(void) const;
		int getTeamHeatmapSamples(const std::string &teamName,
					  int column,
					  int row) const;
		int getTeamHeatmapMaxSamples(const std::string &teamName) const;
		int getPlayerHeatmapSamples(const std::string &teamName,
					    int number,
					    int column,
					    int row) const;
		int getPlayerHeatmapMaxSamples(const std::string &teamName, int number) const;
	};

};

#endif
