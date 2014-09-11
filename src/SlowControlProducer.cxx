#include "eudaq/Producer.hh"
#include "eudaq/Logger.hh"
#include "eudaq/RawDataEvent.hh"
#include "eudaq/Timer.hh"
#include "eudaq/Utils.hh"
#include "eudaq/OptionParser.hh"
#include "eudaq/ExampleHardware.hh"
#include "CommandFactory.hh"
#include "ExceptionFactory.hh"
#include "LazyDeviceReader.hh"
#include "SerialCommunicator.hh"
#include "StringUtility.hh"
#include <array>
#include <iostream>
#include <ostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace deco;

static const std::string EVENT_TYPE = "SlowControlExample";

class SlowControl: public eudaq::Producer {
public:
	SlowControl(const std::string &name, const std::string &runcontrol) :
			eudaq::Producer(name, runcontrol), _run(0), _ev(0), _started(false), _done(false) {
	}

	virtual void OnConfigure(const eudaq::Configuration &config) {
		const int devices = countDevices(config);

		std::stringstream s;
		for (int i = 0; i < devices; i++) {
			char id = 'a' + i;

			s.str("");
			s << id << "_config";
			std::ifstream configFile(config.Get(s.str(), ""), std::ifstream::in);
			eudaq::Configuration deviceProperties(configFile, "CONNECTION");

			s.str("");
			s << id << "_port";
			std::string port = config.Get(s.str(), "");

			if (port == "") {
				std::cerr << ExceptionFactory::generateMessage("Malformed config file: no port found!", __FILE__, __LINE__) << std::endl;
				SetStatus(eudaq::Status::LVL_ERROR, "Malformed config file: " + config.Name());
				return;
			}

			std::shared_ptr<SerialCommunicator> sc = createSerialCommunicator(deviceProperties, port);
			try {
				sc->connect();
			} catch(std::runtime_error &e) {
				std::cerr << ExceptionFactory::generateMessage("Error while trying to connect, due to...", __FILE__, __LINE__) << std::endl;
				std::cerr << e.what() << std::endl;
				SetStatus(eudaq::Status::LVL_ERROR, "Malformed config file: " + config.Name());
				return;
			}

			try {
				deviceProperties.SetSection("COMMANDS");
				executeInitialCommands(sc, config, deviceProperties, id);
				LazyDeviceReader ldr = createLazyDeviceReader(sc, config, deviceProperties, id);
				_devices.push_back(ldr);
			} catch(std::runtime_error &e) {
				std::cerr << e.what() << std::endl;

				SetStatus(eudaq::Status::LVL_ERROR, "Malformed config file: " + config.Name());
				return;
			}
		}

		if (devices <= 0) {
			std::cerr << ExceptionFactory::generateMessage("Malformed config file: no devices found!", __FILE__, __LINE__) << std::endl;
			SetStatus(eudaq::Status::LVL_ERROR, "Malformed config file: " + config.Name());
			return;
		}

		SetStatus(eudaq::Status::LVL_OK, "Configured (" + config.Name() + ")");
	}

	int countDevices(const eudaq::Configuration &config) const {
		std::stringstream s;

		int i = 0;
		char id;
		bool exists;
		do {
			id = 'a' + i;

			s.str("");
			s << id << "_config";
			exists = config.Get(s.str(), "") != "" ? true : false;

			i++;
		} while (exists);

		return i - 1;
	}

	int countCommands(const eudaq::Configuration &config, char id) const {
		std::stringstream s;

		int i = 1;
		bool exists;
		do {
			s.str("");
			s << id << "_command" << "_" << i;
			exists = config.Get(s.str(), "") != "" ? true : false;

			i++;
		} while (exists);
		return i - 2;
	}

	int countQueries(const eudaq::Configuration &config, char id) const {
		std::stringstream s;

		int i = 1;
		bool exists;
		do {
			s.str("");
			s << id << "_query" << "_" << i;
			exists = config.Get(s.str(), "") != "" ? true : false;

			i++;
		} while (exists);
		return i - 2;
	}

	std::shared_ptr<SerialCommunicator> createSerialCommunicator(const eudaq::Configuration &deviceProperties, const std::string &port) {
		int baudRate = deviceProperties.Get("baudRate", 0);
		int characterSize = deviceProperties.Get("characterSize", 0);
		bool stopBits = (deviceProperties.Get("sendTwoStopBits", "false").compare("true")) ? true : false;
		bool parity = (deviceProperties.Get("enableParity", "false").compare("true")) ? true : false;

		return std::make_shared<SerialCommunicator>(port, baudRate, characterSize, stopBits, parity, 10);
	}

	void executeInitialCommands(std::shared_ptr<SerialCommunicator> sc, const eudaq::Configuration &config, const eudaq::Configuration &deviceProperties,
			const char ID) throw(std::runtime_error) {
		const int N = countCommands(config, ID);

		std::stringstream s;
		for (int j = 1; j <= N; j++) {
			s.str("");
			s << ID << "_command" << "_" << j;
			std::string command = config.Get(s.str(), "");
			if (command == "") {
				throw std::runtime_error(ExceptionFactory::generateMessage("Malformed config file: command \'" + s.str() + "\' not found!", __FILE__, __LINE__));
			}

			std::pair<std::string, std::string> x = StringUtility::splitByFirstOccurrence(command, ',');

			std::string key = StringUtility::trim(x.first);
			std::string value = StringUtility::trim(x.second);

			key = deviceProperties.Get(key, "");
			if (key == "") {
				throw std::runtime_error(ExceptionFactory::generateMessage("Malformed config file: could not execute initial command \'" + StringUtility::trim(x.first) + "\' (value: \'" + value + "\') - unknown command!?", __FILE__, __LINE__));
			}

			try {
				Query query = CommandFactory::generateQuery(key, value);
				sc->send(query);
			} catch (std::runtime_error &e) {
				throw e;
			}
		}
	}

	LazyDeviceReader createLazyDeviceReader(std::shared_ptr<SerialCommunicator> sc, const eudaq::Configuration &config,
			const eudaq::Configuration &deviceProperties, const char ID) throw(std::runtime_error) {
		const int N = countQueries(config, ID);

		std::stringstream s;
		std::vector<LazyDeviceReader::Command> commands;
		for (int j = 1; j <= N; j++) {
			s.str("");
			s << ID << "_query" << "_" << j;
			std::string command = config.Get(s.str(), "");
			if (command == "") {
				throw std::runtime_error(ExceptionFactory::generateMessage("Malformed config file: could not find query \'" + s.str() + "\'!", __FILE__, __LINE__));
			}

			std::pair<std::string, std::string> x = StringUtility::splitByFirstOccurrence(command, ',');
			std::pair<std::string, std::string> y = StringUtility::splitByFirstOccurrence(x.second, ',');

			std::string v1 = StringUtility::trim(x.first);
			std::string v2 = StringUtility::trim(y.first);
			std::string v3 = StringUtility::trim(y.second);

			s.str("");
			s << v1 << ".command";
			std::string p1 = deviceProperties.Get(s.str(), "");
			if (p1 == "") {
				throw std::runtime_error(ExceptionFactory::generateMessage("Malformed config file: could not find \'" + s.str() + "\'!", __FILE__, __LINE__));
			}

			s.str("");
			s << v1 << ".regexp";
			std::string p2 = deviceProperties.Get(s.str(), "");
			if (p2 == "") {
				throw std::runtime_error(ExceptionFactory::generateMessage("Malformed config file: could not find \'" + s.str() + "\'!", __FILE__, __LINE__));
			}

			s.str("");
			s << v1 << ".pattern";
			std::string p3 = deviceProperties.Get(s.str(), "");
			if (p3 == "") {
				throw std::runtime_error(ExceptionFactory::generateMessage("Malformed config file: could not find \'" + s.str() + "\'!", __FILE__, __LINE__));
			}

			RichQuery query = CommandFactory::generateQuery(p1, p2, p3);

			LazyDeviceReader::Command c = { query, v2, std::stol(v3) };
			commands.push_back(c);
		}

		return LazyDeviceReader(sc, commands);
	}

	virtual void OnStartRun(unsigned param) {
		_run = param;
		_ev = 0;

		_started = true;

		eudaq::RawDataEvent bore(eudaq::RawDataEvent::BORE(EVENT_TYPE, _run));
		SendEvent(bore);

		SetStatus(eudaq::Status::LVL_OK, "Running");
	}

	virtual void OnStopRun() {
		_started = false;

		SendEvent(eudaq::RawDataEvent::EORE("Test", _run, ++_ev));
	}

	virtual void OnTerminate() {
		_done = true;
	}

	void ReadoutLoop() {
		long c = 0;
		while (!_done) {
			eudaq::mSleep(100);

			if (!_started) {
				continue;
			}

			std::vector<std::pair<std::string, std::string>> data;
			for (int i = 0; i < _devices.size(); i++) {
				_devices[i].lazyEnrichData(data);
			}

			if (data.size() == 0) {
				continue;
			}

			eudaq::RawDataEvent event(EVENT_TYPE, _run, _ev);
			for (std::pair<std::string, std::string> p : data) {
				std::cout << "[" << c++ << "] " << p.first << ": " << p.second << std::endl;
				event.SetTag(p.first, p.second);
			}

			SendEvent(event);
		}
	}

private:
	bool _started, _done;
	unsigned _run, _ev;
	std::vector<LazyDeviceReader> _devices;
};

int main(int, const char **argv) {
	eudaq::OptionParser op("EUDAQ SlowControl Producer", "1.0");
	eudaq::Option<std::string> rctrl(op, "r", "runcontrol", "tcp://localhost:44000", "address", "The address of the RunControl.");
	eudaq::Option<std::string> level(op, "l", "log-level", "NONE", "level", "The minimum level for displaying log messages locally");
	eudaq::Option<std::string> name(op, "n", "name", "SlowControlExample", "string", "The name of this Producer");
	try {
		op.Parse(argv);
		EUDAQ_LOG_LEVEL(level.Value());

		SlowControl producer(name.Value(), rctrl.Value());
		producer.ReadoutLoop();
	} catch (...) {
		return op.HandleMainException();
	}

	return 0;
}
