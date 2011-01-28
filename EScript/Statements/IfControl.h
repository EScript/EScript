// IfControl.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef IFCONTROL_H
#define IFCONTROL_H

#include "../Object.h"

namespace EScript {

/*! [IfControl]  ---|> [Object] */
class IfControl : public Object {
		ES_PROVIDES_TYPE_NAME(IfControl)
	public:
		IfControl( Object * condition=NULL, Object * action=NULL,Object * elseAction=NULL);
		virtual ~IfControl();

		/// ---|> [Object]
		virtual std::string toString()const ;
		Object * execute(Runtime & rt);
	private:
		ObjRef conditionRef;
		ObjRef actionRef;
		ObjRef elseActionRef;
};
}

#endif // IFCONTROL_H
