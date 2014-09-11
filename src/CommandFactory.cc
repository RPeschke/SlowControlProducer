#include "CommandFactory.hh"

#include <regex>

#include "ExceptionFactory.hh"

using namespace deco;

Query CommandFactory::generateQuery(const std::string &command) {
	return Query(command);
}

Query CommandFactory::generateQuery(const std::string &command, const std::string &arg, const char target) {
	std::vector<std::string> v;
	v.push_back(arg);

	return Query(replace(command, target, v));
}

Query CommandFactory::generateQuery(const std::string &command, const std::vector<std::string> &args, const char target) {
	return Query(replace(command, target, args));
}

RichQuery CommandFactory::generateQuery(const std::string &command, const std::string &regexp, const std::string &pattern) {
	return RichQuery(command, regexp, pattern);
}

RichQuery CommandFactory::generateQuery(const std::string &command, const std::string &regexp, const std::string &pattern, const std::string &arg,
		const char target) {
	std::vector<std::string> v;
	v.push_back(arg);

	return RichQuery(replace(command, target, v), regexp, pattern);
}

RichQuery CommandFactory::generateQuery(const std::string &command, const std::string &regexp, const std::string &pattern,
		const std::vector<std::string> &args, const char target) {
	return RichQuery(replace(command, target, args), regexp, pattern);
}

std::string CommandFactory::generateAnswer(const std::string &answer, const std::string &regexp, const std::string &pattern, const char target)
		throw (std::invalid_argument) {
	std::regex rgx;
	try {
		rgx = std::regex(regexp);
	} catch (...) {
		throw std::invalid_argument(
				ExceptionFactory::generateMessage("Regular Expression \'" + regexp + "\' does not compile!", "CommandFactory.cc", __LINE__));
	}

	std::string input = answer;
	std::vector<std::string> matches;
	for (std::smatch match; std::regex_search(input, match, rgx); input = match.suffix().str()) {
		matches.push_back(match[0]);
	}

	return replace(pattern, target, matches);
}

std::string CommandFactory::replace(std::string pattern, const char target, const std::vector<std::string> &args) {
	pattern += ";";
	bool qmark = false;
	int x = 0;
	std::string out("");
	size_t c = 0;
	for (size_t i = 0; i < pattern.length(); i++) {
		if (c >= args.size()) {
			out += pattern[i];
		} else if (pattern[i] == target) {
			qmark = true;
		} else {
			if (qmark) {
				if (pattern[i] >= '0' && pattern[i] <= '9')
					x = x * 10 + pattern[i] - '0';
				else {
					qmark = false;
					out += args[c++].substr(x) + pattern[i];
					x = 0;
				}
			} else
				out += pattern[i];
		}
	}

	return out.substr(0, out.length() - 1);
}
