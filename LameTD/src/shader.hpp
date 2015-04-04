#ifndef _SHADER_HPP_INCLUDED_
#define _SHADER_HPP_INCLUDED_

#include <GL/GLee.h>
#include <list>
#include <string>
#include "ressource.hpp"
#include <stdio.h>

namespace rc {
	struct uniform_type {
		std::string name;
		GLint loc;
		unsigned int type;
		const void* data;
	};

	namespace uniform {
		const unsigned int SCALAR = 	0x00010000;
		const unsigned int VEC2 = 		0x00020000;
		const unsigned int VEC3 =		0x00040000;
		const unsigned int VEC4 = 		0x00080000;
		const unsigned int MAT2 = 		0x00100000;
		const unsigned int MAT3 = 		0x00200000;
		const unsigned int MAT4 = 		0x00400000;
		const unsigned int MAT2x3 = 	0x00800000;
		const unsigned int MAT3x2 = 	0x01000000;
		const unsigned int MAT2x4 =		0x02000000;
		const unsigned int MAT4x2 = 	0x04000000;
		const unsigned int MAT3x4 =		0x08000000;
		const unsigned int MAT4x3 =		0x10000000;

		const unsigned int FLOAT =		0x00000F00;
		const unsigned int INTEGER = 	0x0000F000;

		const unsigned int SAMPLER = SCALAR | INTEGER;
		const unsigned int BOOL = SCALAR | INTEGER;
	}

	class UniformSet {
		private:
			GLhandleARB prog;

		public:
			std::list<uniform_type> uniformList;

			UniformSet( ) : prog(0) { }
			UniformSet( GLhandleARB p ) : prog(p) { }
			~UniformSet( ) { };
			void setProgram( GLhandleARB p ) { prog = p; }
			void add( unsigned int type, const char* name, const void* data, bool goon = true );
			void apply( ) const;
	};

	class Shader : public Ressource {
		private:
			GLhandleARB vs, fs, prog;
			int logwhat;
			std::string mFilename;

			static GLhandleARB lastProg;

		public:
			static const bool VERTEX = 0;
			static const bool FRAGMENT = 1;
			UniformSet defaultUniforms;
			static int binds;
			static std::string global;

			Shader( );
			~Shader( ) { destroy( ); }
			void destroy( );
			void source( bool, const char* );
			void sourceFile( bool, const char* );
			void load( const char* fname );
			void reload( ) { printf( "Reload: '%s'\n", mFilename.c_str( ) ); destroy( ); load( mFilename.c_str( ) ); }
			GLhandleARB program( ) const;
			void use( ) {
				if( this == 0 ) {
					if( lastProg ) {
						glUseProgramObjectARB( lastProg = 0 );
						++binds;
					}
					return;
				}

				if( prog != lastProg ) {
					glUseProgramObjectARB( lastProg = prog );
					++binds;
				}
				defaultUniforms.apply( );
			}
			void build( );
			void log( char** );
			//void log( char* );*/
			bool validUniform( const char* );
	};
	void useNoShader( );
};

#endif
