// WhileControl.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef WHILECONTROL_H
#define WHILECONTROL_H

#include "../Object.h"
#include "../Utils/ObjRef.h"

namespace EScript {

/*! [WhileControl]  ---|> [Object]  */
class WhileControl : public Object {
		ES_PROVIDES_TYPE_NAME(WhileControl)
	public:
		WhileControl(Object * condition=NULL,Object * action=NULL,bool doWhile=false);
		virtual ~WhileControl();

		Object * getCondition()const    {   return condition.get(); }
		Object * getAction()const       {   return action.get();    }

		/// ---|> [Object]
		virtual std::string toString()const;
		virtual Object * execute(Runtime & rt);

	private:
		ObjRef condition;
		ObjRef action;
		bool doWhile;
};
}

#endif // WHILECONTROL_H
