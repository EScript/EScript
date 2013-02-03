// ConversionBasics.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_CONVERSION_BASICS_H
#define ES_CONVERSION_BASICS_H

namespace EScript {
class Object;
class Runtime;
template<class T> class EPtr;
typedef EPtr<Object> ObjPtr;

template<typename target_t>
target_t convertTo(Runtime &,ObjPtr);

}
#endif // ES_CONVERSION_BASICS_H
