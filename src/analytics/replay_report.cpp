#include "analytics/replay_report.h"

#include "analytics/replay_manifest.h"

#include <fstream>
#include <sstream>
#include <vector>

namespace soccer {

	static bool readFile(const std::string &path, std::string *body)
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

	static std::string directoryName(const std::string &path)
	{
		std::string::size_type slash = path.find_last_of("/\\");
		if(slash == std::string::npos) {
			return "";
		}

		return path.substr(0, slash);
	}

	static std::string parentDirectory(const std::string &path)
	{
		std::string dir = directoryName(path);
		if(dir == path) {
			return "";
		}

		return dir;
	}

	static std::string joinPath(const std::string &base, const std::string &leaf)
	{
		if(base == "") {
			return leaf;
		}

		char last = base[base.size() - 1];
		if(last == '/' || last == '\\') {
			return base + leaf;
		}

		return base + "/" + leaf;
	}

	bool readReplaySummary(const std::string &manifestPath,
			       std::string *summaryBody,
			       std::string *errorMessage)
	{
		if(!summaryBody) {
			if(errorMessage) {
				*errorMessage = "summary output pointer was null";
			}
			return false;
		}

		ReplayManifest manifest;
		if(!ReplayManifest::load(manifestPath, &manifest)) {
			if(errorMessage) {
				*errorMessage = "could not load replay manifest: " + manifestPath;
			}
			return false;
		}

		if(manifest.summaryPath == "") {
			if(errorMessage) {
				*errorMessage = "replay manifest does not include a summary path";
			}
			return false;
		}

		std::vector<std::string> candidates;
		candidates.push_back(manifest.summaryPath);

		std::string manifestDir = directoryName(manifestPath);
		if(manifestDir != "") {
			candidates.push_back(joinPath(manifestDir, manifest.summaryPath));
			std::string manifestParent = parentDirectory(manifestDir);
			if(manifestParent != "") {
				candidates.push_back(joinPath(manifestParent, manifest.summaryPath));
			}
		}

		for(std::vector<std::string>::const_iterator it = candidates.begin();
		    it != candidates.end(); ++it) {
			if(readFile(*it, summaryBody)) {
				return true;
			}
		}

		if(errorMessage) {
			*errorMessage = "could not read replay summary: " + manifest.summaryPath;
		}
		return false;
	}

};
