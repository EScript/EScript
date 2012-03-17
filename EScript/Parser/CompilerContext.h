// CompilerContext.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef COMPILER_CONTEXT_H
#define COMPILER_CONTEXT_H

#include "../Utils/StringId.h"
#include "../Instructions/Instruction.h"
#include "../Instructions/InstructionBlock.h"
#include <map>
#include <set>
#include <string>
#include <vector>
#include <sstream>

namespace EScript {

/*! Compiler
	Input: Syntax tree made of Expressions
	Output: Byte Code made of EAssembler-Instructions */

/*! Collection of "things" used during the compilation process.
	As the compilation process is currently under development, it is not clear how this class changes 
	in the near future.	*/
class CompilerContext {
		InstructionBlock & instructions;

		typedef std::map<StringId,size_t> indexNameMapping_t;
		std::vector<indexNameMapping_t> visibleLocalVariableStack;
		int currentLine;
		uint32_t currentMarkerId;
	public:
		CompilerContext(InstructionBlock & _instructions) : 
				instructions(_instructions),currentLine(-1),currentMarkerId(Instruction::JMP_TO_MARKER_OFFSET){}
		
		void addInstruction(const Instruction & newInstruction)	{	instructions.addInstruction(newInstruction,currentLine);	}

		uint32_t createMarker()									{	return currentMarkerId++;	}
		uint32_t declareString(const std::string & str)			{	return instructions.declareString(str);	}

		std::string getInstructionsAsString()const				{	return instructions.toString();	}
		StringId getLocalVarName(const int index)const			{	return instructions.getLocalVarName(index);	}

		size_t getNumLocalVars()const							{	return instructions.getNumLocalVars();	}
		std::string getStringConstant(const uint32_t index)const{	return instructions.getStringConstant(index);	}
		int getVarIndex(const StringId name)const;
		
		bool isCurrentInstructionBlock( const InstructionBlock & i2 )const	{	return &i2==&instructions;	}
	
		//! Add the local variables which are already defined in the instructionBlock (e.g. 'this' or the parameters), to the set of visible variables.
		void initBasicLocalVars();
		
		void pushLocalVars(const std::set<StringId> & variableNames);
		void popLocalVars();
		uint32_t registerInternalFunction(const ObjPtr userFunction)	{	return instructions.registerInternalFunction(userFunction);	}
		void setLine(int l)										{	currentLine=l;	}
	
		/*! (static) \todo // move to Compiler
			- Replaces the markers inside the assembly by jump addresses.	*/
		static void finalizeInstructions( InstructionBlock & instructions ); 
		
//		const InstructionBlock & getInstructions()const			{	return instructions;	}
	
		std::ostringstream out; // temporary

};
}

#endif // COMPILER_CONTEXT_H
