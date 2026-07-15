#include "analytics/replay_catalog.h"

#include <cstdlib>
#include <fstream>
#include <sstream>

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

	static bool readStringField(const std::string &body,
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

	static bool readIntField(const std::string &body,
				 const std::string &name,
				 int *value)
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

		const char *start = body.c_str() + colon + 1;
		char *end = NULL;
		long parsed = std::strtol(start, &end, 10);
		if(end == start) {
			return false;
		}

		*value = static_cast<int>(parsed);
		return true;
	}

	static bool readArrayField(const std::string &body,
				   const std::string &name,
				   std::string *arrayBody)
	{
		if(!arrayBody) {
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

		std::string::size_type start = body.find('[', colon + 1);
		if(start == std::string::npos) {
			return false;
		}

		bool inString = false;
		bool escaped = false;
		int depth = 0;
		for(std::string::size_type i = start; i < body.length(); i++) {
			char c = body[i];
			if(inString) {
				if(escaped) {
					escaped = false;
				} else if(c == '\\') {
					escaped = true;
				} else if(c == '"') {
					inString = false;
				}
			} else if(c == '"') {
				inString = true;
			} else if(c == '[') {
				depth++;
			} else if(c == ']') {
				depth--;
				if(depth == 0) {
					*arrayBody = body.substr(start + 1, i - start - 1);
					return true;
				}
			}
		}

		return false;
	}

	static bool nextObject(const std::string &body,
			       std::string::size_type *offset,
			       std::string *objectBody)
	{
		if(!offset || !objectBody) {
			return false;
		}

		std::string::size_type start = body.find('{', *offset);
		if(start == std::string::npos) {
			return false;
		}

		bool inString = false;
		bool escaped = false;
		int depth = 0;
		for(std::string::size_type i = start; i < body.length(); i++) {
			char c = body[i];
			if(inString) {
				if(escaped) {
					escaped = false;
				} else if(c == '\\') {
					escaped = true;
				} else if(c == '"') {
					inString = false;
				}
			} else if(c == '"') {
				inString = true;
			} else if(c == '{') {
				depth++;
			} else if(c == '}') {
				depth--;
				if(depth == 0) {
					*objectBody = body.substr(start, i - start + 1);
					*offset = i + 1;
					return true;
				}
			}
		}

		return false;
	}

	bool loadReplayCatalog(const std::string &catalogPath,
			       std::vector<ReplayCatalogEntry> *entries,
			       std::string *errorMessage)
	{
		if(!entries) {
			if(errorMessage) {
				*errorMessage = "catalog entries pointer was null";
			}
			return false;
		}

		std::string body;
		if(!readFile(catalogPath, &body)) {
			if(errorMessage) {
				*errorMessage = "could not read replay catalog: " + catalogPath;
			}
			return false;
		}

		std::string format;
		int formatVersion = 0;
		if(!readStringField(body, "format", &format) ||
		   !readIntField(body, "format_version", &formatVersion) ||
		   format != "robotic-soccer-replay-catalog" ||
		   formatVersion != 1) {
			if(errorMessage) {
				*errorMessage = "replay catalog format was not recognized";
			}
			return false;
		}

		std::string arrayBody;
		if(!readArrayField(body, "replays", &arrayBody)) {
			if(errorMessage) {
				*errorMessage = "replay catalog did not include replay entries";
			}
			return false;
		}

		std::vector<ReplayCatalogEntry> parsed;
		std::string::size_type offset = 0;
		std::string objectBody;
		while(nextObject(arrayBody, &offset, &objectBody)) {
			ReplayCatalogEntry entry;
			if(!readStringField(objectBody, "id", &entry.id) ||
			   !readStringField(objectBody, "title", &entry.title) ||
			   !readStringField(objectBody, "manifest", &entry.manifestPath) ||
			   !readStringField(objectBody, "team01", &entry.team01Name) ||
			   !readStringField(objectBody, "team02", &entry.team02Name) ||
			   !readStringField(objectBody, "description", &entry.description)) {
				if(errorMessage) {
					*errorMessage = "replay catalog entry was incomplete";
				}
				return false;
			}
			parsed.push_back(entry);
		}

		if(parsed.empty()) {
			if(errorMessage) {
				*errorMessage = "replay catalog did not contain any entries";
			}
			return false;
		}

		*entries = parsed;
		return true;
	}

};
