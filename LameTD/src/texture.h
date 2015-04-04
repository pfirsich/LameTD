// Joel Schumacher, 01.02.2010

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <SOIL.h>
#include <GL/GLee.h>
#include <string>
#include "ressource.hpp"

namespace rc {
	class AbstractTexture {
	protected:
		GLuint mTo;

		static GLuint lastBoundTexture[4];

	public:
		static int binds;

		AbstractTexture( GLuint o ) : mTo(o) { }
		virtual void bind( int stage = 0 );
		virtual GLuint object( ) const { return mTo; }
	};

	class SimpleTexture : public AbstractTexture {
	public:
		SimpleTexture( ) : AbstractTexture(0) { }
		SimpleTexture( GLuint o ) : AbstractTexture(o) { }
		~SimpleTexture( ) { }
		void object( GLuint o ) { mTo = o; }

	};

	class Texture : public AbstractTexture, Ressource {
		private:
			int mHeight, mWidth;

			static GLenum mMin, mMag, mWrap;
			static float mAni;

			void checkTex( int w, int h );

			std::string mFilename;

		public:
			static const int sampler0;
			static const int sampler1;
			static const int sampler2;
			static const int sampler3;

			Texture( ) : AbstractTexture(0) { }
			inline Texture( const char* filename, unsigned int flags = 0 ) : AbstractTexture(0) { loadFile( filename, flags ); }
			inline Texture( const unsigned char* buffer, int width, int height, int channels, unsigned int flags = 0 ) : AbstractTexture(0) { loadBuffer( buffer, width, height, channels, flags ); }
			inline virtual ~Texture( ) { destroy( ); }
			void destroy( ) { if( mTo ) glDeleteTextures( 1, &mTo ); }
			void load( const char* fname ) { loadFile( fname ); }
			void reload( ) { destroy( ); loadFile( mFilename.c_str( ) ); }
			void loadFile( const char* filename, unsigned int flags = 0 );
			void loadBuffer( const unsigned char* buffer, int width, int height, int channels, unsigned int flags = 0 );
			int height( ) const { return mHeight; }
			int width( ) const { return mWidth; }
			void generateMipMaps( ) const;
			void setFilter( GLenum min, GLenum mag, GLenum wrap, float ani ) const;
			static void setDefaultFilter( GLenum min, GLenum mag, GLenum wrap, float ani );
			static float checkAnisotropy( );

			static void unbind( int stage = 0 ) { glActiveTexture( GL_TEXTURE0 + stage ); glBindTexture( GL_TEXTURE_2D, 0 ); }
	};

}

#endif /* TEXTURE_H_ */
