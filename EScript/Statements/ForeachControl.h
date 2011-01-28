// ForeachControl.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef FOREACHCONTROL_H
#define FOREACHCONTROL_H

#include "../Object.h"

namespace EScript {

/*! [ForeachControl]  ---|> [Object]    */
class ForeachControl : public Object {
	public:
		ForeachControl(Object * arrayExpression,identifierId keyVar,identifierId valueVar, Object * action);
		virtual ~ForeachControl();

		/// ---|> [Object]
		virtual std::string toString()const;
		virtual Object * execute(Runtime & rt);

	private:
		ObjRef arrayExpressionRef;
		identifierId  keyVar;
		identifierId valueVar;
		ObjRef actionRef;
};
}

#endif // FOREACHCONTROL_H
