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
//		int markerCounter;
		
		std::vector<StringId> localVariables; 
		
		typedef std::map<StringId,size_t> indexNameMapping_t;
		
		std::vector<indexNameMapping_t> currentLocalVariableStack;
		std::vector<Instruction> instructions;
		std::vector<std::string> stringConstants;
		std::vector<std::string> markerNames;
		int line;
	public:
		typedef uint32_t marker_t;
		CompilerContext() : line(-1){}
		
		marker_t createMarker()	{
			const marker_t m = markerNames.size();
			std::ostringstream o; 
			o << "marker" << m; 
			markerNames.push_back(o.str());
			return m;
		}		
		marker_t createMarker(const std::string & prefix)	{	
			const marker_t m = markerNames.size();
			std::ostringstream o; 
			o << prefix << m; 
			markerNames.push_back(o.str());
			return m;
		}
		std::string getMarkerName(const marker_t m)const{
			std::ostringstream o;
			if(m<=markerNames.size()) o<<markerNames[m];
			else o<<m;
			o<<":";
			return o.str();
		}

		std::ostringstream out; // temporary
		
		void pushLocalVars(const std::set<StringId> & variableNames);
		void popLocalVars();
		size_t getNumLocalVars()const	{	return localVariables.size();	}
		int getVarIndex(const StringId name)const;
		StringId getLocalVarName(const int index)const;
		
		void setLine(int l)				{	line=l;	}
		void addInstruction(const Instruction & newInstruction){	
			instructions.push_back(newInstruction);	
			instructions.back().setLine(line);
		}
		uint32_t declareString(const std::string & str){
			stringConstants.push_back(str);
			return static_cast<uint32_t>(stringConstants.size()-1);
		}
		
		std::string getInstructionsAsString()const;
		std::string getStringConstant(const uint32_t index)const{
			return index<=stringConstants.size() ? stringConstants[index] : "";
		}
};
}

#endif // COMPILER_CONTEXT
