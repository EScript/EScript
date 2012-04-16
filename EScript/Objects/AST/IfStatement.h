// IfStatement.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_IF_STAMTEN_H
#define ES_IF_STAMTEN_H

#include "../Object.h"
#include "Statement.h"

namespace EScript {
namespace AST {

/*! [IfStatement]  ---|> [Object] */
class IfStatement : public Object {
		ES_PROVIDES_TYPE_NAME(IfStatement)
	public:
		IfStatement( Object * _condition, const Statement & _action,const Statement & _elseAction):
			condition(_condition),action(_action),elseAction(_elseAction) {}
		virtual ~IfStatement() {}

		ObjPtr getCondition()const				{	return condition;	}
		const Statement & getAction()const		{	return action;	}
		const Statement & getElseAction()const	{	return elseAction;	}

		//! ---|> Object
		virtual internalTypeId_t _getInternalTypeId()const {	return _TypeIds::TYPE_IF_STATEMENT; }

	private:
		ObjRef condition;
		Statement action;
		Statement elseAction;
};
}
}

#endif // ES_IF_STAMTEN_H
