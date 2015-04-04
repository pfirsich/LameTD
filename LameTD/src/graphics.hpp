#ifndef _GRAPHICS_HPP_
#define _GRAPHICS_HPP_

#include <GL/GLee.h>
#include <GL/glfw.h>
#include <map>
#include <string>
#include "ressource.hpp"
#include "texture.h"

namespace rc {
	extern int XRes, YRes;
	extern std::map<std::string, Ressource*> ressourceMap;
	extern SimpleTexture shadowMap;
	extern float lightMVP[16];

	void init( const char* name, int xres, int yres, int zres, bool fullscreen, float fov, float near, float far, int depth, int fsaasamples, int stencil, int accum );
	void update( );
	void enqueueGLErrors( );

	inline void terminate( ) { glfwTerminate( ); }
	inline bool getKey( int key ) { return glfwGetKey( key ) == GLFW_PRESS; }
	inline bool windowOpen( ) { return glfwGetWindowParam( GLFW_OPENED ) == GL_TRUE; }
	inline void vSync( bool vs ) { glfwSwapInterval( vs ); }
	inline void getMouse( int& x, int& y ) { glfwGetMousePos( &x, &y ); }
	inline void setMouse( int x, int y ) { glfwSetMousePos( x, y ); }
	//inline void setClearColor( const Color& col ) { glClearColor( col.r( ), col.g( ), col.b( ), 1.0f ); }

	template<class T> static T* getRessource( const char* filename ) {
		std::map<std::string, Ressource*>::iterator i = ressourceMap.find( filename );
		if( i == ressourceMap.end( ) ) {
			T* temp = new T;
			ressourceMap[filename] = (Ressource*)temp;
			temp->load( filename );
			return temp;
		} else {
			return (T*)i->second;
		};
	}
}

#endif
