#include "shadows.hpp"
#include "renderable.hpp"
#include <GL/GLee.h>
#include "camera.hpp"
#include "culling.hpp"
#include <stdio.h>
#include <list>

namespace shadows {
	int mapRes;
	GLuint shadowMapTexture, depthFBO;
	float lightMVP[16];
	rc::SimpleTexture shadowMap;

	void setup( int res, bool bilinearpcf ) {
		mapRes = res;
		glGenTextures( 1, &shadowMapTexture );
		shadowMap.object( shadowMapTexture );
		glBindTexture( GL_TEXTURE_2D, shadowMapTexture );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, bilinearpcf?GL_LINEAR:GL_NEAREST ); //NOTE: ATI -> XPLOSN
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, res, res, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glGenFramebuffersEXT( 1, &depthFBO );
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, depthFBO );

		glDrawBuffer( GL_NONE );
		glReadBuffer( GL_NONE );
		glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, shadowMapTexture, 0 );

		GLenum status = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
		if( status != GL_FRAMEBUFFER_COMPLETE_EXT ) printf( "Framebuffer incomplete.\n" );

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	void update( ) {
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, depthFBO );
		glColorMask( false, false, false, false );
		glPushAttrib( GL_VIEWPORT_BIT );
		glViewport( 0, 0, mapRes, mapRes );

		glPushMatrix( );
		glLoadIdentity( );
		glMatrixMode( GL_PROJECTION );
		glPushMatrix( );
		glLoadIdentity( );
		glOrtho( 0, mapRes, 0, mapRes, 0.0f, 1.6f * mapRes );
		glTranslatef( 0.5f * mapRes, 0.5f * mapRes, -0.68f * mapRes );
		glRotatef( 60.0f, 1.0f, 0.0f, 0.0f );
		glRotatef( 30.0f, 0.0f, 0.0f, 1.0f );
		float scale = mapRes * 0.32f / camera::camPos.y( );
		glScalef( scale, scale, scale );
		glTranslatef( -culling::viewCenter.x( ), 0.0f, -culling::viewCenter.z( ) );
		glGetFloatv( GL_PROJECTION_MATRIX, lightMVP );
		glMatrixMode( GL_MODELVIEW );

		glClear( GL_DEPTH_BUFFER_BIT );
		for( std::list<rc::Renderable*>::iterator i = rc::Renderable::renderables.begin( ); i != rc::Renderable::renderables.end( ); ++i ) {
			(*i)->render( );
		}

		glMatrixMode( GL_PROJECTION );
		glPopMatrix( );
		glMatrixMode( GL_MODELVIEW );
		glPopMatrix( );

		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
		glPopAttrib( );
		glColorMask( true, true, true, true );
	}
}
