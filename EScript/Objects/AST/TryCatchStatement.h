// TryCatchStatement.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef TRY_CATCH_STATEMENT_H
#define TRY_CATCH_STATEMENT_H

#include "../Object.h"
#include "BlockStatement.h"

namespace EScript {
namespace AST {
	
/*! [TryCatchStatement]  ---|> [Object] */
class TryCatchStatement : public Object {
		ES_PROVIDES_TYPE_NAME(TryCatchStatement)
	public:
		TryCatchStatement( EPtr<BlockStatement> _tryBlock, EPtr<BlockStatement> _catchBlock, StringId _exceptionVariableName) :
				tryBlock(_tryBlock), catchBlock(_catchBlock), exceptionVariableName(_exceptionVariableName){}
		virtual ~TryCatchStatement(){}

		StringId getExceptionVariableName()const		{	return exceptionVariableName;	}
		EPtr<BlockStatement> getTryBlock()const			{	return tryBlock;	}
		EPtr<BlockStatement> getCatchBlock()const		{	return catchBlock;	}

		//! ---|> Object
		virtual internalTypeId_t _getInternalTypeId()const {	return _TypeIds::TYPE_TRY_CATCH_STATEMENT; }

	private:
		ObjRef conditionRef;
		ERef<BlockStatement> tryBlock;
		ERef<BlockStatement> catchBlock;
		StringId exceptionVariableName;
};
}
}

#endif // TRY_CATCH_STATEMENT_H
