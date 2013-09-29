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

//! (internal)
#define ES_FUNCTION_DEF_(_name) \
	static EScript::RtValue _name(	EScript::Runtime & rt UNUSED_ATTRIBUTE, \
									EScript::ObjPtr thisEObj UNUSED_ATTRIBUTE, \
									const EScript::ParameterValues & parameter UNUSED_ATTRIBUTE)
//---------------------------------
// macros which can be used inside init(...) functions

// -----------------
// general functions

/*! Macro for defining and declaring an EScript function.
	Available variables:
		- EScript::Runtime rt
		- EScript::ParameterValues parameter
	\example
		// Within the SomeLib::init(...) function:
		ES_FUNCTION(typeObj, "someStaticFunction", 1, 1, {
				return someStaticFunction(parameter[0].to<const SomeValue&>(rt));
		})
*/
#define ES_FUNCTION(_typeObject, _fnNameStr, _min, _max, _block) \
	{ \
		struct _fnWrapper { \
			ES_FUNCTION_DEF_(function)_block \
		}; \
		declareFunction(_typeObject, _fnNameStr, _min, _max, _fnWrapper::function); \
	}

/*! Macro for defining and declaring a (simple) EScript function in short form.
	Available variables:
		- EScript::Runtime rt
		- EScript::ParameterValues parameter
	\example
		// Within the SomeLib::init(...) function:
		ES_FUN(typeObj, "someStaticFunction", 1, 1, someStaticFunction(parameter[0].to<const SomeValue&>(rt)))
*/
#define ES_FUN(_typeObject, _fnNameStr, _min, _max, _returnExpr) \
	ES_FUNCTION(_typeObject, _fnNameStr, _min, _max,{return EScript::value(_returnExpr);})

// -----------------
// member functions

/*! Macro for defining and declaring an EScript member function.
	Available variables:
		- EScript::Runtime rt
		- EScript::ParameterValues parameter
		- EScript::Object thisEObj
		- _targetType thisObj
	\example
		// Within the SomeType::init(...) function:
		ES_MFUNCTION(typeObj, SomeType, "someMethod", 1, 1, {
				return thisObj->someMethod(parameter[0].to<const SomeValue&>(rt));
		})
*/
#define ES_MFUNCTION(_typeObject, _targetType, _fnNameStr, _min, _max, _block) \
	ES_FUNCTION(_typeObject, _fnNameStr, _min, _max,{ \
		_targetType * thisObj = thisEObj.to<_targetType*>(rt); \
		do _block while(false); \
	})

/*! Macro for defining and declaring a (simple) EScript member function in short form.
	\note The variable \a thisObj contains the thisEObj with the type \a _targetType (otherwise an exception is thrown).
	Available variables:
		- EScript::Runtime rt
		- EScript::ParameterValues parameter
		- EScript::Object thisEObj
		- _targetType thisObj
	\example
		// Within the SomeType::init(...) function:
		ES_MFUN(typeObj, SomeType, "someMethod", 1, 1, thisObj->someMethod(parameter[0].to<const SomeValue&>(rt)))
*/
#define ES_MFUN(_typeObject, _targetType, _fnNameStr, _min, _max, _returnExpr) \
	ES_MFUNCTION(_typeObject,_targetType,_fnNameStr,_min,_max,{ return EScript::value(_returnExpr); })

// -----------------
// constructors

/*! Macro for defining and declaring a EScript constructor function .
	Available variables:
		- EScript::Runtime rt
		- EScript::ParameterValues parameter
		- EScript::Type thisType
*/
#define ES_CONSTRUCTOR(_typeObject,_min,_max,_block)	\
	ES_MFUNCTION(_typeObject,EScript::Type,"_constructor",_min,_max,{ \
		EScript::Type * thisType UNUSED_ATTRIBUTE = thisObj; \
		do _block while(false); \
	})

/*! Macro for defining and declaring a EScript constructor function in short form .
	Available variables:
		- EScript::Runtime rt
		- EScript::ParameterValues parameter
		- EScript::Type thisType
*/
#define ES_CTOR(_typeObject,_min,_max,_returnExpr) \
	ES_CONSTRUCTOR(_typeObject,_min,_max,{ return EScript::value(_returnExpr); })

#endif // ES_MACROS_H_INCLUDED
