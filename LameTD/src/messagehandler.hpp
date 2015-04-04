// Joel Schumacher, 12.01.2010

#ifndef MESSAGEHANDLER_HPP_
#define MESSAGEHANDLER_HPP_

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <string>
#include <list>
#include <queue>

/* Just so I don't have to look it up all the time:
 *
#define GL_NO_ERROR                       0
#define GL_INVALID_ENUM                   0x0500
#define GL_INVALID_VALUE                  0x0501
#define GL_INVALID_OPERATION              0x0502
#define GL_STACK_OVERFLOW                 0x0503
#define GL_STACK_UNDERFLOW                0x0504
#define GL_OUT_OF_MEMORY                  0x0505
#define GL_TABLE_TOO_LARGE                0x8031
 *
 */

namespace rc { // Recooze
	class Message {
		public:
			enum MESSAGE_TYPE {
				// port2015: apparently no enum value can be named "ERROR" or msvc will complain
				MSG_CRITICAL, // Engine state: not stable - termination advised to avoid hell breaking loose (undefined behaviour)
				MSG_ERROR, // Something went wrong, you'll notice it, BUT IT WON'T STOP ME!
				MSG_INFORMATION // Something you should know
			};

			enum MESSAGE_ID {
				UNKNOWN,
				LOGFILE_OPEN_ERROR, // Log file could not be opened
				NULLHANDLER_REGISTERED, // The current message handler is a "null handler", which just ignores everything
				GLFW_ERROR, // GLFW could not be initialized
				UNKNOWN_COLOR_DEPTH, // Couldn't split color depth into channels depths
				WINDOW_OPEN_ERROR, // Window could not be opened
				OPENGL_ERROR, // An OpenGL error occured
				MESH_OPEN_ERROR, // Mesh file could not opened
				TEXTURE_LOAD_ERROR, // Disk -> Ram failed
				TEXTURE_UPLOAD_ERROR, // Ram -> OpenGL-Texture failed
				SHADER_LINK_ERROR, // Shader could not be linked
				FRAGMENT_SHADER_COMPILE_ERROR, // Fragment shader could not be compiled
				VERTEX_SHADER_COMPILE_ERROR, // Vertex shader could not be compiled
				UNIFORM_TYPE_NOT_SUPPORTED, // Uniform type is not supported
				FRAMEBUFFER_INCOMPLETE, // An error occured during the creation of an FBO
				USER_DEFINED_MESSAGE, // Just an offset -> Message( ..., Message::USER_DEFINED_MESSAGE + MY_MESSAGE, ... );
			};

		private:
			MESSAGE_TYPE mType;
			int mID;
			std::string mText;

		public:
			inline Message( MESSAGE_TYPE type, int id, const std::string& text ) : mType( type ), mID( id ), mText( text ) { }
			inline MESSAGE_TYPE type( ) const { return mType; }
			inline int ID( ) const { return mID; }
			inline std::string text( ) const { return mText; }
	};

	class AbstractMessageHandler {
		public:
			virtual void push( const Message& m ) = 0;
			virtual int pending( ) const = 0;
			virtual const Message& front( ) const = 0;
			virtual void pop( ) = 0;
	};

	class MessageHandler : public AbstractMessageHandler {
		private:
			std::queue<Message> mMessages;
			char* mFilename;

		public:
			MessageHandler( const char* logfile = 0 ) {
				/*if( logfile ) {
					mFilename = new char[strlen(logfile)+1];
					strcpy( mFilename, logfile );
				} else {
					time_t rawtime;
					time( &rawtime );
					tm* timeinfo;
					timeinfo = localtime( &rawtime );

					mFilename = new char[23];
					strftime( mFilename, 23, "logs/log_%d.%m.%y_%H.%M.%S", timeinfo );
				};*/
			}

			~MessageHandler( ) { delete[] mFilename; }

			void push( const Message& m ) {
				mMessages.push( m );

				/*if( m.ID( ) != Message::LOGFILE_OPEN_ERROR ) {
					FILE* f = fopen( mFilename, "a" );
					if( !f ) {
						push( Message( Message::MSG_ERROR, Message::LOGFILE_OPEN_ERROR, "Logfile could not be opened." ) );
					} else {
						fputs( m.text( ).c_str( ), f );
						fputs( "\n", f );
						fclose( f );
					};
				};*/

				//if( m.type( ) == Message::MSG_CRITICAL ) throw m;
			}

			inline int pending( ) const { return mMessages.size( ); }

			inline const Message& front( ) const {
				return mMessages.front( );
			}

			inline void pop( ) { mMessages.pop( ); }
	};

	class NullHandler : public AbstractMessageHandler {
		public:
			inline void push( const Message& m ) { }
			inline int pending( ) const { return 0; }
			inline const Message& front( ) const { return *((Message*)0); }
			inline void pop( ) { }
	};

	extern NullHandler nullHandler;
	extern AbstractMessageHandler* messageHandler;

	inline void registerMessageHandler( AbstractMessageHandler* hMsg ) {
		messageHandler = hMsg;
	}
}

#endif /* MESSAGEHANDLER_HPP_ */
