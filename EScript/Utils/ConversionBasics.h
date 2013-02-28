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

template<typename target_t>
target_t convertTo(Runtime &,ObjPtr);

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
#endif // ES_CONVERSION_BASICS_H
