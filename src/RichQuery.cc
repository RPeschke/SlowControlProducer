#include "RichQuery.hh"

using namespace deco;

RichQuery::RichQuery(const std::string &command, const std::string &regexp, const std::string &pattern) :
		Query(command), _regexp(regexp), _pattern(pattern) {
}

std::string RichQuery::regexp(void) const {
	return _regexp;
}

std::string RichQuery::pattern(void) const {
	return _pattern;
}
