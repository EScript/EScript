// BreakStatement.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_CONTROL_STATEMENTS_H
#define ES_CONTROL_STATEMENTS_H

#include "ASTNode.h"

namespace EScript {
namespace AST {

//! [BreakStatement]  ---|> [ASTNode]
class BreakStatement : public ASTNode {
		ES_PROVIDES_TYPE_NAME(BreakStatement)
	public:
		BreakStatement() : ASTNode(TYPE_BREAK_STATEMENT,false) {}
		virtual ~BreakStatement() {}
};

//! [ContinueStatement]  ---|> [ASTNode]
class ContinueStatement : public ASTNode {
		ES_PROVIDES_TYPE_NAME(ContinueStatement)
	public:
		ContinueStatement() : ASTNode(TYPE_CONTINUE_STATEMENT,false) {}
		virtual ~ContinueStatement() {}
};

//! [ExitStatement]  ---|> [ASTNode]
class ExitStatement : public ASTNode {
		ES_PROVIDES_TYPE_NAME(ExitStatement)
	public:
		ExitStatement(ptr_t _valueExpr) : ASTNode(TYPE_EXIT_STATEMENT,false), valueExpr(_valueExpr) {}
		virtual ~ExitStatement() {}
		ptr_t getValueExpression()	{	return valueExpr;	}
	private:
		ref_t valueExpr;
};

//! [ReturnStatement]  ---|> [ASTNode]
class ReturnStatement : public ASTNode {
		ES_PROVIDES_TYPE_NAME(ReturnStatement)
	public:
		ReturnStatement(ptr_t _valueExpr) : ASTNode(TYPE_RETURN_STATEMENT,false), valueExpr(_valueExpr) {}
		virtual ~ReturnStatement() {}
		ptr_t getValueExpression()	{	return valueExpr;	}
	private:
		ref_t valueExpr;
};
//! [ThrowStatement]  ---|> [ASTNode]
class ThrowStatement : public ASTNode {
		ES_PROVIDES_TYPE_NAME(ThrowStatement)
	public:
		ThrowStatement(ptr_t _valueExpr) : ASTNode(TYPE_THROW_STATEMENT,false), valueExpr(_valueExpr) {}
		virtual ~ThrowStatement() {}
		ptr_t getValueExpression()	{	return valueExpr;	}
	private:
		ref_t valueExpr;
};

//! [YieldStatement]  ---|> [ASTNode]
class YieldStatement : public ASTNode {
		ES_PROVIDES_TYPE_NAME(YieldStatement)
	public:
		YieldStatement(ptr_t _valueExpr): ASTNode(TYPE_YIELD_STATEMENT,false), valueExpr(_valueExpr) {}
		virtual ~YieldStatement() {}
		ptr_t getValueExpression()	{	return valueExpr;	}
	private:
		ref_t valueExpr;
};


}
}

#endif // ES_CONTROL_STATEMENTS_H
