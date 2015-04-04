#ifndef _MAP_HPP_
#define _MAP_HPP_

#include "client.hpp"
#include "object.hpp"
#include <math.h>
#include <algorithm> // port2015: so eigen can use std::min, and std::max
#include <Eigen/Core>
#include "camera.hpp"
#include "texture.h"
#include "ressource.hpp"
#include "graphics.hpp"
#include <stdio.h>

namespace map {
	struct GridField {
		enum {
			EMPTY, NORMAL, RAISED, ROCKS, TREES, WATER, GOAL, SPAWN
		} type;
		unsigned char r, g, b;
		bool occupied;
	};

	extern GridField* mapGrid;
	extern int width, height, occupied, clickable;
	extern rc::Object* ground;
	extern rc::Texture* colorMap;

	void setup( const char* filename );
}

#endif
