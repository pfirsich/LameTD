#ifndef _CULLING_HPP_
#define _CULLING_HPP_

#include <algorithm> // port2015: so eigen can use std::min, and std::max
#include <Eigen/Core>

namespace culling {
	extern float tanCameraAngleTop;
	extern float tanCameraAngleBottom;
	extern float tanX;
	extern Eigen::Vector3f viewCornerBL, viewCornerBR, viewCornerTL, viewCornerTR, viewCenter;

	void setup( float fov, float aspect, float cameraAngle );
	void update( );
}

#endif
