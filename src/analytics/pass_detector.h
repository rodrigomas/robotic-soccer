#ifndef PASS_DETECTOR_H
#define PASS_DETECTOR_H

#include <string>
#include <vector>

#include "vector3d.h"

namespace soccer {

	typedef struct {
		int tick;
		double matchTime;
		std::string possessionTeam;
		int carrierNumber;
		std::string carrierName;
		CVector3D carrierPos;
		CVector3D ballPos;
	} PassDetectorSample;

	typedef struct {
		int tick;
		double matchTime;
		std::string eventType;
		std::string fromTeam;
		int fromNumber;
		std::string fromName;
		std::string toTeam;
		int toNumber;
		std::string toName;
		double passDistance;
		CVector3D ballPos;
	} PassDetectorEvent;

	typedef struct {
		int totalEvents;
		int completedPasses;
		int possessionChanges;
		double longestPassDistance;
		std::string longestPassTeam;
		int longestPassFromNumber;
		int longestPassToNumber;
		std::string lastEventType;
		std::string lastFromTeam;
		int lastFromNumber;
		std::string lastToTeam;
		int lastToNumber;
	} PassDetectorSummary;

	class PassDetector {

		bool hasPrevious;
		PassDetectorSample previous;
		std::vector<PassDetectorEvent> events;

		static double distanceXZ(const CVector3D &a, const CVector3D &b);

	public:
		PassDetector();

		void reset(void);
		bool record(const PassDetectorSample &sample);
		const std::vector<PassDetectorEvent> &getEvents(void) const;
		PassDetectorSummary getSummary(void) const;
		bool writeCsv(const std::string &path) const;
	};

};

#endif
