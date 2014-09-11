#ifndef __COMMAND_FACTORY_H__
#define __COMMAND_FACTORY_H__

#include <stdexcept>
#include <string>
#include <vector>

#include "Query.hh"
#include "RichQuery.hh"

namespace deco {
class CommandFactory {
public:
	static const char TARGET = '$';

	static Query generateQuery(const std::string &command);
	static Query generateQuery(const std::string &command, const std::string &arg, const char target = TARGET);
	static Query generateQuery(const std::string &command, const std::vector<std::string> &args, const char target = TARGET);
	static RichQuery generateQuery(const std::string &command, const std::string &regexp, const std::string &pattern);
	static RichQuery generateQuery(const std::string &command, const std::string &regexp, const std::string &pattern, const std::string &arg,
			const char target = TARGET);
	static RichQuery generateQuery(const std::string &command, const std::string &regexp, const std::string &pattern,
			const std::vector<std::string> &args, const char target = TARGET);
	static std::string generateAnswer(const std::string &answer, const std::string &regexp, const std::string &pattern, const char target = TARGET)
			throw (std::invalid_argument);

private:
	static std::string replace(std::string pattern, const char target, const std::vector<std::string> &args);
};
}

#endif
