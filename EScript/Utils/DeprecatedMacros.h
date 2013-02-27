// DeprecatedMacros.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_DEPRECATED_MACROS_H_INCLUDED
#define ES_DEPRECATED_MACROS_H_INCLUDED

#include "Macros.h"

/*! Macro for defining an EScript function.
	\example
		ES_FUNCTION(esmf_Collection_equal) {
			assertParamCount(runtime, parameter.count(), 1, 1); // at least and at most one parameter
			return assertType<Collection>(runtime, caller)->rt_isEqual(runtime, parameter[0]);
		}
	\deprecated
*/
#define ES_FUNCTION(_name) ES_FUNCTION_DEF_(_name)

/*! Macro for defining a (simple) EScript function in short form.
	\example
		ESF(esmf_Collection_equal, 1, 1, assertType<Collection>(runtime,caller)->rt_isEqual(runtime, parameter[0]))
	\deprecated		
*/
#define ESF(_fnName, _min, _max, _returnExpr) \
	ES_FUNCTION_DEF_(_fnName) { \
		EScript::assertParamCount(runtime, parameter.count(), _min, _max); \
		return (_returnExpr); \
	}


/*! Macro for defining a (simple) EScript member function in short form.
	\note The variable \a self contains the caller with the type \a _objType (otherwise an exception is thrown).
	\example
		ESMF(Collection, esmf_Collection_equal, 1, 1, self->rt_isEqual(runtime, parameter[0]))
	\deprecated
*/
#define ESMF(_objType, _fnName, _min, _max, _returnExpr) \
	ES_FUNCTION_DEF_(_fnName) { \
		EScript::assertParamCount(runtime, parameter.count(), _min, _max); \
		EScript::ObjPtr thisEObj(caller); \
		_objType * self = thisEObj.to<_objType*>(runtime); \
		return (_returnExpr); \
	}

#endif // ES_DEPRECATED_MACROS_H_INCLUDED
