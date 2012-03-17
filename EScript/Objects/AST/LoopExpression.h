// LoopExpression.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef LOOP_EXPRESSION_H
#define LOOP_EXPRESSION_H

#include "../Object.h"
#include "Statement.h"

namespace EScript {

/*! [LoopExpression]  ---|> [Object] */
class LoopExpression : public Object {
		ES_PROVIDES_TYPE_NAME(LoopExpression)
	public:

		//! (static) Factory: for( @p initStmt; @p cond; @p increaseStmt) @p action
		static LoopExpression * createForLoop(const Statement & _initStmt,Object * cond,const Statement & increaseStmt,const Statement & action){
			return new LoopExpression(_initStmt,cond,action,NULL,increaseStmt);
		}
						 
		//! (static) Factory: while( @p cond ) @p action
		static LoopExpression * createWhileLoop(Object * cond,const Statement & _action){
			return new LoopExpression(Statement(),cond,_action,NULL,Statement());
		}						 
		
		//! (static) Factory: do @p action while( @p cond )
		static LoopExpression * createDoWhileLoop(Object * cond,const Statement & _action){
			return new LoopExpression(Statement(),NULL,_action,cond,Statement());
		}
		
		virtual ~LoopExpression(){}



		//! ---|> Statement
		virtual void _asm(CompilerContext & ctxt);
	private:
		LoopExpression( const Statement & initStmt,
						Object * preConditionExpression,
						const Statement & action,
						Object * postConditionExpression,
						const Statement & increaseStmt);
		Statement initStmt;
		ObjRef preConditionExpression;
		Statement action;
		ObjRef postConditionExpression;
		Statement increaseStmt;
};
}

#endif // LOOP_EXPRESSION_H
