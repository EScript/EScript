// ForeachExpression.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef FOREACH_EXPRESSION_H
#define FOREACH_EXPRESSION_H

#include "../Object.h"
#include "Statement.h"

namespace EScript {

/*! [ForeachExpression]  ---|> [Object] */
class ForeachExpression : public Object {
		ES_PROVIDES_TYPE_NAME(ForeachExpression)
	public:

		ForeachExpression( Object * collectionExpression,
						const StringId keyId,
						const StringId valueId,
						const Statement & action );
						
		virtual ~ForeachExpression(){}



		//! ---|> Statement
		virtual void _asm(CompilerContext & ctxt);
	private:

		ObjRef collectionExpression;
		StringId keyName;
		StringId valueName;
		Statement action;
};
}

#endif // FOREACH_EXPRESSION_H
