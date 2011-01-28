// ConditionalExpr.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_CONDITIONAL_EXPRESSION_H
#define ES_CONDITIONAL_EXPRESSION_H

#include "../Object.h"

namespace EScript {


/*! [ConditionalExpr]  ---|> [Object] */
class ConditionalExpr : public Object {
		ES_PROVIDES_TYPE_NAME(ConditionalExpr)
	public:
		ConditionalExpr( ObjPtr _condition=NULL, ObjPtr _action=NULL,ObjPtr _elseAction=NULL);
		virtual ~ConditionalExpr();

		Object * getCondition()const	{	return condition.get();	}
		Object * getAction()const		{	return action.get();	}
		Object * getElseAction()const	{	return elseAction.get();	}

		/// ---|> [Object]
		virtual std::string toString()const ;
		Object * execute(Runtime & rt);

	private:
		ObjRef condition;
		ObjRef action;
		ObjRef elseAction;
};
}

#endif // ES_CONDITIONAL_EXPRESSION_H
