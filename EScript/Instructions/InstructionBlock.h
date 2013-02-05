// InstructionBlock.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef INSTRUCTION_BLOCK_H
#define INSTRUCTION_BLOCK_H

#include "Instruction.h"
#include "../Utils/StringId.h"
#include "../Objects/Object.h"

#include <string>
#include <vector>

namespace EScript {
class UserFunction;

//! Collection of (assembler-)instructions and the corresponding data.
class InstructionBlock {
		std::vector<StringId> localVariables;
		std::vector<std::string> stringConstants;  //! \todo --> StringData
		std::vector<Instruction> instructions;
		std::vector<ObjRef > internalFunctions; //! UserFunction
		// flags...
	public:
		InstructionBlock();

		void addInstruction(const Instruction & newInstruction)	{	instructions.push_back(newInstruction);	}
		void addInstruction(const Instruction & newInstruction,int line)	{
			instructions.push_back(newInstruction);
			instructions.back().setLine(line);
		}
		uint32_t registerInternalFunction(const ObjPtr & userFunction)	{
			internalFunctions.push_back(userFunction);
			return static_cast<uint32_t>(internalFunctions.size()-1);
		}

		uint32_t declareString(const std::string & str){
			stringConstants.push_back(str);
			return static_cast<uint32_t>(stringConstants.size()-1);
		}
		uint32_t declareLocalVariable(const StringId & name){
			localVariables.push_back(name);
			return static_cast<uint32_t>(localVariables.size()-1);
		}
		const Instruction & getInstruction(const size_t index)const	{	return instructions[index];	}
		int getLine(const size_t index)const						{	return index<instructions.size() ? instructions[index].getLine() : -1;	}


		StringId getLocalVarName(const size_t index)const;
		const std::vector<StringId> & getLocalVariables()const		{	return localVariables;	}

		size_t getNumLocalVars()const								{	return localVariables.size();	}
		size_t getNumInstructions()const							{	return instructions.size();	}
		std::string getStringConstant(const uint32_t index)const	{	return index<=stringConstants.size() ? stringConstants[index] : "";	}
		UserFunction * getUserFunction(const uint32_t index)const;

		std::vector<Instruction> & _accessInstructions()			{	return instructions;	}
		const std::vector<Instruction> & getInstructions()const		{	return instructions;	}

		std::string toString()const;
};
}

#endif // INSTRUCTION_BLOCK_H
