// IfStatement.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef IFCONTROL_H
#define IFCONTROL_H

#include "../Object.h"
#include "Statement.h"

namespace EScript {
namespace AST {
	
/*! [IfStatement]  ---|> [Object] */
class IfStatement : public Object {
		ES_PROVIDES_TYPE_NAME(IfStatement)
	public:
		IfStatement( Object * condition, const Statement & action,const Statement & elseAction);
		virtual ~IfStatement();

		Object * getCondition()const			{	return conditionRef.get();	}
		const Statement & getAction()const		{	return actionRef;	}
		const Statement & getElseAction()const	{	return elseActionRef;	}

		//! ---|> Object
		virtual void _asm(CompilerContext & ctxt);
	private:
		ObjRef conditionRef;
		Statement actionRef;
		Statement elseActionRef;
};
}
}

#endif // IFCONTROL_H
