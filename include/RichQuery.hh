#ifndef __RICH_QUERY_H__
#define __RICH_QUERY_H__

#include "Query.hh"

namespace deco {
class RichQuery: public Query {
public:
	RichQuery(const std::string &command, const std::string &regexp, const std::string &pattern);
	std::string regexp(void) const;
	std::string pattern(void) const;

private:
	std::string _regexp;
	std::string _pattern;
};
}

#endif
