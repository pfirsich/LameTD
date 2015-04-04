#include "graphics.hpp"
#include <algorithm> // port2015: so eigen can use std::min, and std::max
#include <Eigen/Core>
#include <stdio.h>

namespace camera {
	float heightSpeed;
	float heightMoveSmoothness;
	int moveBorder;
	float moveSpeed;
	float lookAngle;
	float targetHeight;
	Eigen::Vector3f camPos;

	void setup( float heightspeed, float heightmovesmoothness, int moveborder, float movespeed, float lookangle, float targetheight ) {
		camPos.setZero( );
		heightSpeed = heightspeed;
		heightMoveSmoothness = heightmovesmoothness;
		moveBorder = moveborder;
		moveSpeed = movespeed;
		lookAngle = lookangle;
		targetHeight = targetheight;
	}

	void update( float speed ) {
		static int lastMW = 0;
		if( !lastMW ) lastMW = glfwGetMouseWheel( );

		int mw = glfwGetMouseWheel( );
		targetHeight -= (mw - lastMW) / heightSpeed;
		if( targetHeight < 5.0f ) targetHeight = 5.0f;
		if( targetHeight > 100.0f ) targetHeight = 100.0f;
		lastMW = mw;
		camPos.y( ) += (targetHeight - camPos.y( )) / heightMoveSmoothness * speed;

		int mx, my;
		glfwGetMousePos( &mx, &my );
		if( mx > rc::XRes - moveBorder )
			camPos.x( ) += (mx - (rc::XRes - moveBorder)) / (float)moveBorder * moveSpeed * speed;
		if( mx < moveBorder )
			camPos.x( ) -= (moveBorder - mx) / (float)moveBorder * moveSpeed * speed;
		if( my > rc::YRes - moveBorder )
			camPos.z( ) += (my - (rc::YRes - moveBorder)) / (float)moveBorder * moveSpeed * speed;
		if( my < moveBorder )
			camPos.z( ) -= (moveBorder - my) / (float)moveBorder * moveSpeed * speed;

		glLoadIdentity( );
		glRotatef( lookAngle, 1.0f, 0.0f, 0.0f );
		glTranslatef( -camPos.x( ), -camPos.y( ), -camPos.z( ) );
	}
}
