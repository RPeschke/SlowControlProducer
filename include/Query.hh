#ifndef __QUERY_H__
#define __QUERY_H__

#include <string>

namespace deco {
class Query {
public:
	Query(const std::string &command);
	std::string command(void) const;

private:
	std::string _command;
};
}

#endif
