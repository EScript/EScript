// LogicOpExpr.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef LOGICOP_H
#define LOGICOP_H

#include "../Object.h"
#include <string>

namespace EScript {
namespace AST {
	
/*! [LogicOpExpr]  ---|> [Object]   */
class LogicOpExpr : public Object {
		ES_PROVIDES_TYPE_NAME(LogicOpExpr)
	public:
		enum opType_T{
			OR,AND,NOT
		};

		static LogicOpExpr * createAnd(Object * _left,Object * _right)	{	return new LogicOpExpr(_left,_right,LogicOpExpr::AND);	}
		static LogicOpExpr * createNot(Object * expr)					{	return new LogicOpExpr(expr,NULL,LogicOpExpr::NOT);	}
		static LogicOpExpr * createOr(Object * _left,Object * _right)	{	return new LogicOpExpr(_left,_right,LogicOpExpr::OR);	}
		
		virtual ~LogicOpExpr(){}

		ObjPtr getLeft()const  			{   return left;   }
		ObjPtr getRight()const 			{   return right;  }
		opType_T getOperator()const   	{   return op;  }

		/// ---|> [Object]
		virtual internalTypeId_t _getInternalTypeId()const {	return _TypeIds::TYPE_LOGIC_OP_EXPRESSION; }

	private:
		LogicOpExpr(Object * _left,Object * _right,opType_T _op) : left(_left),right(_right),op(_op) {}

		ObjRef left;
		ObjRef right;
		opType_T op;
};
}
}

#endif // LOGICOP_H
