// ConversionBasics.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_CONVERSION_BASICS_H
#define ES_CONVERSION_BASICS_H

#include <type_traits>

namespace EScript {
class Object;
class Runtime;
template<class T> class EPtr;
typedef EPtr<Object> ObjPtr;

/*! Convert the given EScript object to a desired type -- if the conversion 
	fails, an exception is thrown.	*/
template<typename target_t> target_t convertTo(Runtime &,ObjPtr);

template<class T> static T * assertType(Runtime &, const ObjPtr &); // forward declaration

/* _Internals::doConvertTo(...) works as a dispatcher for ObjPtr.to<target_t>. 
	If target_t is a pointer to an EScript object, assertType is called; convertTo<target_t> otherwise. */
namespace _Internals{
template<typename target_t> 
inline target_t doConvertTo(Runtime &runtime,const ObjPtr &src,
							typename std::enable_if<!std::is_convertible<target_t, const Object *>::value>::type * = nullptr) {
	return EScript::convertTo<target_t>(runtime,src); 
}

template<typename target_t> 
inline target_t doConvertTo(Runtime &runtime,const ObjPtr &src,
							typename std::enable_if<std::is_convertible<target_t, const Object *>::value>::type * = nullptr) {
	return EScript::assertType<typename std::remove_pointer<target_t>::type>(runtime,src);
}
}

}
/*! Defines conversion functions for converting an EScript object to a contained C++ object.
	ES_CONV_EOBJ_TO_OBJ( type of EScript object, target type, expression to convert the eObj to the target type)
	- Internally specifies two template functions; one for "target_t" and one for "const target_t".
	- Needs to be located inside the global namespace.
	- The EScript object of type eSource_t is available in the expression by the variable "eObj".
	- If a specialized implementation is needed that does not fit this macro, manually provide a 
	  template specialization of the convertTo<>(..) method. 
	\code
	  ES_CONV_EOBJ_TO_OBJ(E_SomeReferenceObj, SomeWrappedObject&, **eObj) // simple example
	\endcode
*/
#define ES_CONV_EOBJ_TO_OBJ(eSource_t,target_t,expression) \
namespace EScript{ \
	template<> inline target_t convertTo<target_t>(Runtime & rt,ObjPtr source)				{	\
		eSource_t * eObj = assertType<eSource_t>(rt,source); \
		return (expression);	}\
	template<> inline const target_t convertTo<const target_t>(Runtime & rt,ObjPtr source)				{	\
		eSource_t * eObj = assertType<eSource_t>(rt,source); \
		return (expression);	}\
}
/*! Defines a factory function for creating an EScript object for a given C++ object.
	ES_CONV_OBJ_TO_EOBJ(source type, target type of EScript object, expression to create an EScript object)
	- Needs to be located inside the global namespace.
	- The C++ object of type source_t is available in the expression by the variable "obj".
	- If a specialized implementation is needed that does not fit this macro, manually overload 
	  the method eTarget * create(source_t);
	\code
	  ES_CONV_OBJ_TO_EOBJ(const SomeWrappedObject&,E_SomeReferenceObj, new E_SomeReferenceObj(obj)) 
	  ES_CONV_OBJ_TO_EOBJ(SomeWrappedObject*, E_SomeReferenceObj, obj ? new E_SomeReferenceObj(*obj) : nullptr)
	\endcode
*/
#define ES_CONV_OBJ_TO_EOBJ(source_t,eTarget_t,expression) \
namespace EScript{ \
	inline eTarget_t * create(source_t obj)	{	return (expression);	}\
}
#endif // ES_CONVERSION_BASICS_H
