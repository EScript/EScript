// Function.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef LIBFUNCTION_H
#define LIBFUNCTION_H

#include "ExtObject.h"
#include "../Runtime/Runtime.h"

namespace EScript {

/*! [Function] ---|> [ExtObject]   */
class Function : public ExtObject {
		ES_PROVIDES_TYPE_NAME(Function)
	public:
		typedef Object * ( * functionPtr)(Runtime & runtime,Object * caller, const ParameterValues & parameter);
		// ---
		Function(functionPtr fnptr);
		virtual ~Function();
		functionPtr getFnPtr()const							{	return fnptr;	}

		/// ---|> [Object]
		virtual Object * clone()const 						{	return new Function(fnptr);	}
		virtual internalTypeId_t _getInternalTypeId()const 	{	return _TypeIds::TYPE_FUNCTION;	}
	private:
		functionPtr fnptr;
};

}
#endif // LIBFUNCTION_H
