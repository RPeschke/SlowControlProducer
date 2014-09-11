#include "LazyDeviceReader.hh"

using namespace deco;

LazyDeviceReader::LazyDeviceReader(std::shared_ptr<Device> device, struct Command &command) :
		_device(device), _time(1), _firstReadout(true) {
	_commands.push_back(command);
}

LazyDeviceReader::LazyDeviceReader(std::shared_ptr<Device> device, std::vector<struct Command> &commands) :
		_device(device), _commands(commands), _time(commands.size()), _firstReadout(true) {
}

void LazyDeviceReader::lazyEnrichData(std::vector<std::pair<std::string, std::string>> &data) throw (std::runtime_error) {
	if (_firstReadout) {
		_firstReadout = false;

		for (int i = 0; i < _commands.size(); i++) {
			data.push_back(readoutAndSetTime(i));
		}
	} else {
		for (int i = 0; i < _commands.size(); i++) {
			std::chrono::milliseconds t1 = _time[i];
			std::chrono::milliseconds t2 = currentTime();
			std::chrono::milliseconds dt(_commands[i].readouttime);

			if (t2 - t1 > dt) {
				data.push_back(readoutAndSetTime(i));
			}
		}
	}
}

std::chrono::milliseconds LazyDeviceReader::currentTime(void) {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
}

std::pair<std::string, std::string> LazyDeviceReader::readoutAndSetTime(const unsigned ID) {
	Command command = _commands[ID];

	_time[ID] = currentTime();

	std::string answer = _device->query(command.query);

	return {command.tag, answer};
}

std::shared_ptr<Device> LazyDeviceReader::device(void) {
	return _device;
}
