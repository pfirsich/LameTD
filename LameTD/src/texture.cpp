// Joel Schumacher, 01.02.2010

#include "texture.h"
#include "messagehandler.hpp"

namespace rc {
	GLuint AbstractTexture::lastBoundTexture[4] = { 0, 0, 0, 0 };
	GLenum Texture::mMin = GL_NEAREST, Texture::mMag = GL_NEAREST, Texture::mWrap = GL_REPEAT;
	float Texture::mAni = 0.0f;

	const int Texture::sampler0 = 0;
	const int Texture::sampler1 = 1;
	const int Texture::sampler2 = 2;
	const int Texture::sampler3 = 3;

	int AbstractTexture::binds;

	void setParams( GLenum min, GLenum mag, GLenum wrap, float ani ) {
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap );
		//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ani );
	}

	void Texture::checkTex( int w, int h ) {
		if( mTo ) {
			mWidth = w;
			mHeight = h;
			setParams( mMin, mMag, mWrap, mAni );
		} else {
			messageHandler->push( Message( Message::MSG_ERROR, Message::TEXTURE_UPLOAD_ERROR, std::string( "Texture could not be uploaded. SOIL-Error: " ) + SOIL_last_result( ) ) );
		}
	}

	void Texture::loadFile( const char* filename, unsigned int flags ) {
		int w, h, c;
		mFilename = filename;
		unsigned char* buf = SOIL_load_image( filename, &w, &h, &c, SOIL_LOAD_AUTO );
		if( buf ) {
			loadBuffer( buf, w, h, c, flags );
			SOIL_free_image_data( buf );
		} else
			messageHandler->push( Message( Message::MSG_ERROR, Message::TEXTURE_LOAD_ERROR, std::string("Texture '") + std::string(filename) + std::string("' could not be loaded. SOIL-Error: " ) + SOIL_last_result( ) ) );
	}

	void Texture::loadBuffer( const unsigned char* buffer, int width, int height, int channels, unsigned int flags ) {
		mTo = SOIL_create_OGL_texture( buffer, width, height, channels, mTo, flags );
		checkTex( width, height );
	}

	float Texture::checkAnisotropy( ) {
		float ret = 0.0;
		if( GLEE_EXT_texture_filter_anisotropic ) glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ret );
		return ret;
	}

	void Texture::generateMipMaps( ) const {
		glBindTexture( GL_TEXTURE_2D, mTo );
		glGenerateMipmapEXT( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, 0 );
	}

	void Texture::setFilter( GLenum min, GLenum mag, GLenum wrap, float ani ) const {
		glBindTexture( GL_TEXTURE_2D, mTo );
		setParams( min, mag, wrap, ani );
		glBindTexture( GL_TEXTURE_2D, 0 );
	}

	void Texture::setDefaultFilter( GLenum min, GLenum mag, GLenum wrap, float ani ) {
		mMin = min; mMag = mag; mWrap = wrap; mAni = ani;
	}

	void AbstractTexture::bind( int stage ) {
			/*if( this == 0 ) {
				glBindTexture( GL_TEXTURE_2D, 0 );
				return;
			}*/

			if( lastBoundTexture[stage] != mTo ) {
				++binds;
				glActiveTexture( GL_TEXTURE0 + stage );
				glBindTexture( GL_TEXTURE_2D, lastBoundTexture[stage] = mTo );
			}
	}
}
