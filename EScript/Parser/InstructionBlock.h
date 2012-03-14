// InstructionBlock.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef INSTRUCTION_BLOCK_H
#define INSTRUCTION_BLOCK_H

#include "../Utils/StringId.h"
#include "../Objects/Internals/Instruction.h"

#include <string>
#include <vector>

namespace EScript {

/*! Collection of (assembler-)instructions and the corresponding data.	*/
class InstructionBlock {
		
		std::vector<StringId> localVariables; 
		
		std::vector<std::string> stringConstants;
		std::vector<std::string> markerNames;

		std::vector<Instruction> instructions;

	public:
		typedef uint32_t marker_t;
		static const marker_t NAMED_MARKER_OFFSET = 0x10000;

		InstructionBlock() {}
		
		void addInstruction(const Instruction & newInstruction)	{	instructions.push_back(newInstruction);	}
		void addInstruction(const Instruction & newInstruction,int line)	{	
			instructions.push_back(newInstruction);	
			instructions.back().setLine(line);
		}

		void clearMarkerNames()								{	markerNames.clear();	}
		marker_t createMarker();
		marker_t createMarker(const std::string & prefix);
		
		uint32_t declareString(const std::string & str){
			stringConstants.push_back(str);
			return static_cast<uint32_t>(stringConstants.size()-1);
		}
		uint32_t declareLocalVarible(const StringId name){
			localVariables.push_back(name);
			return static_cast<uint32_t>(localVariables.size()-1);
		}
		std::string toString()const;
		StringId getLocalVarName(const int index)const;

		std::string getMarkerName(const marker_t m)const;

		size_t getNumLocalVars()const							{	return localVariables.size();	}
		std::string getStringConstant(const uint32_t index)const{	return index<=stringConstants.size() ? stringConstants[index] : "";	}
		int getVarIndex(const StringId name)const;
		
		bool hasJumpMarkers()const								{	return !markerNames.empty();	}
		
		std::vector<Instruction> & _accessInstructions()		{	return instructions;	}
		const std::vector<Instruction> & _accessInstructions()const	{	return instructions;	}

		
};
}

#endif // INSTRUCTION_BLOCK_H
