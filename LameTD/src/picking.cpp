#include "picking.hpp"
#include <GL/GLee.h>
#include <GL/glfw.h>
#include "map.hpp"

namespace picking {
	Eigen::Vector2i update( ) {
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		rc::AbstractTexture* tempT = map::ground->getTexture( );
		map::ground->setTexture( map::colorMap );
		rc::Shader* tempS = map::ground->getShader( );
		map::ground->setShader( 0 );
		map::ground->render( );
		map::ground->setTexture( tempT );
		map::ground->setShader( tempS );

		int mx, my;
		glfwGetMousePos( &mx, &my );
		unsigned char color[3];
		glReadPixels( mx, rc::YRes - my, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &color );

		map::GridField* field = map::mapGrid;
		for( int y = 0; y < map::height; ++y ) {
			for( int x = 0; x < map::width; ++x ) {
				if( field->type == map::GridField::NORMAL || field->type == map::GridField::RAISED ) {
					if( field->r == color[0] && field->g == color[1] && field->b == color[2] )
						return Eigen::Vector2i( x, y );
				}
				field++;
			};
		};

		return Eigen::Vector2i( -1, -1 );
	}
}
