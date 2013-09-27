// LoopStatement.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_LOOP_EXPRESSION_H
#define ES_LOOP_EXPRESSION_H

#include "ASTNode.h"

namespace EScript {
namespace AST {

//! [LoopStatement]  ---|> [ASTNode]
class LoopStatement : public ASTNode {
		ES_PROVIDES_TYPE_NAME(LoopStatement)
	public:

		//! (static) Factory: for( @p initStmt; @p cond; @p increaseStmt) @p action
		static LoopStatement * createForLoop(ptr_t _initStmt,ptr_t cond,ptr_t increaseStmt,ptr_t action,ptr_t _elseAction=nullptr){
			return new LoopStatement(_initStmt,cond,action,nullptr,increaseStmt,_elseAction);
		}

		//! (static) Factory: while( @p cond ) @p action
		static LoopStatement * createWhileLoop(ptr_t cond,ptr_t _action,ptr_t _elseAction=nullptr){
			return new LoopStatement(nullptr,cond,_action,nullptr,nullptr,_elseAction);
		}

		//! (static) Factory: do @p action while( @p cond )
		static LoopStatement * createDoWhileLoop(ptr_t cond,ptr_t _action,ptr_t _elseAction=nullptr){
			return new LoopStatement(nullptr,nullptr,_action,cond,nullptr,_elseAction);
		}

		virtual ~LoopStatement(){}

		ptr_t getInitStatement()const			{	return initStmt;	}
		ptr_t getPreConditionExpression()const	{	return preConditionExpression;	}
		ptr_t getAction()const					{	return actionStmt;	}
		ptr_t getPostConditionExpression()const	{	return postConditionExpression;	}
		ptr_t getIncreaseStatement()const		{	return increaseStmt;	}
		ptr_t getElseAction()const				{	return elseAction;	}

	private:
		LoopStatement( ptr_t _initStmt,	ptr_t _preConditionExpression, ptr_t _action, ptr_t _postConditionExpression, ptr_t _increaseStmt,ptr_t _elseAction) :
				ASTNode(TYPE_LOOP_STATEMENT,false),
				initStmt(_initStmt),preConditionExpression(_preConditionExpression),
				actionStmt(_action),postConditionExpression(_postConditionExpression),increaseStmt(_increaseStmt),elseAction(_elseAction){}

		ref_t initStmt;
		ref_t preConditionExpression;
		ref_t actionStmt;
		ref_t postConditionExpression;
		ref_t increaseStmt;
		ref_t elseAction;
};
}
}

#endif // ES_LOOP_EXPRESSION_H
