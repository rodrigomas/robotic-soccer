#include "analytics/replay_report.h"

#include <iostream>
#include <string>

int main(int argc, char **argv)
{
	if(argc != 2) {
		std::cerr << "usage: replay_report <replay_manifest.json>\n";
		return 2;
	}

	std::string body;
	std::string error;
	if(!soccer::readReplaySummary(argv[1], &body, &error)) {
		std::cerr << "replay_report: " << error << "\n";
		return 1;
	}

	std::cout << body;
	if(body.empty() || body[body.size() - 1] != '\n') {
		std::cout << "\n";
	}

	return 0;
}
