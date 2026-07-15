#include "analytics/replay_report.h"

#include <iostream>
#include <string>

int main(int argc, char **argv)
{
	if(argc != 2 && argc != 3) {
		std::cerr << "usage: replay_report [--json|--text] <replay_manifest.json>\n";
		return 2;
	}

	std::string mode = "--json";
	std::string manifestPath = argv[1];
	if(argc == 3) {
		mode = argv[1];
		manifestPath = argv[2];
	}

	if(mode != "--json" && mode != "--text") {
		std::cerr << "replay_report: unknown mode " << mode << "\n";
		return 2;
	}

	if(mode == "--text") {
		soccer::ReplayReportSummary summary;
		std::string error;
		if(!soccer::loadReplaySummary(manifestPath, &summary, &error)) {
			std::cerr << "replay_report: " << error << "\n";
			return 1;
		}

		std::cout << soccer::formatReplaySummaryText(summary);
		return 0;
	}

	std::string body;
	std::string error;
	if(!soccer::readReplaySummary(manifestPath, &body, &error)) {
		std::cerr << "replay_report: " << error << "\n";
		return 1;
	}

	std::cout << body;
	if(body.empty() || body[body.size() - 1] != '\n') {
		std::cout << "\n";
	}

	return 0;
}
