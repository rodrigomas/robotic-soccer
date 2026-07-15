#include "match_heatmap.h"

#include <fstream>
#include <iomanip>

namespace soccer {

	MatchHeatmap::MatchHeatmap() :
		minX(-45.0),
		maxX(45.0),
		minZ(-60.0),
		maxZ(60.0),
		columns(18),
		rows(24)
	{
	}

	std::string MatchHeatmap::csv(const std::string &value)
	{
		std::string result = "\"";

		for(std::string::const_iterator it = value.begin(); it != value.end(); ++it) {
			if(*it == '"') {
				result += "\"\"";
			} else {
				result += *it;
			}
		}

		result += "\"";
		return result;
	}

	void MatchHeatmap::reset(double fieldMinX, double fieldMaxX,
				 double fieldMinZ, double fieldMaxZ,
				 int gridColumns, int gridRows)
	{
		minX = fieldMinX;
		maxX = fieldMaxX;
		minZ = fieldMinZ;
		maxZ = fieldMaxZ;
		columns = gridColumns > 0 ? gridColumns : 18;
		rows = gridRows > 0 ? gridRows : 24;
		series.clear();
	}

	int MatchHeatmap::cellIndex(const CVector3D &pos) const
	{
		if(pos.x < minX || pos.x > maxX || pos.z < minZ || pos.z > maxZ) {
			return -1;
		}

		double xNorm = (pos.x - minX) / (maxX - minX);
		double zNorm = (pos.z - minZ) / (maxZ - minZ);
		int column = static_cast<int>(xNorm * columns);
		int row = static_cast<int>(zNorm * rows);

		if(column >= columns) {
			column = columns - 1;
		}

		if(row >= rows) {
			row = rows - 1;
		}

		if(column < 0 || row < 0) {
			return -1;
		}

		return row * columns + column;
	}

	MatchHeatmap::Series *MatchHeatmap::findSeries(const std::string &entityType,
						       const std::string &teamName,
						       int number,
						       const std::string &name)
	{
		for(std::vector<Series>::iterator it = series.begin(); it != series.end(); ++it) {
			if(it->entityType == entityType &&
			   it->teamName == teamName &&
			   it->number == number &&
			   it->name == name) {
				return &(*it);
			}
		}

		Series next;
		next.entityType = entityType;
		next.teamName = teamName;
		next.number = number;
		next.name = name;
		next.cells.assign(columns * rows, 0);
		series.push_back(next);

		return &series[series.size() - 1];
	}

	void MatchHeatmap::record(const std::string &entityType,
				  const std::string &teamName,
				  int number,
				  const std::string &name,
				  const CVector3D &pos)
	{
		int index = cellIndex(pos);

		if(index < 0) {
			return;
		}

		Series *target = findSeries(entityType, teamName, number, name);
		target->cells[index]++;
	}

	int MatchHeatmap::getColumns(void) const
	{
		return columns;
	}

	int MatchHeatmap::getRows(void) const
	{
		return rows;
	}

	int MatchHeatmap::getTeamSamples(const std::string &teamName,
					 int column,
					 int row) const
	{
		if(column < 0 || column >= columns || row < 0 || row >= rows) {
			return 0;
		}

		int index = row * columns + column;
		int samples = 0;

		for(std::vector<Series>::const_iterator it = series.begin(); it != series.end(); ++it) {
			if(it->entityType == "player" && it->teamName == teamName) {
				samples += it->cells[index];
			}
		}

		return samples;
	}

	int MatchHeatmap::getTeamMaxSamples(const std::string &teamName) const
	{
		int best = 0;

		for(int row = 0; row < rows; row++) {
			for(int column = 0; column < columns; column++) {
				int samples = getTeamSamples(teamName, column, row);
				if(samples > best) {
					best = samples;
				}
			}
		}

		return best;
	}

	bool MatchHeatmap::writeCsv(const std::string &path) const
	{
		std::ofstream out(path.c_str());

		if(!out.is_open()) {
			return false;
		}

		double cellWidth = (maxX - minX) / columns;
		double cellHeight = (maxZ - minZ) / rows;

		out << "entity_type,team,number,name,column,row,x_min,x_max,z_min,z_max,samples\n";

		for(std::vector<Series>::const_iterator it = series.begin(); it != series.end(); ++it) {
			for(int row = 0; row < rows; row++) {
				for(int column = 0; column < columns; column++) {
					int index = row * columns + column;
					int samples = it->cells[index];

					if(samples <= 0) {
						continue;
					}

					double x0 = minX + column * cellWidth;
					double x1 = x0 + cellWidth;
					double z0 = minZ + row * cellHeight;
					double z1 = z0 + cellHeight;

					out << csv(it->entityType) << ","
						<< csv(it->teamName) << ","
						<< it->number << ","
						<< csv(it->name) << ","
						<< column << ","
						<< row << ","
						<< std::fixed << std::setprecision(5)
						<< x0 << ","
						<< x1 << ","
						<< z0 << ","
						<< z1 << ","
						<< samples << "\n";
				}
			}
		}

		return true;
	}

};
