// ConditionalExpr.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_CONDITIONAL_EXPRESSION_H
#define ES_CONDITIONAL_EXPRESSION_H

#include "../Object.h"

namespace EScript {
namespace AST {

/*! [ConditionalExpr]  ---|> [Object] */
class ConditionalExpr : public Object {
		ES_PROVIDES_TYPE_NAME(ConditionalExpr)
	public:
		explicit ConditionalExpr( ObjPtr _condition=NULL, ObjPtr _action=NULL,ObjPtr _elseAction=NULL) :
				condition(_condition),action(_action),elseAction(_elseAction) {}
		virtual ~ConditionalExpr() {}

		ObjPtr getCondition()const	{	return condition;	}
		ObjPtr getAction()const		{	return action;	}
		ObjPtr getElseAction()const	{	return elseAction;	}

		/// ---|> [Object]
		virtual internalTypeId_t _getInternalTypeId()const {	return _TypeIds::TYPE_CONDITIONAL_EXPRESSION; }

	private:
		ObjRef condition;
		ObjRef action;
		ObjRef elseAction;
};
}
}

#endif // ES_CONDITIONAL_EXPRESSION_H
