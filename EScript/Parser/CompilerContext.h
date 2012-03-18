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
#include <stack>

namespace EScript {

/*! Collection of "things" used during the compilation process.
	As the compilation process is currently under development, it is not clear how this class changes 
	in the near future.	*/
class CompilerContext {
		InstructionBlock & instructions;

		typedef std::map<StringId,size_t> nameToIndexMapping_t;
	public:
		enum setting_t{
			VISIBLE_LOCAL_VARIABLES, //!< the local variables declared in a BlockStatement
			BREAK_MARKER,
			CONTINUE_MARKER,
			EXCEPTION_MARKER //!< the marker of the next variables declared in a BlockStatement
		};
	
	private:
		struct SettingsStackEntry{
			setting_t type;
			uint32_t marker;
			nameToIndexMapping_t localVariables;
			
			SettingsStackEntry(setting_t _type=VISIBLE_LOCAL_VARIABLES) : type(_type),marker(Instruction::INVALID_JUMP_ADDRESS){}
			SettingsStackEntry(setting_t _type,uint32_t _marker) : type(_type),marker(_marker){}
			
		};

		std::vector<SettingsStackEntry> settingsStack;

		//! Local variable collections needed for determining which variables have been declared inside a 'try'-block
		std::stack<std::vector<size_t> *> variableCollectorStack;

		int currentLine;
		uint32_t currentMarkerId;
		

	public:
		CompilerContext(InstructionBlock & _instructions) : 
				instructions(_instructions),currentLine(-1),currentMarkerId(Instruction::JMP_TO_MARKER_OFFSET){}
		
		void addInstruction(const Instruction & newInstruction)			{	instructions.addInstruction(newInstruction,currentLine);	}

		
		/*! Collect all variable indices on the settings stack until an entry with the given type is found.
			Iff no entry of the given type is found on the stack, false is returned.	*/
		bool collectLocalVariables(setting_t entryType,std::vector<size_t> & variableIndices);
		
		
		uint32_t createMarker()											{	return currentMarkerId++;	}
		uint32_t declareString(const std::string & str)					{	return instructions.declareString(str);	}

		//! if the setting is not defined, Instruction::INVALID_JUMP_ADDRESS is returned.
		uint32_t getCurrentMarker(setting_t markerType)const;
		
		int getCurrentVarIndex(const StringId name)const;

		std::string getInstructionsAsString()const						{	return instructions.toString();	}
		StringId getLocalVarName(const int index)const					{	return instructions.getLocalVarName(index);	}

		size_t getNumLocalVars()const									{	return instructions.getNumLocalVars();	}
		std::string getStringConstant(const uint32_t index)const		{	return instructions.getStringConstant(index);	}

		
		bool isCurrentInstructionBlock( const InstructionBlock & i2 )const	{	return &i2==&instructions;	}
	
		//! Add the local variables which are already defined in the instructionBlock (e.g. 'this' or the parameters), to the set of visible variables.
		void pushSetting_basicLocalVars();

		void pushSetting_marker(setting_t type, const uint32_t marker)	{	settingsStack.push_back(SettingsStackEntry(type,marker));	}
		
		void pushSetting_localVars(const std::set<StringId> & variableNames);
		void popSetting()												{	settingsStack.pop_back();	}

		uint32_t registerInternalFunction(const ObjPtr userFunction)	{	return instructions.registerInternalFunction(userFunction);	}
		void setLine(int l)												{	currentLine=l;	}
		
		//! All newly defined variables are also added to the topmost collection.
		void pushLocalVarsCollector(std::vector<size_t> * collection)	{	variableCollectorStack.push(collection);	}
		void popLocalVarsCollector()									{	variableCollectorStack.pop();	}
	
		/*! (static) \todo // move to Compiler
			- Replaces the markers inside the assembly by jump addresses.	*/
		static void finalizeInstructions( InstructionBlock & instructions ); 
		
};
}

#endif // COMPILER_CONTEXT_H
