// LoopStatement.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef LOOP_EXPRESSION_H
#define LOOP_EXPRESSION_H

#include "../Object.h"
#include "Statement.h"

namespace EScript {
namespace AST {
	
/*! [LoopStatement]  ---|> [Object] */
class LoopStatement : public Object {
		ES_PROVIDES_TYPE_NAME(LoopStatement)
	public:

		//! (static) Factory: for( @p initStmt; @p cond; @p increaseStmt) @p action
		static LoopStatement * createForLoop(const Statement & _initStmt,Object * cond,const Statement & increaseStmt,const Statement & action){
			return new LoopStatement(_initStmt,cond,action,NULL,increaseStmt);
		}
						 
		//! (static) Factory: while( @p cond ) @p action
		static LoopStatement * createWhileLoop(Object * cond,const Statement & _action){
			return new LoopStatement(Statement(),cond,_action,NULL,Statement());
		}						 
		
		//! (static) Factory: do @p action while( @p cond )
		static LoopStatement * createDoWhileLoop(Object * cond,const Statement & _action){
			return new LoopStatement(Statement(),NULL,_action,cond,Statement());
		}
		
		virtual ~LoopStatement(){}
	
		const Statement & getInitStatement()const			{		return initStmt;	}
		ObjPtr getPreConditionExpression()const				{		return preConditionExpression;	}
		const Statement & getAction()const					{		return action;	}
		ObjPtr getPostConditionExpression()const			{		return postConditionExpression;	}
		const Statement & getIncreaseStatement()const		{		return increaseStmt;	}

		//! ---|> Object
		virtual internalTypeId_t _getInternalTypeId()const	{	return _TypeIds::TYPE_LOOP_STATEMENT; }

	private:
		LoopStatement( const Statement & _initStmt,	Object * _preConditionExpression, const Statement & _action, Object * _postConditionExpression, const Statement & _increaseStmt) : 
				initStmt(_initStmt),preConditionExpression(_preConditionExpression),action(_action),postConditionExpression(_postConditionExpression),increaseStmt(_increaseStmt){}
			
		Statement initStmt;
		ObjRef preConditionExpression;
		Statement action;
		ObjRef postConditionExpression;
		Statement increaseStmt;
};
}
}

#endif // LOOP_EXPRESSION_H
