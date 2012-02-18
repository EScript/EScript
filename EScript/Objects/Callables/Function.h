// Function.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef LIBFUNCTION_H
#define LIBFUNCTION_H

#include "../Object.h"
#include "../../Utils/ObjArray.h"

namespace EScript {
class Runtime;

/*! [Function] ---|> [Object]   */
class Function : public Object {
		ES_PROVIDES_TYPE_NAME(Function)
	public:

	/*! @name Initialization */
	//	@{
	public:
		static Type * getTypeObject();
		static void init(EScript::Namespace & globals);
	//	@}

	// -----
		typedef Object * ( * functionPtr)(Runtime & runtime,Object * caller, const ParameterValues & parameter);
		// ---
		Function(functionPtr fnptr);
		Function(StringId originalName, int minParamCount, int maxParamCount, functionPtr fnptr);
		virtual ~Function();

		int getCallCounter()const							{	return callCounter;	}
		functionPtr getFnPtr()const							{	return fnptr;	}
		int getMaxParamCount()const							{	return maxParamCount;	}
		int getMinParamCount()const							{	return minParamCount;	}
		StringId getOriginalName()const						{	return originalName;	}
		void increaseCallCounter()							{	++callCounter;	}
		void resetCallCounter()								{	callCounter=0;	}

		/// ---|> [Object]
		virtual Object * clone()const 						{	return new Function(fnptr);	}
		virtual internalTypeId_t _getInternalTypeId()const 	{	return _TypeIds::TYPE_FUNCTION;	}
	private:
		functionPtr fnptr;
		int minParamCount,maxParamCount;
		StringId originalName;
		int callCounter;
};

}
#endif // LIBFUNCTION_H
