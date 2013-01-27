// TryCatchStatement.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef TRY_CATCH_STATEMENT_H
#define TRY_CATCH_STATEMENT_H

#include "ASTNode.h"
#include "Block.h"

namespace EScript {
namespace AST {

//! [TryCatchStatement]  ---|> [ASTNode]
class TryCatchStatement : public ASTNode {
		ES_PROVIDES_TYPE_NAME(TryCatchStatement)
	public:
		TryCatchStatement( ptr_t _tryBlock, ptr_t _catchBlock, StringId _exceptionVariableName) :
				ASTNode(TYPE_TRY_CATCH_STATEMENT,false),
				tryBlock(_tryBlock), catchBlock(_catchBlock), exceptionVariableName(_exceptionVariableName){}
		virtual ~TryCatchStatement(){}

		const StringId & getExceptionVariableName()const	{	return exceptionVariableName;	}
		ptr_t getTryBlock()const							{	return tryBlock;	}
		ptr_t getCatchBlock()const							{	return catchBlock;	}

	private:
		ref_t tryBlock;
		ref_t catchBlock;
		StringId exceptionVariableName;
};
}
}

#endif // TRY_CATCH_STATEMENT_H
