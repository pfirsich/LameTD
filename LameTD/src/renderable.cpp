#include "renderable.hpp"

namespace rc {
	std::list<Renderable*> Renderable::renderables;
	int Renderable::rendered;
	bool Renderable::insertSorted = false;

	bool pRenderablePredicate( const rc::Renderable* a, const rc::Renderable* b ) {
		return a->hash( ) < b->hash( );
	}

	Renderable::Renderable( ) {
		if( insertSorted ) {
			std::list<Renderable*>::iterator first, second;
			first = second = renderables.begin( );;
			for(; second != renderables.end( ); ++second ) {
				if( !pRenderablePredicate( *first, *second ) ) break;
				first = second;
			};
			renderables.insert( second, this );
		} else {
			renderables.push_back( this );
		};
	}

	void Renderable::finishInitialization( ) {
		renderables.sort( pRenderablePredicate );
		insertSorted = true;
	};
}
