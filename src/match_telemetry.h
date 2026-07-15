#ifndef MATCH_TELEMETRY_H
#define MATCH_TELEMETRY_H

#include <fstream>
#include <string>

#include "ball.h"
#include "player.h"

namespace soccer {

	class CMatchTelemetry {

		bool active;
		int tick;
		int sampleIndex;
		int sampleStride;
		std::ofstream out;
		std::string outputPath;
		std::string team01Name;
		std::string team02Name;

		static std::string csv(const std::string &value);
		static std::string fileSafe(const std::string &value);
		void writeEntity(double matchTime, const std::string &entityType,
				 const std::string &teamName, int number,
				 const std::string &name, const CVector3D &pos,
				 const CVector3D &vel, bool team01Ball);

	public:
		CMatchTelemetry();
		~CMatchTelemetry();

		bool begin(const std::string &team01, const std::string &team02,
			   int stride = 5);
		void sample(double clockMin, double clockSec, const CBall &ball,
			    CPlayer **team01Players, int nTeam01Players,
			    CPlayer **team02Players, int nTeam02Players,
			    bool team01Ball);
		void finish(void);
		const std::string &getOutputPath(void) const;
	};

};

#endif
