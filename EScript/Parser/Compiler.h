// Compiler.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_COMPILER_H
#define ES_COMPILER_H

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
	
class CompilerContext;

/*! Compiler
	Input: Syntax tree made of Expressions
	Output: Byte Code made of EAssembler-Instructions */

class Compiler {
	public:
		Compiler(){}
	
		void compileExpression(CompilerContext & ctxt,ObjPtr expression)const;
};
}

#endif // ES_COMPILER_H
