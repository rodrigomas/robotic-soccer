#ifndef MATCH_HEATMAP_H
#define MATCH_HEATMAP_H

#include <string>
#include <vector>

#include "vector3d.h"

namespace soccer {

	class MatchHeatmap {

		typedef struct {
			std::string entityType;
			std::string teamName;
			int number;
			std::string name;
			std::vector<int> cells;
		} Series;

		double minX;
		double maxX;
		double minZ;
		double maxZ;
		int columns;
		int rows;
		std::vector<Series> series;

		static std::string csv(const std::string &value);
		int cellIndex(const CVector3D &pos) const;
		Series *findSeries(const std::string &entityType,
				   const std::string &teamName,
				   int number,
				   const std::string &name);

	public:
		MatchHeatmap();

		void reset(double fieldMinX, double fieldMaxX,
			   double fieldMinZ, double fieldMaxZ,
			   int gridColumns, int gridRows);
		void record(const std::string &entityType,
			    const std::string &teamName,
			    int number,
			    const std::string &name,
			    const CVector3D &pos);
		bool writeCsv(const std::string &path) const;
	};

};

#endif
