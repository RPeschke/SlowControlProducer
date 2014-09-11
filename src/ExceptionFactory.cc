#include "ExceptionFactory.hh"

using namespace deco;

std::string ExceptionFactory::generateMessage(const std::string msg, const std::string filename, const int line) {
	return "[" + filename + " - line: " + std::to_string(line) + "] " + msg;
}
