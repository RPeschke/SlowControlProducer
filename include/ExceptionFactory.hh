#ifndef __EXCEPTION_FACTORY_H__
#define __EXCEPTION_FACTORY_H__

#include <string>

namespace deco {
class ExceptionFactory {
public:
	static std::string generateMessage(const std::string msg, const std::string filename, const int line);
};
}

#endif
