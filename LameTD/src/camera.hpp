#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <algorithm> // port2015: so eigen can use std::min, and std::max
#include <Eigen/Core>

namespace camera {
	extern float heightSpeed;
	extern float heightMoveSmoothness;
	extern int moveBorder;
	extern float moveSpeed;
	extern float lookAngle;
	extern float targetHeight;
	extern Eigen::Vector3f camPos;

	void setup( float heightspeed, float heightmovesmoothness, int moveborder, float movespeed, float lookangle, float targetheight );
	void update( float speed );
}

#endif
