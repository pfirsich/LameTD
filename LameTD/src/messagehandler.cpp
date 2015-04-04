// Joel Schumacher, 12.01.2010

#include "messagehandler.hpp"

namespace rc { // Recooze
	NullHandler nullHandler;
	AbstractMessageHandler* messageHandler = &nullHandler;
}
