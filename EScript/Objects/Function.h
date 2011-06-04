// Function.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef LIBFUNCTION_H
#define LIBFUNCTION_H

#include "Object.h"
#include "../Runtime/Runtime.h"

namespace EScript {

/*! [Function] ---|> [Object]   */
class Function : public Object {
		ES_PROVIDES_TYPE_NAME(Function)
	public:
		typedef Object * ( * functionPtr)(Runtime & runtime,Object * caller, const ParameterValues & parameter);
		// ---
		Function(functionPtr fnptr);
		Function(identifierId originalName, int minParamCountint, int maxParamCount,functionPtr fnptr);
		virtual ~Function();
		
		functionPtr getFnPtr()const							{	return fnptr;	}
		int getMaxParamCount()const							{	return maxParamCount;	}
		int getMinParamCount()const							{	return minParamCount;	}

		/// ---|> [Object]
		virtual Object * clone()const 						{	return new Function(fnptr);	}
		virtual internalTypeId_t _getInternalTypeId()const 	{	return _TypeIds::TYPE_FUNCTION;	}
	private:
		functionPtr fnptr;
		int minParamCount,maxParamCount;
		identifierId originalName;
};

}
#endif // LIBFUNCTION_H
