#ifndef _MESH_HPP_INCLUDED_
#define _MESH_HPP_INCLUDED_

#include <algorithm> // port2015: so eigen can use std::min, and std::max
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <GL/GLee.h>
#include "ressource.hpp"

namespace rc {
	struct Vertex {
		Eigen::Vector2f tex;
		Eigen::Vector3f normal;
		Eigen::Vector3f pos;

		Vertex( ) : tex(0.0f, 0.0f), normal( 0.0f, 0.0f, 0.0f), pos(0.0f, 0.0f, 0.0f) { }
	};

	class Mesh : public Ressource {
		private:
			int mIndexCount;
			unsigned short mVertexCount;
			GLuint mVertices, mIndices;
			GLenum mDrawMode;
			Vertex* mVertexData;
			unsigned short* mIndexData;
			std::string mFilename;

			int fillNoExt( const char* filename );
			void loadCachedFileNoUpload( const char* filename );
			void saveRidleyFile( const char* filename );

			static void loadCachedFileNoUpload_CB_Wrapper( void* object );

		public:
			enum drawMode { POINTS = GL_POINTS, LINES = GL_LINES, LINE_STRIP = GL_LINE_STRIP, TRIANGLES = GL_TRIANGLES, QUADS = GL_QUADS };

			static int polysDrawn;

			Mesh( ) : mIndexCount(0), mVertexCount(0), mVertices(0), mIndices(0), mVertexData(0), mIndexData(0) { }
			Mesh( const char* filename ) : mVertices(0), mIndices(0), mVertexData(0), mIndexData(0) { create( ); loadFile( filename ); }
			void create( );
			~Mesh( ) { destroy( ); }
			void destroy( );
			void cleanTempBuffers( );

			void load( const char* filename ) { loadFile( filename ); }
			void reload( ) { destroy( ); load( mFilename.c_str( ) ); }
			void loadFile( const char* filename ); // Looks for file.ridleymesh, if not there, loads file.3ds and creates file.ridleymesh
			void loadRidleyFile( const char* filename );
			void loadRidley( const char* buffer, int size );
			void load3DSFile( const char* filename );
			void load3DS( const char* buffer, int size );

			void upload( bool dynamic = false );
			void upload( unsigned short vertnum, Vertex* vertices, int indnum, const unsigned short* indices, drawMode dm = TRIANGLES, bool dynamic = false );

			void render( );

			int indexCount( ) const { return mIndexCount; }
			unsigned short vertexCount( ) const { return mVertexCount; }
			drawMode type( ) const { return (drawMode)mDrawMode; }
			void type( drawMode drawmode ) { mDrawMode = drawmode; }
	};
}

#endif
