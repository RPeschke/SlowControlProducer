#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <stdexcept>
#include <string>

#include "RichQuery.hh"

namespace deco {

class Device {
public:
	virtual ~Device(void) {};
	virtual void connect(void) throw (std::runtime_error) = 0;
	virtual bool connected(void) const = 0;
	virtual void disconnect(void) throw (std::runtime_error) = 0;
	virtual void send(const Query &) throw (std::runtime_error) = 0;
	virtual std::string plainRead(void) throw (std::runtime_error) = 0;
	virtual std::string query(const RichQuery &) throw (std::runtime_error) = 0;
};
}

#endif
