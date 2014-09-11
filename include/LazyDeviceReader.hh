#ifndef __LAZY_DEVICE_READER_H__
#define __LAZY_DEVICER_EADER_H__

#include <chrono>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "Device.hh"

namespace deco {

class LazyDeviceReader {
public:
	struct Command {
		RichQuery query;
		std::string tag;
		long readouttime;
	};

	LazyDeviceReader(std::shared_ptr<Device>, struct Command &);
	LazyDeviceReader(std::shared_ptr<Device>, std::vector<struct Command> &commands);
	void lazyEnrichData(std::vector<std::pair<std::string, std::string>> &) throw (std::runtime_error);

private:
	std::shared_ptr<Device> _device;
	std::vector<struct Command> _commands;
	std::vector<std::chrono::milliseconds> _time;
	bool _firstReadout;

	std::chrono::milliseconds currentTime(void);
	std::pair<std::string, std::string> readoutAndSetTime(const unsigned ID);
	std::shared_ptr<Device> device(void);
};
}

#endif
