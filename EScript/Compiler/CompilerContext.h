// FunCompileContext.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef COMPILER_CONTEXT_H
#define COMPILER_CONTEXT_H

#include "../Utils/CodeFragment.h"
#include "../Utils/StringId.h"
#include "../Instructions/Instruction.h"
#include "../Instructions/InstructionBlock.h"
#include "VariableTypes.h"
#include <set>
#include <string>
#include <vector>
#include <sstream>
#include <stack>

namespace EScript {
class StaticData;
namespace AST {
class ASTNode;
}
class Compiler;

/*! Collection of "things" used during the compilation process of one user function function
	(or block of code without surrounding function).*/
class FunCompileContext {
		Compiler & compiler;
		StaticData & staticData;
		InstructionBlock & instructions;

		typedef std::unordered_map<StringId,varLocation_t> varLocationMap_t;
	public:
		enum setting_t{
			VISIBLE_LOCAL_AND_STATIC_VARIABLES, //!< the local variables declared in a Block
			BREAK_MARKER,
			CONTINUE_MARKER,
			EXCEPTION_MARKER //!< the marker of the next variables declared in a Block
		};

	private:
		struct SettingsStackEntry{
			setting_t type;
			uint32_t marker;
			varLocationMap_t declaredVariables;

			SettingsStackEntry(setting_t _type = VISIBLE_LOCAL_AND_STATIC_VARIABLES) : type(_type),marker(Instruction::INVALID_JUMP_ADDRESS){}
			SettingsStackEntry(setting_t _type,uint32_t _marker) : type(_type),marker(_marker){}

		};

		std::vector<SettingsStackEntry> settingsStack;

		//! Local variable collections needed for determining which variables have been declared inside a 'try'-block
		std::stack<std::vector<size_t> *> variableCollectorStack;

		int currentLine;
		uint32_t currentMarkerId;
		uint32_t currentOnceMarkerCounter; // used for @(once) [statement]

		CodeFragment code;
		FunCompileContext* parent; // used for detecting the visibility of static variables
		bool usesStaticVars; // if true, the function has to reference the static data container
	public:
		FunCompileContext(Compiler & _compiler,StaticData&sData, InstructionBlock & _instructions,const CodeFragment & _code) :
				compiler(_compiler),staticData(sData),instructions(_instructions),currentLine(-1),currentMarkerId(Instruction::JMP_TO_MARKER_OFFSET),
				currentOnceMarkerCounter(0),code(_code),parent(nullptr),usesStaticVars(false){}

		// create a context for a function embedded another function
		FunCompileContext(FunCompileContext& parentCtxt,InstructionBlock & _instructions,const CodeFragment & _code) :
				compiler(parentCtxt.compiler),staticData(parentCtxt.staticData),
				instructions(_instructions),currentLine(-1),currentMarkerId(Instruction::JMP_TO_MARKER_OFFSET),
				currentOnceMarkerCounter(0),code(_code),parent(&parentCtxt),usesStaticVars(false){}

		void addInstruction(const Instruction & newInstruction)			{	instructions.addInstruction(newInstruction,currentLine);	}


		/*! Collect all variable indices on the settings stack until an entry with the given type is found.
			Iff no entry of the given type is found on the stack, false is returned.	*/
		bool collectLocalVariables(setting_t entryType,std::vector<size_t> & variableIndices);

		void addExpression(EPtr<AST::ASTNode> expression);
		void addStatement(EPtr<AST::ASTNode> stmt);

		uint32_t createMarker()											{	return currentMarkerId++;	}
		StringId createOnceStatementMarker(); // used for @(once) [statement]
		uint32_t declareString(const std::string & str)					{	return instructions.declareString(str);	}

		const CodeFragment & getCode()const								{	return code;	}
		Compiler & getCompiler()const									{	return compiler;	}
		StaticData & getStaticData()const								{	return staticData;	}
		int getCurrentLine()const										{	return currentLine;	}
		//! if the setting is not defined, Instruction::INVALID_JUMP_ADDRESS is returned.
		uint32_t getCurrentMarker(setting_t markerType)const;

		varLocation_t getCurrentVarLocation(const StringId & name)const;

		std::string getInstructionsAsString()const						{	return instructions.toString();	}
		StringId getLocalVarName(const size_t index)const				{	return instructions.getLocalVarName(index);	}

		size_t getNumLocalVars()const									{	return instructions.getNumLocalVars();	}
		std::string getStringConstant(const uint32_t index)const		{	return instructions.getStringConstant(index);	}
		bool getUsesStaticVars()const									{	return usesStaticVars;	}

		void markAsUsingStaticVars()									{	usesStaticVars = true;	}

		//! Add the local variables which are already defined in the instructionBlock (e.g. 'this' or the parameters), to the set of visible variables.
		void pushSetting_basicLocalVars();

		void pushSetting_marker(setting_t type, const uint32_t marker)	{	settingsStack.push_back(SettingsStackEntry(type,marker));	}

		void pushSetting_declaredVars(const declaredVariableMap_t & variables);
		void popSetting()												{	settingsStack.pop_back();	}

		uint32_t registerInternalFunction(const ObjPtr & userFunction)	{	return instructions.registerInternalFunction(userFunction);	}
		void setLine(int l)												{	currentLine = l;	}

		//! All newly defined variables are also added to the topmost collection.
		void pushLocalVarsCollector(std::vector<size_t> * collection)	{	variableCollectorStack.push(collection);	}
		void popLocalVarsCollector()									{	variableCollectorStack.pop();	}


};
}

#endif // COMPILER_CONTEXT_H
