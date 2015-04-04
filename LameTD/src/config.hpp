#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_

#include <map>
#include <string>

class ConfigFile {
	private:
		std::map<std::string,float> mNums;
		std::map<std::string,std::string> mStrs;
		bool mError;

	public:
		ConfigFile( ) : mError( false ) { }
		ConfigFile( const std::string& filename ) { load( filename ); }
		~ConfigFile( ) { }
		void load( const std::string& filename );
		void clear( ) { mNums.clear( ); mStrs.clear( ); }
		float getNum( const std::string& id ) { return mNums[id]; }
		std::string getString( const std::string& id ) { return mStrs[id]; }
		bool error( ) { return mError; }
};

#endif
