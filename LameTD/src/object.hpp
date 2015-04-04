#ifndef _OBJECT_HPP_
#define _OBJECT_HPP_

#include <algorithm> //  port2015: so eigen can use std::min, and std::max
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "texture.h"
#include "mesh.hpp"
#include "renderable.hpp"
#include "shader.hpp"

namespace rc {
	class Object : public Renderable {
	public:
		enum Blending {
			NONE = 0, ADD, TRANSPARENCY
		};

	private:
		static Blending lastBlending;

		AbstractTexture* mTexture[4];
		Mesh* mMesh;
		Shader* mShader;
		Eigen::Vector3f mPosition, mScale, mOrientation, mColor;
		bool mEnabled, mDontCull;
		unsigned int mHash;
		Blending mBlending;

		void updateHash( ) {
			int shader = 0;
			if( mShader ) shader = mShader->program( );
			int texture = 0;
			if( mTexture[0] ) texture = mTexture[0]->object( );
			int blend = (int)mBlending;

			mHash = (blend << 30) | ((shader & 0x3FFF) << 16) | (texture & 0xFFFF);
		}

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
		static std::list<Object*> objects;
		static float* MVP;
		static Eigen::Transform3f Transform;

		Object( ) : mMesh(0), mShader(0), mEnabled(true), mDontCull(false), mHash(0), mBlending(NONE) {
			mTexture[0] = mTexture[1] = mTexture[2] = mTexture[3] = 0;
			mPosition.setZero( ); mOrientation.setZero( ); mScale.setOnes( ); mColor.setOnes( );
			objects.push_back( this );
		}
		~Object( ) { objects.remove( this ); }
		void setMesh( Mesh* mesh ) { mMesh = mesh; }
		Mesh* getMesh( ) const { return mMesh; }
		void setTexture( AbstractTexture* tex, int stage = 0 ) { mTexture[stage] = tex; if( !stage ) updateHash( ); }
		AbstractTexture* getTexture( int stage = 0 ) const { return mTexture[stage]; }
		void setShader( Shader* shader ) { mShader = shader; updateHash( ); }
		Shader* getShader( ) const { return mShader; }
		void position( const Eigen::Vector3f& pos ) { mPosition = pos; }
		Eigen::Vector3f position( ) const { return mPosition; }
		void orientation( const Eigen::Vector3f& ori ) { mOrientation = ori; }
		Eigen::Vector3f orientation( ) const { return mOrientation; }
		void scale( const Eigen::Vector3f scale ) { mScale = scale; }
		Eigen::Vector3f scale( ) const { return mScale; }
		void enabled( bool e ) { mEnabled = e; }
		bool enabled( ) const { return mEnabled; }
		void dontCull( bool c ) { mDontCull = c; }
		bool dontCull( ) const { return mDontCull; }
		void color( const Eigen::Vector3f c ) { mColor = c; }
		Eigen::Vector3f color( ) const { return mColor; }
		unsigned int hash( ) const { return mHash; }
		Blending blending( ) const { return mBlending; }
		void blending( Blending b ) { mBlending = b; updateHash( ); }
		void render( );
	};
}

#endif
