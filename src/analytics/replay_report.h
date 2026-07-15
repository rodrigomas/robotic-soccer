#ifndef REPLAY_REPORT_H
#define REPLAY_REPORT_H

#include <string>

namespace soccer {

	bool readReplaySummary(const std::string &manifestPath,
			       std::string *summaryBody,
			       std::string *errorMessage);

};

#endif
