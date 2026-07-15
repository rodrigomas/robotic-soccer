#ifndef REPLAY_MANIFEST_H
#define REPLAY_MANIFEST_H

#include <stdint.h>
#include <string>

namespace soccer {

	class ReplayManifest {

		static std::string json(const std::string &value);
		static bool readFile(const std::string &path, std::string *body);
		static bool readStringField(const std::string &body,
					    const std::string &name,
					    std::string *value);
		static bool readNumberField(const std::string &body,
					    const std::string &name,
					    uint32_t *value);
		static bool readIntField(const std::string &body,
					 const std::string &name,
					 int *value);
		static bool readTeamName(const std::string &body,
					 const std::string &slot,
					 std::string *value);
		static bool readFilePath(const std::string &body,
					 const std::string &name,
					 std::string *value);

	public:
		std::string format;
		int formatVersion;
		std::string runId;
		std::string createdAt;
		uint32_t randomSeed;
		std::string team01Name;
		std::string team02Name;
		int sampleStride;
		std::string metadataPath;
		std::string snapshotsPath;
		std::string eventsPath;
		std::string derivedEventsPath;
		std::string heatmapPath;
		std::string metricsPath;
		std::string pressurePath;

		ReplayManifest();

		bool write(const std::string &path) const;
		static bool load(const std::string &path, ReplayManifest *manifest);
	};

};

#endif
