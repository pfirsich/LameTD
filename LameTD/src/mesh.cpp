#include "mesh.hpp"
#include <GL/glfw.h>
#include <string.h>
#include <cctype>
#include <stdio.h>
#include <list>
#include "messagehandler.hpp"

namespace rc {
	int Mesh::polysDrawn;

	int fileToBuf( const char* filename, char** buf ) {
		FILE* f = fopen( filename, "rb" );
		if( f ) {
			fseek( f, 0, SEEK_END );
			int size = ftell( f );
			fseek( f, 0, SEEK_SET );
			*buf = new char[size];
			int read = fread( *buf, 1, size, f );
			fclose( f );

			return read;
		} else {
			return -1;
		}
	}

	void Mesh::cleanTempBuffers( ) {
		if( mVertexData ) delete[] mVertexData; mVertexData = 0;
		if( mIndexData ) delete[] mIndexData; mIndexData = 0;
	}

	void Mesh::create( ) {
		 if( !mVertices ) glGenBuffersARB( 1, &mVertices );
		 if( !mIndices ) glGenBuffersARB( 1, &mIndices );
	}

	void Mesh::destroy( ) {
		if( mVertices ) glDeleteBuffers( 1, &mVertices );
		if( mIndices ) glDeleteBuffers( 1, &mIndices );
		cleanTempBuffers( );
	}

	int Mesh::fillNoExt( const char* filename ) {
		std::string fname = filename;
		int ret = -1; // -1: No loading possible, 0 = 3ds, 1 = ridley-mesh
		int size;
		char* buf = 0;

		size = fileToBuf( (fname + ".ridleymesh").c_str( ), &buf );
		if( size != -1 ) {
			ret = 1;
			loadRidley( buf, size );
		} else {
			size = fileToBuf( (fname + ".3ds").c_str( ), &buf );
			if( size != -1 ) {
				ret = 0;
				load3DS( buf, size );
			} else {
				messageHandler->push( Message( Message::MSG_ERROR, Message::MESH_OPEN_ERROR, std::string( "Neither " ) + filename + ".ridleymesh nor " + filename + ".3ds could be opened." ) );
			};
		}

		if( size != 1 ) delete[] buf;

		return ret;
	}

	void Mesh::loadFile( const char* filename ) {
		int ridley = fillNoExt( filename );
		if( ridley == 0 ) {
			saveRidleyFile( (std::string(filename) + ".ridleymesh").c_str( ) );
		};
		upload( );
		cleanTempBuffers( );
	}

	void Mesh::upload( bool dynamic ) {
		if( !mVertexData || !mIndexData ) return;
		create( );
		glBindBuffer( GL_ARRAY_BUFFER, mVertices );
		glBufferData( GL_ARRAY_BUFFER, mVertexCount * sizeof(Vertex), mVertexData, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndices );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, mIndexCount * sizeof(unsigned short), mIndexData, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );
	}

	void Mesh::saveRidleyFile( const char* filename ) {
		FILE* f = fopen( filename, "wb" );
		if( f ) {
			fwrite( &mVertexCount, 2, 1, f );
			fwrite( &mIndexCount, 4, 1, f );
			fwrite( mVertexData, sizeof(Vertex), mVertexCount, f );
			fwrite( mIndexData, sizeof(unsigned short), mIndexCount, f );
			fclose( f );
		} else {
			fclose( f );
			messageHandler->push(Message(Message::MSG_ERROR, Message::MESH_OPEN_ERROR, std::string(filename) + " could not be opened for writing."));
		};
	}

	void Mesh::loadRidleyFile( const char* filename ) {
		char* buf;
		int size = fileToBuf( filename, &buf );
		if( size != -1 ) {
			loadRidley( buf, size );
			upload( );
			cleanTempBuffers( );
		} else {
			messageHandler->push(Message(Message::MSG_ERROR, Message::MESH_OPEN_ERROR, std::string(filename) + " could not be opened."));
		}
	}

	void Mesh::loadRidley( const char* buffer, int size ) {
		memcpy( &mVertexCount, buffer, 2 );
		memcpy( &mIndexCount, buffer + 2, 4 );
		mVertexData = new Vertex[mVertexCount];
		memcpy( mVertexData, buffer + 6, sizeof(Vertex) * mVertexCount );
		mIndexData = new unsigned short[mIndexCount];
		memcpy( mIndexData, buffer + 6 + sizeof(Vertex) * mVertexCount, sizeof(unsigned short) * mIndexCount );
		mDrawMode = TRIANGLES;
		upload( );
		cleanTempBuffers();
	}

	void Mesh::load3DSFile( const char* filename ) {
		char* buf;
		int size = fileToBuf( filename, &buf );
		if( size != -1 ) {
			load3DS( buf, size );
			upload( );
			cleanTempBuffers( );
		} else {
			messageHandler->push( Message( Message::MSG_ERROR, Message::MESH_OPEN_ERROR, std::string( filename ) + " could not be opened." ) );
		}
	}

	void Mesh::load3DS( const char* buf, int size ) {
		int indoffset = 0;
		mVertexCount = 0;
		mIndexCount = 0;
		int oldvc = 0;
		int indadd = 0;

		// March through chunks
		int i = 0;
		while( i < size ) {
			switch( *(unsigned short*)(buf + i) ) {
				case 0x4D4D: // Main chunk
				case 0x3D3D: // Editor chunk (the object is in here)
				case 0x4100: // Mesh Chunk (in Object-Chunk)
					i += 6;
					break;
				case 0x4000: // An object
					i += strlen( buf + i + 6 ) + 7; // Skip object's name
					break;
				case 0x4110: {// The vertex list
					oldvc = mVertexCount;
					mVertexCount += (int)*(unsigned short*)(buf + i + 6);
					Vertex* temp = new Vertex[mVertexCount];
					if( mVertexData ) {
						memcpy( temp, mVertexData, oldvc * sizeof(Vertex) );
						delete[] mVertexData;
					};
					mVertexData = temp;
					i += 8;
					//float* fbuf = (float*)(buf + i);
					for( int j = oldvc; j < mVertexCount; ++j ) {
						memcpy( (unsigned char*)(mVertexData + j) + 20, buf + i, 12 );
						//mVertexData[j].pos << fbuf[0], fbuf[1], fbuf[2];
						i += 12;
						//fbuf += 3;
					};
				} break;
				case 0x4120: { // Face indices
					int faces = (int)*(unsigned short*)(buf + i + 6);
					indoffset = mIndexCount;
					mIndexCount += faces * 3;
					unsigned short* newinds = new unsigned short[mIndexCount];
					if( mIndexData ) {
						memcpy( newinds, mIndexData, indoffset * sizeof(unsigned short) );
						delete[] mIndexData;
					};
					mIndexData = newinds;
					unsigned short* sbuf = (unsigned short*)(buf + i + 8);
					int k = indoffset;
					for( int j = 0; j < faces * 4; ++j ) {
						if( (j + 1) % 4 != 0 || j == 0 ) mIndexData[k++] = sbuf[j] + indadd;
					};
					i += *(unsigned int*)(buf + i + 2);
					indadd = mVertexCount;
					} break;
				case 0x4140: { // UV-coordinates
					i += 8;
					//float* fbuf = (float*)(buf + i);
					for( int j = oldvc; j < mVertexCount; ++j ) {
						memcpy( mVertexData + j, buf + i, 8 );
						//mVertexData[j].tex << fbuf[0], fbuf[1];
						i += 8;
						//fbuf += 3;
					};
					} break;
				default:
					i += *(unsigned int*)(buf + i + 2);
					break;
			};
		};

		/*int* newVertexMap = new int[mVertexCount];
		int* faceMap = new int[mVertexCount];
		for( int i = 0; i < mVertexCount; ++i ) {
			newVertexMap[i] = i;
			faceMap[i] = i;
		}

		// Merge vertices
		int newCount = mVertexCount;
		for( int i = 0; i < mVertexCount; ++i ) {
			if( newVertexMap[i] = i ) {
				for( int j = i; j < mVertexCount; ++j ) {
					if( (mVertexData[i].pos - mVertexData[j].pos).squaredNorm( ) < 0.1f ) {
						newCount--;
						newVertexMap[j] = i;
						faceMap[j] = i;
					};
				};
			};
		};

		Vertex* temp = mVertexData;
		mVertexData = new Vertex[newCount];
		for( int i = 0; i < mVertexCount ) {

		};

		mVertexCount = newCount;*/

		// Normals
		Eigen::Vector3f* faceNormals = new Eigen::Vector3f[mIndexCount/3];
		int fi = 0;
		for( int i = 0; i < mIndexCount; i += 3 )
			faceNormals[fi++] = (mVertexData[mIndexData[i]].pos - mVertexData[mIndexData[i+2]].pos).cross( mVertexData[mIndexData[i+1]].pos - mVertexData[mIndexData[i+2]].pos );

		for( int i = 0; i < mIndexCount; ++i )
			mVertexData[mIndexData[i]].normal += faceNormals[i/3];

		delete[] faceNormals;

		for( int i = 0; i < mVertexCount; ++i )
			mVertexData[i].normal.normalize( );

		puts( "BLAM" );

		mDrawMode = TRIANGLES;
	}

	void Mesh::upload( unsigned short vertnum, Vertex* vertices, int indnum, const unsigned short* indices, drawMode dm, bool dynamic ) {
		create( );

		glBindBuffer( GL_ARRAY_BUFFER, mVertices );
		glBufferData( GL_ARRAY_BUFFER, vertnum * sizeof(Vertex), vertices, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndices );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, indnum * sizeof(unsigned short), indices, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );

		mVertexCount = vertnum;
		mIndexCount = indnum;
		mDrawMode = dm;
	}

	void Mesh::render( ) {
		glBindBuffer( GL_ARRAY_BUFFER_ARB, mVertices );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, mIndices );

		glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
		glInterleavedArrays( GL_T2F_N3F_V3F, 0, NULL );
		glDrawRangeElements( mDrawMode, 0, mVertexCount - 1, mIndexCount, GL_UNSIGNED_SHORT, NULL );
		glPopClientAttrib( );

		polysDrawn += mIndexCount / 3;

		//glBindBuffer( GL_ARRAY_BUFFER_ARB, 0 ); // TODO: Bench this!
		//glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, 0 ); // TODO: Bench this!

	}
}
