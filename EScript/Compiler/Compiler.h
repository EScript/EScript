// Compiler.h
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2012-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef ES_COMPILER_H
#define ES_COMPILER_H

#include "../Utils/CodeFragment.h"
#include "../Utils/Logger.h"
#include "../Utils/StringId.h"
#include "../Utils/StringData.h"
#include "../Instructions/Instruction.h"
#include "../Instructions/InstructionBlock.h"

namespace EScript {

class FnCompileContext;
class Logger;
namespace AST{
class ASTNode;
}

/*! Compiler
	Input: Syntax tree made of Expressions
	Output: Byte Code made of EAssembler-Instructions */

class Compiler {
	public:
		Compiler(Logger * _logger = nullptr);

		UserFunction * compile(const CodeFragment & code);

	// -------------

	//! @name Logging
	//	@{
	public:
		Logger * getLogger()const				{	return logger.get();	}
	private:
		_CountedRef<Logger> logger;
	//	@}
	// -------------

	//! @name Internal helpers
	//	@{
	public:
		/*! (static,internal)
			- Replaces the markers inside the assembly by jump addresses.	*/
		static void finalizeInstructions( InstructionBlock & instructions );
		void addExpression(FnCompileContext & ctxt,EPtr<AST::ASTNode> expression)const;
		void addStatement(FnCompileContext & ctxt,EPtr<AST::ASTNode> statement)const;

		void throwError(FnCompileContext & ctxt,const std::string & message)const;
	private:
		void compileASTNode(FnCompileContext & ctxt,EPtr<AST::ASTNode> node)const;
	//	@}

};
}

#endif // ES_COMPILER_H
