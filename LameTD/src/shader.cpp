#include <stdio.h>
#include <GL/GLee.h>
#include "shader.hpp"
#include <string.h>

namespace rc {
	int Shader::binds;
	GLhandleARB Shader::lastProg = 0;
	std::string Shader::global;

	Shader::Shader( ) : vs(0), fs(0) { }

	void Shader::destroy( ) {
		if( vs ) glDeleteObjectARB( vs );
		if( fs ) glDeleteObjectARB( fs );
		glDeleteObjectARB( prog );
	}

	void Shader::load( const char* fname ) {
		mFilename = fname;
		char* f = new char[strlen(fname)+1];
		strcpy( f, fname );
		char* sc = (char*)strchr( f, ';' );
		if( !sc ) {
			puts( "No semicolon found. Can't load shader." );
			return;
		}
		if( strlen( f ) + f - sc ) {
			sourceFile( FRAGMENT, sc + 1 );
		};
		*sc = '\0';
		if( sc - f ) {
			sourceFile( VERTEX, f );
		};

		build( );
		char* l = 0;
		log( &l );
		if( l ) puts( l );
	}

	void Shader::source( bool type, const char* buf ) {
		if( type ) {
			if( !fs ) fs = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );
			glShaderSourceARB( fs, 1, (const char**)&buf, NULL );
		} else {
			if( !vs ) vs = glCreateShaderObjectARB( GL_VERTEX_SHADER_ARB );
			glShaderSourceARB( vs, 1, (const char**)&buf, NULL );
		};
	}

	void Shader::sourceFile( bool type, const char* file ) {
		//printf( "file: %s\n", file );
		FILE* f = fopen( file, "r" );
		if( !f ) {
			printf( "'%s' could not be opened in Shader::sourceFile( ).\n", file );
			return;
		}
		fseek( f, 0, SEEK_END );
		int size = ftell( f );
		fseek( f, 0, SEEK_SET );
		char* buf = new char[size+1+global.size()];
		memcpy( buf, global.c_str( ), global.size( ) );
		int r = fread( buf + global.size(), 1, size, f );
		buf[r+global.size( )] = '\0';
		fclose( f );

		//printf( "'%s'\n", buf );

		if( type ) {
			if( !fs ) fs = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );
			glShaderSourceARB( fs, 1, (const char**)&buf, NULL );
		} else {
			if( !vs ) vs = glCreateShaderObjectARB( GL_VERTEX_SHADER_ARB );
			glShaderSourceARB( vs, 1, (const char**)&buf, NULL );
		};

		delete[] buf;
	}

	void Shader::build( ) {
		//puts( "Build" );
		prog = glCreateProgramObjectARB( );
		defaultUniforms.setProgram( prog );
		int status;
		if( vs ) {
			glCompileShaderARB( vs );
			glGetObjectParameterivARB( vs, GL_OBJECT_COMPILE_STATUS_ARB, &status );
			if( !status ) {
				logwhat = 1;
				puts( "Vertex Shader could not be compiled." );
				return;
			};
			glAttachObjectARB( prog, vs );
		};
		if( fs ) {
			glCompileShaderARB( fs );
			glGetObjectParameterivARB( fs, GL_OBJECT_COMPILE_STATUS_ARB, &status );
			if( !status ) {
				logwhat = 2;
				puts( "Fragment Shader could not be compiled." );
				return;
			};
			glAttachObjectARB( prog, fs );
		};
		glLinkProgramARB( prog );
		glGetObjectParameterivARB( prog, GL_OBJECT_LINK_STATUS_ARB, &status );
		if( !status ) {
			logwhat = 3;
			puts( "Shader program could not be linked." );
			return;
		};
	}

	void Shader::log( char** buf ) {
		GLhandleARB obj = 0;
		switch( logwhat ) {
			case 1:
				obj = vs;
				break;
			case 2:
				obj = fs;
				break;
			case 3:
				obj = prog;
				break;
		};
		if( obj ) {
			int length;
			glGetObjectParameterivARB( obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length );
			*buf = new char[length+1];
			glGetInfoLogARB( obj, length+1, &length, *buf );
		};
	}

	/*void Shader::log( char* file ) {
		char* str;
		log( &str );
		FILE* f = fopen( file, "w" );
		if( !f ) throw exception( "File could not be opened in Shader::log( ).", exception::FILE_ERROR, this );
		fwrite( str, 1, strlen( str ), f );
		fclose( f );
	}*/

	GLhandleARB Shader::program( ) const {
		return prog;
	};

	bool Shader::validUniform( const char* n ) {
		return glGetUniformLocation( prog, n ) > -1;
	}

	void UniformSet::add( unsigned int t, const char* n, const void* d, bool goon ) {
		bool save;
		if( goon ) save = glGetUniformLocation( prog, n ) > -1; else save = true;
		if( save ) {
			uniform_type temp;
			temp.name = n;
			temp.loc = glGetUniformLocationARB( prog, n );
			if( temp.loc == -1 ) {
				puts( "Uniform doesn't correspond to an active uniform in program object or begins with 'gl_'." );
				return;
			}
			if( (t & 0xFF) == 0 ) t |= 1;
			temp.type = t;
			temp.data = d;
			uniformList.push_back( temp );
		};
	}

	void UniformSet::apply( ) const {
		for( std::list<uniform_type>::const_iterator i = uniformList.begin( ); i != uniformList.end( ); ++i ) {
			if( i->type & uniform::SCALAR ) {
				if( i->type & uniform::INTEGER ) glUniform1iv( i->loc, i->type & 0xFF, (GLint*)i->data );
				if( i->type & uniform::FLOAT ) glUniform1fv( i->loc, i->type & 0xFF, (GLfloat*)i->data );
			};
			if( i->type & uniform::VEC2 ) {
				if( i->type & uniform::INTEGER ) glUniform2iv( i->loc, i->type & 0xFF, (GLint*)i->data );
				if( i->type & uniform::FLOAT ) glUniform2fv( i->loc, i->type & 0xFF, (GLfloat*)i->data );
			};
			if( i->type & uniform::VEC3 ) {
				if( i->type & uniform::INTEGER ) glUniform3iv( i->loc, i->type & 0xFF, (GLint*)i->data );
				if( i->type & uniform::FLOAT ) glUniform3fv( i->loc, i->type & 0xFF, (GLfloat*)i->data );
			};
			if( i->type & uniform::MAT4 ) {
				if( i->type & uniform::FLOAT ) glUniformMatrix4fv( i->loc, i->type & 0xFF, false, (GLfloat*)i->data );
			};
		};
	}

	void useNoShader( ) {
		glUseProgramObjectARB( 0 );
	}
};
