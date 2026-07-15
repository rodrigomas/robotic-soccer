#include "analytics/replay_catalog.h"

#include <iostream>
#include <string>
#include <vector>

using soccer::ReplayCatalogEntry;
using soccer::loadReplayCatalog;

static int fail(const char *message)
{
	std::cerr << "replay_catalog_test: " << message << "\n";
	return 1;
}

int main(void)
{
	std::vector<ReplayCatalogEntry> entries;
	std::string error;
	if(!loadReplayCatalog("../fixtures/replays/index.json", &entries, &error)) {
		return fail("could not load replay catalog");
	}

	if(entries.size() != 1) {
		return fail("catalog entry count was wrong");
	}

	const ReplayCatalogEntry &entry = entries[0];
	if(entry.id != "basic_match" ||
	   entry.title != "Basic Match Fixture" ||
	   entry.manifestPath != "basic_match/replay_basic_match.json" ||
	   entry.team01Name != "Botafogo" ||
	   entry.team02Name != "Flamengo" ||
	   entry.description.find("dashboard") == std::string::npos) {
		return fail("catalog entry fields were wrong");
	}

	return 0;
}
