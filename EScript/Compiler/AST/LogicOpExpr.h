// LogicOpExpr.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_LOGICOP_EXPR_H
#define ES_LOGICOP_EXPR_H

#include "ASTNode.h"
#include <string>

namespace EScript {
namespace AST {

//! [LogicOpExpr]  ---|> [ASTNode]
class LogicOpExpr : public ASTNode {
		ES_PROVIDES_TYPE_NAME(LogicOpExpr)
	public:
		enum opType_T{	OR, AND, NOT	};

		static LogicOpExpr * createAnd(ptr_t _left,ptr_t _right)	{	return new LogicOpExpr(_left,_right,LogicOpExpr::AND);	}
		static LogicOpExpr * createNot(ptr_t expr)					{	return new LogicOpExpr(expr,nullptr,LogicOpExpr::NOT);	}
		static LogicOpExpr * createOr(ptr_t _left,ptr_t _right)		{	return new LogicOpExpr(_left,_right,LogicOpExpr::OR);	}

		virtual ~LogicOpExpr(){}

		ptr_t getLeft()const 		{	return left;	}
		ptr_t getRight()const		{	return right;	}
		opType_T getOperator()const	{	return op;	}

	private:
		LogicOpExpr(ptr_t _left,ptr_t _right,opType_T _op) : 
				ASTNode(TYPE_LOGIC_OP_EXPRESSION,true),left(_left),right(_right),op(_op) {}

		ref_t left,right;
		opType_T op;
};
}
}

#endif // ES_LOGICOP_EXPR_H
