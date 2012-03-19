// ForeachStatement.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef FOREACH_EXPRESSION_H
#define FOREACH_EXPRESSION_H

#include "../Object.h"
#include "Statement.h"

namespace EScript {
namespace AST {
	
/*! [ForeachStatement]  ---|> [Object] */
class ForeachStatement : public Object {
		ES_PROVIDES_TYPE_NAME(ForeachStatement)
	public:

		ForeachStatement( Object * _collectionExpression,
						const StringId _keyId,
						const StringId _valueId,
						const Statement & _action ) : 
				collectionExpression(_collectionExpression),keyName(_keyId),valueName(_valueId),action(_action){}
						
		virtual ~ForeachStatement(){}

		//! ---|> Object
		virtual internalTypeId_t _getInternalTypeId()const {	return _TypeIds::TYPE_FOREACH_STATEMENT; }
		
	private:

		ObjRef collectionExpression;
		StringId keyName;
		StringId valueName;
		Statement action;
};
}
}

#endif // FOREACH_EXPRESSION_H
