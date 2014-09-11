#ifndef STRINGUTILITY_H_
#define STRINGUTILITY_H_

#include <string>

namespace deco {
class StringUtility {
public:
	static std::string trim(const std::string &);
	static std::pair<std::string, std::string> splitByFirstOccurrence(const std::string &, const char);
	static std::string toLowerCase(const std::string &);
};
}

#endif
