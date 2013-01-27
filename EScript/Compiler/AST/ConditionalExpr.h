// ConditionalExpr.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_CONDITIONAL_EXPRESSION_H
#define ES_CONDITIONAL_EXPRESSION_H

#include "ASTNode.h"

namespace EScript {
namespace AST {

//! [ConditionalExpr]  ---|> [ASTNode]
class ConditionalExpr : public ASTNode {
		ES_PROVIDES_TYPE_NAME(ConditionalExpr)
	public:
		explicit ConditionalExpr( ptr_t _condition = nullptr, ptr_t _action = nullptr,ptr_t _elseAction = nullptr) :
				ASTNode(TYPE_CONDITIONAL_EXPRESSION,true),
				condition(_condition),action(_action),elseAction(_elseAction) {}
		virtual ~ConditionalExpr() {}

		ptr_t getCondition()const	{	return condition;	}
		ptr_t getAction()const		{	return action;	}
		ptr_t getElseAction()const	{	return elseAction;	}

	private:
		ref_t condition;
		ref_t action;
		ref_t elseAction;
};
}
}

#endif // ES_CONDITIONAL_EXPRESSION_H
