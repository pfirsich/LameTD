#ifndef _RENDERABLE_HPP_
#define _RENDERABLE_HPP_

#include <list>

namespace rc {
	class Renderable {
	public:
		static int rendered;
		static std::list<Renderable*> renderables;
		static bool insertSorted;

		Renderable( );

		~Renderable( ) {
			renderables.remove( this );
		}

		virtual void render( ) = 0;
		virtual unsigned int hash( ) const = 0;

		static void finishInitialization( );
	};
}

#endif
