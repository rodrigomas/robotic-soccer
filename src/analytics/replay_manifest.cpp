#include "analytics/replay_manifest.h"

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <sstream>

namespace soccer {

	ReplayManifest::ReplayManifest() :
		format("robotic-soccer-replay-manifest"),
		formatVersion(1),
		randomSeed(0),
		sampleStride(1)
	{
	}

	std::string ReplayManifest::json(const std::string &value)
	{
		std::string result = "\"";

		for(std::string::const_iterator it = value.begin(); it != value.end(); ++it) {
			switch(*it) {
				case '"':
					result += "\\\"";
					break;
				case '\\':
					result += "\\\\";
					break;
				case '\b':
					result += "\\b";
					break;
				case '\f':
					result += "\\f";
					break;
				case '\n':
					result += "\\n";
					break;
				case '\r':
					result += "\\r";
					break;
				case '\t':
					result += "\\t";
					break;
				default:
					result += *it;
					break;
			}
		}

		result += "\"";
		return result;
	}

	bool ReplayManifest::readFile(const std::string &path, std::string *body)
	{
		if(!body) {
			return false;
		}

		std::ifstream in(path.c_str());
		if(!in.is_open()) {
			return false;
		}

		std::ostringstream out;
		out << in.rdbuf();
		*body = out.str();
		return true;
	}

	bool ReplayManifest::readStringField(const std::string &body,
					     const std::string &name,
					     std::string *value)
	{
		if(!value) {
			return false;
		}

		std::string key = "\"" + name + "\"";
		std::string::size_type keyPos = body.find(key);
		if(keyPos == std::string::npos) {
			return false;
		}

		std::string::size_type colon = body.find(':', keyPos + key.length());
		if(colon == std::string::npos) {
			return false;
		}

		std::string::size_type quote = body.find('"', colon + 1);
		if(quote == std::string::npos) {
			return false;
		}

		std::string result;
		bool escaped = false;
		for(std::string::size_type i = quote + 1; i < body.length(); i++) {
			char c = body[i];
			if(escaped) {
				switch(c) {
					case 'b':
						result += '\b';
						break;
					case 'f':
						result += '\f';
						break;
					case 'n':
						result += '\n';
						break;
					case 'r':
						result += '\r';
						break;
					case 't':
						result += '\t';
						break;
					default:
						result += c;
						break;
				}
				escaped = false;
			} else if(c == '\\') {
				escaped = true;
			} else if(c == '"') {
				*value = result;
				return true;
			} else {
				result += c;
			}
		}

		return false;
	}

	bool ReplayManifest::readNumberField(const std::string &body,
					     const std::string &name,
					     uint32_t *value)
	{
		if(!value) {
			return false;
		}

		std::string key = "\"" + name + "\"";
		std::string::size_type keyPos = body.find(key);
		if(keyPos == std::string::npos) {
			return false;
		}

		std::string::size_type colon = body.find(':', keyPos + key.length());
		if(colon == std::string::npos) {
			return false;
		}

		std::string::size_type start = colon + 1;
		while(start < body.length() &&
		      std::isspace(static_cast<unsigned char>(body[start]))) {
			start++;
		}

		std::string::size_type end = start;
		while(end < body.length() &&
		      std::isdigit(static_cast<unsigned char>(body[end]))) {
			end++;
		}

		if(end == start) {
			return false;
		}

		*value = static_cast<uint32_t>(std::strtoul(body.substr(start, end - start).c_str(),
							   NULL, 10));
		return true;
	}

	bool ReplayManifest::readIntField(const std::string &body,
					  const std::string &name,
					  int *value)
	{
		uint32_t parsed = 0;
		if(!value || !readNumberField(body, name, &parsed)) {
			return false;
		}

		*value = static_cast<int>(parsed);
		return true;
	}

	bool ReplayManifest::readTeamName(const std::string &body,
					  const std::string &slot,
					  std::string *value)
	{
		std::string marker = "\"slot\": \"" + slot + "\"";
		std::string::size_type slotPos = body.find(marker);
		if(slotPos == std::string::npos) {
			return false;
		}

		std::string::size_type objectEnd = body.find('}', slotPos + marker.length());
		if(objectEnd == std::string::npos) {
			return false;
		}

		return readStringField(body.substr(slotPos, objectEnd - slotPos), "name", value);
	}

	bool ReplayManifest::readFilePath(const std::string &body,
					  const std::string &name,
					  std::string *value)
	{
		std::string filesKey = "\"files\"";
		std::string::size_type filesPos = body.find(filesKey);
		if(filesPos == std::string::npos) {
			return false;
		}

		std::string::size_type objectEnd = body.find("\n  }", filesPos + filesKey.length());
		if(objectEnd == std::string::npos) {
			return false;
		}

		return readStringField(body.substr(filesPos, objectEnd - filesPos), name, value);
	}

	bool ReplayManifest::write(const std::string &path) const
	{
		std::ofstream out(path.c_str());

		if(!out.is_open()) {
			return false;
		}

		out << "{\n";
		out << "  \"format\": " << json(format) << ",\n";
		out << "  \"format_version\": " << formatVersion << ",\n";
		out << "  \"run_id\": " << json(runId) << ",\n";
		out << "  \"created_at\": " << json(createdAt) << ",\n";
		out << "  \"random_seed\": " << randomSeed << ",\n";
		out << "  \"teams\": [\n";
		out << "    {\"slot\": \"team01\", \"name\": " << json(team01Name) << "},\n";
		out << "    {\"slot\": \"team02\", \"name\": " << json(team02Name) << "}\n";
		out << "  ],\n";
		out << "  \"sample_stride\": " << sampleStride << ",\n";
		out << "  \"files\": {\n";
		out << "    \"metadata\": " << json(metadataPath) << ",\n";
		out << "    \"snapshots\": " << json(snapshotsPath) << ",\n";
		out << "    \"events\": " << json(eventsPath) << ",\n";
		out << "    \"derived_events\": " << json(derivedEventsPath) << ",\n";
		out << "    \"heatmap\": " << json(heatmapPath) << ",\n";
		out << "    \"metrics\": " << json(metricsPath) << ",\n";
		out << "    \"pressure\": " << json(pressurePath) << ",\n";
		out << "    \"shots\": " << json(shotsPath) << "\n";
		out << "  }\n";
		out << "}\n";

		return true;
	}

	bool ReplayManifest::load(const std::string &path, ReplayManifest *manifest)
	{
		if(!manifest) {
			return false;
		}

		std::string body;
		if(!readFile(path, &body)) {
			return false;
		}

		ReplayManifest parsed;
		if(!readStringField(body, "format", &parsed.format) ||
		   !readIntField(body, "format_version", &parsed.formatVersion) ||
		   !readStringField(body, "run_id", &parsed.runId) ||
		   !readStringField(body, "created_at", &parsed.createdAt) ||
		   !readNumberField(body, "random_seed", &parsed.randomSeed) ||
		   !readTeamName(body, "team01", &parsed.team01Name) ||
		   !readTeamName(body, "team02", &parsed.team02Name) ||
		   !readIntField(body, "sample_stride", &parsed.sampleStride) ||
		   !readFilePath(body, "metadata", &parsed.metadataPath) ||
		   !readFilePath(body, "snapshots", &parsed.snapshotsPath) ||
		   !readFilePath(body, "events", &parsed.eventsPath) ||
		   !readFilePath(body, "heatmap", &parsed.heatmapPath) ||
		   !readFilePath(body, "metrics", &parsed.metricsPath)) {
			return false;
		}

		readFilePath(body, "derived_events", &parsed.derivedEventsPath);
		readFilePath(body, "pressure", &parsed.pressurePath);
		readFilePath(body, "shots", &parsed.shotsPath);

		if(parsed.format != "robotic-soccer-replay-manifest" ||
		   parsed.formatVersion != 1) {
			return false;
		}

		*manifest = parsed;
		return true;
	}

};
