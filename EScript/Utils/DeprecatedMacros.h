// DeprecatedMacros.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_DEPRECATED_MACROS_H_INCLUDED
#define ES_DEPRECATED_MACROS_H_INCLUDED

#include "Macros.h"


//! (internal)
#define ES_FUNCTION_DEF_OLD_(_name) \
	static EScript::RtValue _name(	EScript::Runtime & runtime UNUSED_ATTRIBUTE, \
									EScript::ObjPtr caller UNUSED_ATTRIBUTE, \
									const EScript::ParameterValues & parameter UNUSED_ATTRIBUTE)

//---------------------------------
// macros which can be used inside init(...) functions

/*! Macro for defining and declaring a (simple) EScript function in short form.
	\example
		// Within the Collection::init(...) function:
		ESF_DECLARE(typeObj, "==", 1, 1, assertType<Collection>(runtime,caller)->rt_isEqual(runtime, parameter[0]))
*/
#define ESF_DECLARE(_obj, _fnNameStr, _min, _max, _returnExpr) \
	{ \
		struct _fnWrapper { \
			ES_FUNCTION_DEF_OLD_(function) {\
				return EScript::value(_returnExpr); \
			} \
		}; \
		declareFunction(_obj, _fnNameStr, _min, _max, _fnWrapper::function); \
	}


/*! Macro for defining and declaring a (simple) EScript member function in short form.
	\note The variable \a self contains the caller with the type \a _objType (otherwise an exception is thrown).
	\example
		// Within the Collection::init(...) function:
		ESMF_DECLARE(typeObj, Collection, "==", 1, 1, self->rt_isEqual(runtime, parameter[0]))
*/
#define ESMF_DECLARE(_obj, _objType, _fnNameStr, _min, _max, _returnExpr) \
	{ \
		struct _fnWrapper { \
			ES_FUNCTION_DEF_OLD_(function) {\
				EScript::ObjPtr thisEObj(caller); \
				_objType * self = thisEObj.to<_objType*>(runtime); \
				return EScript::value(_returnExpr); \
			} \
		}; \
		declareFunction(_obj, _fnNameStr, _min, _max, _fnWrapper::function); \
	}


/*! Macro for defining and declaring an EScript function.
	\example
		// Within the Collection::init(...) function:
		ES_FUNCTION_DECLARE(typeObj, "==", 1, 1, {
				return assertType<Collection>(runtime, caller)->rt_isEqual(runtime, parameter[0]);
		})
*/
#define ES_FUNCTION_DECLARE(_obj, _fnNameStr, _min, _max, _block) \
	{ \
		struct _fnWrapper { \
			ES_FUNCTION_DEF_OLD_(function){ \
				do _block while(false); \
			} \
		}; \
		declareFunction(_obj, _fnNameStr, _min, _max, _fnWrapper::function); \
	}


/*! Macro for defining and declaring an EScript member function.
	\example
		// Within the Collection::init(...) function:
		ES_MFUNCTION_DECLARE(typeObj, Collection, "==", 1, 1, {
				return self->rt_isEqual(runtime, parameter[0]);
		})
*/
#define ES_MFUNCTION_DECLARE(_obj, _objType, _fnNameStr, _min, _max, _block) \
	{ \
		struct _fnWrapper { \
			ES_FUNCTION_DEF_OLD_(function){ \
				EScript::ObjPtr thisEObj(caller); \
				_objType * self = thisEObj.to<_objType*>(runtime); \
				do _block while(false); \
			} \
		}; \
		declareFunction(_obj, _fnNameStr, _min, _max, _fnWrapper::function); \
	}


/*! Macro for defining an EScript function.
	\example
		ES_FUNCTION(esmf_Collection_equal) {
			assertParamCount(runtime, parameter.count(), 1, 1); // at least and at most one parameter
			return assertType<Collection>(runtime, caller)->rt_isEqual(runtime, parameter[0]);
		}
	\deprecated
*/
#define ES_FUNCTION(_name) ES_FUNCTION_DEF_OLD_(_name)

/*! Macro for defining a (simple) EScript function in short form.
	\example
		ESF(esmf_Collection_equal, 1, 1, assertType<Collection>(runtime,caller)->rt_isEqual(runtime, parameter[0]))
	\deprecated		
*/
#define ESF(_fnName, _min, _max, _returnExpr) \
	ES_FUNCTION_DEF_OLD_(_fnName) { \
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
	ES_FUNCTION_DEF_OLD_(_fnName) { \
		EScript::assertParamCount(runtime, parameter.count(), _min, _max); \
		EScript::ObjPtr thisEObj(caller); \
		_objType * self = thisEObj.to<_objType*>(runtime); \
		return (_returnExpr); \
	}

#endif // ES_DEPRECATED_MACROS_H_INCLUDED
