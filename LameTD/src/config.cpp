#include "config.hpp"
#include <fstream>
#include <stdlib.h>

void ConfigFile::load( const std::string& filename ) {
	clear( );
	mError = false;
	std::ifstream file( filename.c_str( ), std::ios_base::in );
	if( file ) {
		std::string line;
		while( getline( file, line ) ) {
			if( line[0] != '#' ) {
				size_t split = line.find_first_of( '=' );
				if( split != std::string::npos ) {
					std::string val = line.substr(split+1);
					if( val[0] == '"' ) {
						mStrs[line.substr(0,split)] = val.substr(1,val.length()-2);
					} else {
						mNums[line.substr(0, split)] = (float)atof(val.c_str());
					};
				};
			};
		};
	} else {
		mError = true;
	};
}
