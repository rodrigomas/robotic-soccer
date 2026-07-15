#ifndef REPLAY_CATALOG_H
#define REPLAY_CATALOG_H

#include <string>
#include <vector>

namespace soccer {

	struct ReplayCatalogEntry {
		std::string id;
		std::string title;
		std::string manifestPath;
		std::string team01Name;
		std::string team02Name;
		std::string description;
	};

	bool loadReplayCatalog(const std::string &catalogPath,
			       std::vector<ReplayCatalogEntry> *entries,
			       std::string *errorMessage);

};

#endif
