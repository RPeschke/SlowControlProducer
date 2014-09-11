#ifndef __SERIAL_COMMUNICATOR_H__
#define __SERIAL_COMMUNICATOR_H__

#include <stdexcept>
#include <string>

#ifdef WIN32
#include <windows.h>
typedef HANDLE PORTHANDLE;
#else
typedef int PORTHANDLE;
#endif

#include "Device.hh"

namespace deco {

class SerialCommunicator: public Device {
public:
	SerialCommunicator(std::string port, int baudRate, int characterSize, bool sendTwoStopBits, bool enableParity, char commandTermination);
	~SerialCommunicator(void);
	void connect(void) throw (std::runtime_error);
	bool connected(void) const;
	void disconnect(void) throw (std::runtime_error);
	void send(const Query &) throw (std::runtime_error);
	std::string plainRead(void) throw (std::runtime_error);
	std::string query(const RichQuery &) throw (std::runtime_error);
	void defaultSleep(const long milliseconds);

private:
	std::string _port;
	int _baudRate;
	int _characterSize;
	bool _stopBits;
	int _parity;
	int _connected;
	PORTHANDLE _fd;
	size_t _sizeOfReadString;
	long _sleep;
	char _commandTermination;

	int getBaudRate(int) throw (std::invalid_argument);

#ifndef WIN32
	int characterSizeMask(int) throw (std::invalid_argument);
	int stopBitsMask(bool) throw (std::invalid_argument);
	int parityMask(bool) throw (std::invalid_argument);
#endif
};
}

#endif
