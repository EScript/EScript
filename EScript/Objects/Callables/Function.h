// Function.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef LIBFUNCTION_H
#define LIBFUNCTION_H

#include "../Type.h"
#include "../../Utils/ObjArray.h"
#include "../../Runtime/RtValue.h"

namespace EScript {
class Runtime;

//! [Function] ---|> [Object]
class Function : public Object {
		ES_PROVIDES_TYPE_NAME(Function)
	public:

	//! @name Initialization
	//	@{
	public:
		ESCRIPTAPI static Type* getTypeObject();
		ESCRIPTAPI static void init(EScript::Namespace & globals);
	//	@}

	// -----
		typedef RtValue ( * functionPtr)(Runtime & runtime,ObjPtr thisEObj, const ParameterValues & parameter);
		// ---
		ESCRIPTAPI Function(functionPtr fnptr);
		ESCRIPTAPI Function(StringId originalName, int minParamCount, int maxParamCount, functionPtr fnptr);
		virtual ~Function()	{}

		int getCallCounter()const							{	return callCounter;	}
		functionPtr getFnPtr()const							{	return fnptr;	}
		int getMaxParamCount()const							{	return maxParamCount;	}
		int getMinParamCount()const							{	return minParamCount;	}
		StringId getOriginalName()const						{	return originalName;	}
		void increaseCallCounter()							{	++callCounter;	}
		void resetCallCounter()								{	callCounter = 0;	}

		//! ---|> [Object]
		Object * clone()const override						{	return new Function(fnptr);	}
		internalTypeId_t _getInternalTypeId()const override	{	return _TypeIds::TYPE_FUNCTION;	}
	private:
		functionPtr fnptr;
		int minParamCount,maxParamCount;
		StringId originalName;
		int callCounter;
};

}
#endif // LIBFUNCTION_H
