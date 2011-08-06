// Macros.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_MACROS_H_INCLUDED
#define ES_MACROS_H_INCLUDED

#if !defined(_MSC_VER)
#define UNUSED_ATTRIBUTE __attribute__ ((unused)) 
#else
#define UNUSED_ATTRIBUTE 
#endif

/*! Macro for defining an EScript function.
	\example
		ES_FUNCTION(esmf_Collection_equal) {
			assertParamCount(runtime, parameter.count(), 1, 1); // at least and at most one parameter
			return Bool::create(assertType<Collection>(runtime, caller)->rt_isEqual(runtime, parameter[0]));
		}
*/
#define ES_FUNCTION(_name) \
	static EScript::Object * _name(	EScript::Runtime & runtime UNUSED_ATTRIBUTE, \
									EScript::Object * caller UNUSED_ATTRIBUTE, \
									const EScript::ParameterValues & parameter UNUSED_ATTRIBUTE)


/*! Macro for defining a (simple) EScript function in short form.
	\example
		ESF(esmf_Collection_equal, 1, 1, Bool::create(assertType<Collection>(runtime,caller)->rt_isEqual(runtime, parameter[0])))
*/
#define ESF(_fnName, _min, _max, _returnExpr) \
	ES_FUNCTION(_fnName) { \
		EScript::assertParamCount(runtime, parameter.count(), _min, _max); \
		return (_returnExpr); \
	}


/*! Macro for defining a (simple) EScript member function in short form.
	\note The variable \a self contains the caller with the type \a _objType (otherwise an exception is thrown).
	\example
		ESMF(Collection, esmf_Collection_equal, 1, 1, Bool::create(self->rt_isEqual(runtime, parameter[0])))
*/
#define ESMF(_objType, _fnName, _min, _max, _returnExpr) \
	ES_FUNCTION(_fnName) { \
		EScript::assertParamCount(runtime, parameter.count(), _min, _max); \
		_objType * self = EScript::assertType<_objType>(runtime, caller); \
		return (_returnExpr); \
	}

//---------------------------------
// macros which can be used inside init(...) functions

/*! Macro for defining and declaring a (simple) EScript function in short form.
	\example
		// Within the Collection::init(...) function:
		ESF_DECLARE(typeObj, "==", 1, 1, Bool::create(assertType<Collection>(runtime,caller)->rt_isEqual(runtime, parameter[0])))
*/
#define ESF_DECLARE(_obj, _fnNameStr, _min, _max, _returnExpr) \
	{ \
		struct _fnWrapper { \
			ES_FUNCTION(function) {\
				return (_returnExpr); \
			} \
		}; \
		declareFunction(_obj, _fnNameStr, _min, _max, _fnWrapper::function); \
	}


/*! Macro for defining and declaring a (simple) EScript member function in short form.
	\note The variable \a self contains the caller with the type \a _objType (otherwise an exception is thrown).
	\example
		// Within the Collection::init(...) function:
		ESMF_DECLARE(typeObj, Collection, "==", 1, 1, Bool::create(self->rt_isEqual(runtime, parameter[0])))
*/
#define ESMF_DECLARE(_obj, _objType, _fnNameStr, _min, _max, _returnExpr) \
	{ \
		struct _fnWrapper { \
			ES_FUNCTION(function) {\
				_objType * self = EScript::assertType<_objType>(runtime, caller); \
				return (_returnExpr); \
			} \
		}; \
		declareFunction(_obj, _fnNameStr, _min, _max, _fnWrapper::function); \
	}


/*! Macro for defining and declaring an EScript function.
	\example
		// Within the Collection::init(...) function:
		ES_FUNCTION_DECLARE(typeObj, "==", 1, 1, {
				return Bool::create(assertType<Collection>(runtime, caller)->rt_isEqual(runtime, parameter[0]));
		})
*/
#define ES_FUNCTION_DECLARE(_obj, _fnNameStr, _min, _max, _block) \
	{ \
		struct _fnWrapper { \
			ES_FUNCTION(function){ \
				do _block while(false); \
			} \
		}; \
		declareFunction(_obj, _fnNameStr, _min, _max, _fnWrapper::function); \
	}


/*! Macro for defining and declaring an EScript member function.
	\example
		// Within the Collection::init(...) function:
		ES_MFUNCTION_DECLARE(typeObj, Collection, "==", 1, 1, {
				return Bool::create(self->rt_isEqual(runtime, parameter[0]));
		})
*/
#define ES_MFUNCTION_DECLARE(_obj, _objType, _fnNameStr, _min, _max, _block) \
	{ \
		struct _fnWrapper { \
			ES_FUNCTION(function){ \
				_objType * self = EScript::assertType<_objType>(runtime, caller); \
				do _block while(false); \
			} \
		}; \
		declareFunction(_obj, _fnNameStr, _min, _max, _fnWrapper::function); \
	}

#endif // ES_MACROS_H_INCLUDED
