#include "graphics.hpp"
#include "messagehandler.hpp"
#include <sstream>
#include "renderable.hpp"
#include "shader.hpp"
#include "texture.h"
#include "mesh.hpp"
#include "camera.hpp"
#include "culling.hpp"
#include "object.hpp"

namespace rc {
	int XRes, YRes;
	std::map<std::string, Ressource*> ressourceMap;

	void enqueueGLErrors( ) {
		static int lastError = GL_NO_ERROR;

		int err = glGetError( );
		if( err != lastError ) {
			std::stringstream ss;
			ss << "An OpenGL-Error occured: 0x" << std::hex << err;
			messageHandler->push( Message( Message::MSG_ERROR, Message::OPENGL_ERROR, ss.str( ) ) );
			printf( "OGL-Error: 0x%02X\n", err );
		};
	}

	void update( ) {
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		//glGetDoublev( GL_MODELVIEW_MATRIX, preRenderMVP );

		Renderable::rendered = 0;
		Texture::binds = 0;
		Shader::binds = 0;
		Mesh::polysDrawn = 0;
		for( std::list<Renderable*>::iterator i = Renderable::renderables.begin( ); i != Renderable::renderables.end( ); ++i ) {
			(*i)->render( );
		}

		/*getRessource<Shader>(";media/depth.frag")->use( );
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, shadowMapTexture );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_NONE );
		glDisable( GL_DEPTH_TEST );
		glLoadIdentity( );
		glMatrixMode( GL_PROJECTION );
		glPushMatrix( );
		glLoadIdentity( );
		gluOrtho2D( 0.0f, XRes, 0.0f, YRes );
		glScalef( 512.0f, 512.0f, 0.0f );
		//glTranslatef( 256.0f, 256.0f, 0.0f );
		glBegin( GL_TRIANGLE_STRIP );
			glTexCoord2f( 0, 1 ); glVertex2f( 0, 1.0f );
			glTexCoord2f( 0, 0 ); glVertex2f( 0, 0 );
			glTexCoord2f( 1, 1 ); glVertex2f( 1.0f, 1.0f );
			glTexCoord2f( 1, 0 ); glVertex2f( 1.0f, 0 );
		glEnd( );
		glPopMatrix( );
		glMatrixMode( GL_MODELVIEW );
		glEnable( GL_DEPTH_TEST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB );
		glDisable( GL_TEXTURE_2D );*/

		enqueueGLErrors( );
		glfwSwapBuffers( );
	}

	void getComponents( int depth, int* r, int* g, int* b, int* a ) {
		*r = *g = *b = *a = 8;
		switch( depth ) {
			case 0: *r = *g = *b = *a = 0;
			case 16: *r = *b = 5; *g = 6; *a = 0; break;
			case 24: *a = 0; break;
			case 32: break;
			default:
				messageHandler->push( Message( Message::MSG_CRITICAL, Message::UNKNOWN_COLOR_DEPTH, "Unknown color/accum depth. Use 0, 16, 24 or 32 instead." ) );
		};
	}

	void init( const char* name, int xres, int yres, int zres, bool fullscreen, float fov, float _near, float _far, int depth, int fsaasamples, int stencil, int accum ) {
		XRes = xres;
		YRes = yres;

		/*std::stringstream ss;
		ss << "Opening window (name = " << name << ", xres = " << xres << ", yres = " << yres << ", zres = " << zres << ", fullscreen = " << fullscreen << ", fov = " << fov << ", near = " << near << ", far = " << far << ", depth = " << depth << ", fsaasamples = " << fsaasamples << ", stencil = " << stencil << ", accum = " << accum << ").";
		messageHandler->push( Message( Message::MSG_INFORMATION, Message::UNKNOWN, ss.str( ) ) );*/
		if( !glfwInit( ) )
			messageHandler->push( Message( Message::MSG_CRITICAL, Message::GLFW_ERROR, "GLFW could not be initialized." ) );

		glfwOpenWindowHint( GLFW_WINDOW_NO_RESIZE, true );
		glfwOpenWindowHint( GLFW_FSAA_SAMPLES, fsaasamples );
		int r, g, b, a;
		getComponents( accum, &r, &g, &b, &a );
		glfwOpenWindowHint( GLFW_ACCUM_RED_BITS, r );
		glfwOpenWindowHint( GLFW_ACCUM_GREEN_BITS, g );
		glfwOpenWindowHint( GLFW_ACCUM_BLUE_BITS, b );
		glfwOpenWindowHint( GLFW_ACCUM_ALPHA_BITS, a );
		getComponents( zres, &r, &g, &b, &a );
		if( !glfwOpenWindow( xres, yres, r, g, b, a, depth, stencil, GLFW_WINDOW + fullscreen ) )
			messageHandler->push( Message( Message::MSG_CRITICAL, Message::WINDOW_OPEN_ERROR, "Window could not be opened." ) );

		glfwSetWindowTitle( name );

		glViewport( 0, 0, xres, yres );
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity( );

		gluPerspective( fov, (GLfloat)xres/yres, _near, _far );
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity( );

		glEnable( GL_DEPTH_TEST );
		glEnable( GL_CULL_FACE );
		//glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

		glAlphaFunc( GL_NOTEQUAL, 0.0f );
		glEnable( GL_ALPHA_TEST );

		Object::MVP = Object::Transform.data( );

		//CPUCount = glfwGetNumberOfProcessors( );
	}
}
