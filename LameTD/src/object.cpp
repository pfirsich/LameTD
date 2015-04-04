#include "object.hpp"
#include <stdio.h>
#include <algorithm> // port2015: so eigen can use std::min, and std::max
#include <Eigen/Core>

namespace rc {
	std::list<Object*> Object::objects;
	Object::Blending Object::lastBlending = NONE;
	float* Object::MVP = 0;
	Eigen::Transform3f Object::Transform;

	void putMatrix( float* mat ) {
		printf( "-----------------\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n",
				mat[0], mat[4], mat[8], mat[12], mat[1], mat[5], mat[9], mat[13],
				mat[2], mat[6], mat[10], mat[14], mat[3], mat[7], mat[11], mat[15] );
	}

	void Object::render( ) {
		if( mEnabled ) {
			++Renderable::rendered;
			glPushMatrix( );

			Transform =	Eigen::Translation3f( mPosition ) *
						Eigen::AngleAxisf( mOrientation.x( ), Eigen::Vector3f::UnitX( ) ) *
						Eigen::AngleAxisf( mOrientation.y( ), Eigen::Vector3f::UnitY( ) ) *
						Eigen::AngleAxisf( mOrientation.z( ), Eigen::Vector3f::UnitZ( ) ) *
						Eigen::Scaling3f( mScale );
			glMultMatrixf( Transform.data( ) );

			if( mMesh ) {
				if( mTexture[0] ) {
					glActiveTexture( GL_TEXTURE0 );
					glEnable( GL_TEXTURE_2D );
					for( int i = 0; i < 4; ++i ) if( mTexture[i] ) mTexture[i]->bind( i );
				}

				mShader->use( );

				if( mBlending != lastBlending ) {
					switch( mBlending ) {
					case NONE:
						glDisable( GL_BLEND );
						break;
					case ADD:
						glBlendFunc( GL_ONE, GL_ONE );
						glEnable( GL_BLEND );
						break;
					case TRANSPARENCY:
						glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
						glEnable( GL_BLEND );
						break;
					};
					lastBlending = mBlending;
				};

				mMesh->render( );

				if( mTexture[0] ) glDisable( GL_TEXTURE_2D );
			}

			glPopMatrix( );
		};
	}
}
