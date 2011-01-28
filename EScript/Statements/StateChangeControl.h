// StateChangeControl.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef STATECHANGECONTROL_H
#define STATECHANGECONTROL_H

#include "../Object.h"
#include "../Runtime/Runtime.h"

namespace EScript {

/*! [StateChangeControl]  ---|> [Object]    */
class StateChangeControl : public Object {
		ES_PROVIDES_TYPE_NAME(StateChangeControl)
	public:
		static StateChangeControl * createBreakControl();
		static StateChangeControl * createContinueControl();
		static StateChangeControl * createReturnControl(Object * expression);
		static StateChangeControl * createThrowControl(Object * expression);
		static StateChangeControl * createExitControl(Object * expression);

		StateChangeControl(Runtime::state_t _type,Object * exp);
		virtual ~StateChangeControl();

		Object * getExpression()const   {   return resultExpRef.get();  }

		/// ---|> [Object]
		virtual std::string toString()const;
		virtual Object * execute(Runtime & rt);

	private:
		ObjRef resultExpRef;
		Runtime::state_t type;
};
}

#endif // STATECHANGECONTROL_H
