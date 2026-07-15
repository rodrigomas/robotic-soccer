#ifndef MATCH_METRICS_H
#define MATCH_METRICS_H

#include <string>
#include <vector>

#include "vector3d.h"

namespace soccer {

	class MatchMetrics {

		typedef struct {
			std::string entityType;
			std::string teamName;
			int number;
			std::string name;
			int samples;
			double distance;
			double speedSum;
			double maxSpeed;
			bool hasPrevious;
			CVector3D previousPos;
		} Series;

		std::vector<Series> series;

		static std::string csv(const std::string &value);
		Series *findSeries(const std::string &entityType,
				   const std::string &teamName,
				   int number,
				   const std::string &name);

	public:
		MatchMetrics();

		void reset(void);
		void record(const std::string &entityType,
			    const std::string &teamName,
			    int number,
			    const std::string &name,
			    const CVector3D &pos,
			    const CVector3D &vel);
		bool writeCsv(const std::string &path) const;
	};

};

#endif
