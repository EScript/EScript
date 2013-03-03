// DeclarationHelper.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_DECLARATION_HELPER_H_INCLUDED
#define ES_DECLARATION_HELPER_H_INCLUDED

#include "StringId.h"
#include <cstddef>
#include <iosfwd>
#include <string>

namespace EScript {
// Forward declarations.
class ExtObject;
class Namespace;
class Object;
class RtValue;
class Runtime;
class Type;

template<class _T>class EPtr;
typedef EPtr<Object> ObjPtr;

template<class _T>class ERef;
typedef ERef<Object> ObjRef;

template<typename _T>class _ObjArray;
typedef _ObjArray<ObjRef> ParameterValues;

typedef RtValue ( * _functionPtr)(Runtime & runtime,ObjPtr caller, const ParameterValues & parameter);


//! Add a type Function attribute to @p type with given name.
void declareFunction(Type * type, StringId nameId, _functionPtr fn);
void declareFunction(Type * type, const char * name, _functionPtr fn);
void declareFunction(Type * type, StringId nameId, int minParamCount, int maxParamCount, _functionPtr fn);
void declareFunction(Type * type, const char * name, int minParamCount, int maxParamCount, _functionPtr fn);
void declareConstant(Type * type, StringId nameId, const RtValue& value);
void declareConstant(Type * type, const char * name, const RtValue& value);

void declareFunction(Namespace * nameSpace, StringId nameId, _functionPtr fn);
void declareFunction(Namespace * nameSpace, const char * name, _functionPtr fn);
void declareFunction(Namespace * nameSpace, StringId nameId, int minParamCount, int maxParamCount, _functionPtr fn);
void declareFunction(Namespace * nameSpace, const char * name, int minParamCount, int maxParamCount, _functionPtr fn);
void declareConstant(Namespace * nameSpace, StringId nameId, const RtValue& value);
void declareConstant(Namespace * nameSpace, const char * name, const RtValue& value);

//! Adds a function as object attribute (it is copied to each instance)
void declareObjectFunction(Type * type, const char * name, _functionPtr fn);

void initPrintableName(Type * type, const std::string & printableName);
void initPrintableName(ExtObject * type, const std::string & printableName);

void markAttributeAsObjectAttribute(Type * type, StringId nameId);
void markAttributeAsObjectAttribute(Type * type, const char * name);

void copyAttributeAsAlias(Type * type, const char * originalName, const char * aliasName);

}

#endif /* ES_DECLARATION_HELPER_H_INCLUDED */
