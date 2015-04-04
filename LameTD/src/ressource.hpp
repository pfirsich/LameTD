#ifndef _RESSOURCE_HPP_
#define _RESSOURCE_HPP_

#include <string>
#include <map>

namespace rc {
	class Ressource {
	public:
		virtual void load( const char* ) = 0;
		virtual void reload( ) = 0;
	};
}

#endif
