#include <GL/GLee.h>
#include <GL/glfw.h>
#include "texture.h"
#include "mesh.hpp"
#include "config.hpp"
#include "ressource.hpp"
#include <stdio.h>
//#include <enet/enet.h>
#include <string>
#include <time.h>
#include "client.hpp"
#include "messagehandler.hpp"
#include "graphics.hpp"
#include "object.hpp"
#include "camera.hpp"
#include "map.hpp"
#include "picking.hpp"
#include "shadows.hpp"
#include <stdlib.h>

ConfigFile config;
float shaderTime;

void handleMessages( ) {
	while( rc::messageHandler->pending( ) ) {
		rc::Message temp = rc::messageHandler->front( );
		printf( "Message (%d): %d -> %s\n", (int)temp.type( ), temp.ID( ), temp.text( ).c_str( ) );
		rc::messageHandler->pop( );
	};
}

std::string strFloat( float i ) {
	std::stringstream out;
	out << i;
	return out.str( );
}

std::string file( const char* filename ) {
	FILE* f = fopen( filename, "r" );
	if( !f ) {
		printf( "'%s' could not be opened.\n", filename );
		return "";
	}
	fseek( f, 0, SEEK_END );
	int size = ftell( f );
	fseek( f, 0, SEEK_SET );
	char* buf = new char[size];
	int r = fread( buf, 1, size, f );
	buf[r] = '\0';
	fclose( f );
	std::string temp = buf;
	delete[] buf;
	return temp;
};

int main( ) {
	config.load( "config.cfg" );
	if( config.error( ) ) {
		puts( "Error loading config." );
		return 1;
	}

	rc::MessageHandler hMessages;
	rc::registerMessageHandler( &hMessages );

	rc::init( "KakaTD", (int)config.getNum("xres"), (int)config.getNum("yres"), (int)config.getNum("zres"), config.getNum("fullscreen") > 0.01f, config.getNum("fov"), config.getNum("near"), config.getNum("far"), (int)config.getNum("depth"), (int)config.getNum("fsaa"), (int)config.getNum("stencil"), (int)config.getNum("accum") );
	handleMessages( );

	rc::Shader::global = "const vec3 lightDir = vec3( 0.5, 0.0, 0.866 );\n";
	if( (int)config.getNum("pcf") ) rc::Shader::global += "#define PCF\n";
	rc::Shader::global += "#define SHADOWMAPSIZE " + strFloat((float)config.getNum("shadowmapres")) + "\n";
	rc::Shader::global += "#line 0\n";

	culling::setup( config.getNum("fov"), config.getNum("xres") / config.getNum("yres"), 60.0f );
	camera::setup( config.getNum("heightSpeed"), config.getNum("heightMoveSmoothness"), (int)config.getNum("moveBorder"), config.getNum("moveSpeed"), config.getNum("lookAngle"), config.getNum("targetHeight") );
	map::setup( config.getString("map").c_str( ) );
	shadows::setup( (int)config.getNum("shadowmapres"), config.getNum("bilinearpcf") > 0.01f );

	rc::Object pickMarker;
	pickMarker.setMesh( rc::getRessource<rc::Mesh>( "media/cube2" ) );
	pickMarker.setTexture( rc::getRessource<rc::Texture>( "media/spawn.png" ) );
	pickMarker.scale( Eigen::Vector3f( 0.01f, 0.01f, 0.01f ) );
	pickMarker.position( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ) );

	rc::Shader* towerShader = rc::getRessource<rc::Shader>( "media/tower.vert;media/tower.frag" );
	towerShader->defaultUniforms.add( rc::uniform::SAMPLER, "shadowMap", &rc::Texture::sampler1 );
	towerShader->defaultUniforms.add( rc::uniform::MAT4 | rc::uniform::FLOAT, "lightMVP", shadows::lightMVP );
	towerShader->defaultUniforms.add( rc::uniform::MAT4 | rc::uniform::FLOAT, "objectMVP", rc::Object::MVP );

	rc::Renderable::finishInitialization( );
	/*for( std::list<rc::Renderable*>::iterator i = rc::Renderable::renderables.begin( ); i != rc::Renderable::renderables.end( ); ++i) {
		printf( "%d\n", (*i)->hash( ) );
	};*/
	puts( "------------------------------------------------- Start ");

	time_t next = clock( );
	int frameCounter = 0;
	double t0 = 0.0, t1 = 0.0;
	glfwSetTime(0.0);
	Eigen::Vector2i pickPos = Eigen::Vector2i( -1, -1 );
	bool lastEnter = false;
	bool lastClick = false;
	do {
		t0 = glfwGetTime();
		//float speed = ((t0.tv_sec - t1.tv_sec) * 1000000 + (t0.tv_usec - t1.tv_usec)) * 0.0001f;
		//shaderTime = (t0.tv_sec * 1000000 + t0.tv_usec) * 0.000001f;
		float speed = (float)(t0 - t1) * 100.0f;
		shaderTime = (float)t0 * 1.0f;
		t1 = t0;

		bool click = glfwGetMouseButton( GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS;
		if( pickPos.x( ) != -1 ) {
			map::GridField* field = map::mapGrid + pickPos.y( )*map::width + pickPos.x( );
			Eigen::Vector3f pos = Eigen::Vector3f( (pickPos.x( ) - map::width * 0.5f) * SPACING, field->type==map::GridField::RAISED?HEIGHT:0.0f, (pickPos.y( ) - map::height * 0.5f) * SPACING );
			pickMarker.position( pos );
			if( click && !lastClick ) {
				rc::Object* temp = new rc::Object;
				temp->setMesh( rc::getRessource<rc::Mesh>( "media/Blitz" ) );
				temp->setShader( rc::getRessource<rc::Shader>( "media/tower.vert;media/tower.frag" ) );
				temp->setTexture( &shadows::shadowMap, rc::Texture::sampler1 );
				temp->position( pos );
				temp->scale( Eigen::Vector3f( 0.024f, 0.024f, 0.03f ) );
				temp->orientation(Eigen::Vector3f(-(float)M_PI / 2.0f, 0.0f, 0.0f));
			}
		}
		lastClick = click;

		camera::update( speed );
		if( rc::getKey( GLFW_KEY_SPACE ) ) {
			camera::camPos = Eigen::Vector3f( 50.0f, 50.0f, -50.0f );
			camera::targetHeight = 50.0f;
		};

		bool enter = rc::getKey( GLFW_KEY_ENTER );
		if( enter && !lastEnter ) {

		};
		lastEnter = enter;

		//puts( "--------------------------------------- Frame ");
		// Rendering
		culling::update( );
		pickPos = picking::update( );
		shadows::update( );
		rc::update( );
		frameCounter++;
		handleMessages( );

		if( next < clock( ) ) {
			char Buf[256];
			sprintf( Buf, "FPS: %d, Rendered: %d, Texture-Switches: %d, Shader-Binds: %d, Polygons: %.3d", frameCounter, rc::Renderable::rendered, rc::Texture::binds, rc::Shader::binds, rc::Mesh::polysDrawn );
			puts( Buf );
			glfwSetWindowTitle( Buf );
			frameCounter = 0;
			next = clock( ) + CLOCKS_PER_SEC;
		};
	} while( rc::windowOpen( ) && !rc::getKey( GLFW_KEY_ESC ) );

	rc::terminate( );
	return 0;
}
