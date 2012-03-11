// CompilerContext.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef COMPILER_CONTEXT
#define COMPILER_CONTEXT

#include <string>
#include <sstream>

namespace EScript {

/*! Collection of "things" used during the compilation process.
	As the compilation process is currently under development, it is not clear how this class changes 
	in the near future.	*/
class CompilerContext {
		int markerCounter;
	public:
		typedef std::string markerId_t;
		CompilerContext() : markerCounter(0){}
		
		int createNewMarkerNr()	{	return ++markerCounter;	}
		
		markerId_t createMarkerId()	{	
			std::ostringstream o; 
			o<<"marker"<<createNewMarkerNr(); 
			return o.str();	
		}		
		markerId_t createMarkerId(const std::string & prefix)	{	
			std::ostringstream o; 
			o<<prefix<<createNewMarkerNr(); 
			return o.str();	
		}
		
		std::ostringstream out; // temporary
};
}

#endif // COMPILER_CONTEXT
