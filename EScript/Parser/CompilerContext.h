// CompilerContext.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef COMPILER_CONTEXT
#define COMPILER_CONTEXT

#include "../Utils/StringId.h"
#include "../Objects/Internals/Instruction.h"
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <vector>

namespace EScript {

/*! Collection of "things" used during the compilation process.
	As the compilation process is currently under development, it is not clear how this class changes 
	in the near future.	*/
class CompilerContext {
		int markerCounter;
		
		std::vector<StringId> localVariables; 
		
		typedef std::map<StringId,size_t> indexNameMapping_t;
		
		std::vector<indexNameMapping_t> currentLocalVariableStack;
		std::vector<Instruction> instructions;
		int line;
	public:
		typedef std::string markerId_t;
		CompilerContext() : markerCounter(0),line(-1){}
		
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
		
		void pushLocalVars(const std::set<StringId> & variableNames);
		void popLocalVars();
		size_t getNumLocalVars()const	{	return localVariables.size();	}
		int getVarIndex(const StringId name)const;
		StringId getVar(const int index)const;
		
		void setLine(int l)				{	line=l;	}
		void addInstruction(const Instruction & newInstruction){	
			instructions.push_back(newInstruction);	
			instructions.back().setLine(line);
		}
};
}

#endif // COMPILER_CONTEXT
