#ifndef _SHADOWS_HPP_
#define _SHADOWS_HPP_

#include "texture.h"

namespace shadows {
	extern float lightMVP[16];
	extern rc::SimpleTexture shadowMap;

	void setup( int mapSize, bool bilinearpcf );
	void update( );
}

#endif
