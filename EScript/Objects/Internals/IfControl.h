// IfControl.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef IFCONTROL_H
#define IFCONTROL_H

#include "../Object.h"
#include "Statement.h"

namespace EScript {

/*! [IfControl]  ---|> [Object] */
class IfControl : public Object {
		ES_PROVIDES_TYPE_NAME(IfControl)
	public:
		IfControl( Object * condition, const Statement & action,const Statement & elseAction);
		virtual ~IfControl();

		Object * getCondition()const			{	return conditionRef.get();	}
		const Statement & getAction()const		{	return actionRef;	}
		const Statement & getElseAction()const	{	return elseActionRef;	}

	private:
		ObjRef conditionRef;
		Statement actionRef;
		Statement elseActionRef;
};
}

#endif // IFCONTROL_H
