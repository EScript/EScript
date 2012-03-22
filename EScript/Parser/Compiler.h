// Compiler.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_COMPILER_H
#define ES_COMPILER_H

#include "../Utils/Logger.h"
#include "../Utils/StringId.h"
#include "../Utils/StringData.h"
#include "../Instructions/Instruction.h"
#include "../Instructions/InstructionBlock.h"

namespace EScript {
	
class CompilerContext;
class Logger;

/*! Compiler
	Input: Syntax tree made of Expressions
	Output: Byte Code made of EAssembler-Instructions */

class Compiler {
	public:
		Compiler(Logger * _logger = NULL);
	
		void compileExpression(CompilerContext & ctxt,ObjPtr expression)const;
		
		UserFunction * compile(const StringData & code);
	
	// -------------
		
	//! @name Logging
	//	@{
	public:
		Logger * getLogger()const				{	return logger.get();	}
	private:
		void log(CompilerContext & ctxt,Logger::level_t messageLevel, const std::string & msg)const;
		_CountedRef<Logger> logger;
	//	@}

};
}

#endif // ES_COMPILER_H
