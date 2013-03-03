// DeclarationHelper.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "DeclarationHelper.h"
#include "../Basics.h"
#include "../StdObjects.h"
//#include "Hashing.h"
#include "../Objects/Callables/Function.h"
#include "../Consts.h"
//#include "../Objects/Callables/UserFunction.h"
//#include "../Objects/Exception.h"
//#include "../Objects/Namespace.h"
//#include "../Objects/Object.h"
//#include "../Objects/Type.h"
//#include "../Objects/Values/String.h"
//#include "../Compiler/Compiler.h"
//#include "../Runtime/Runtime.h"
//#include "IO/IO.h"
//#include "../Consts.h"
//#include <sstream>

namespace EScript {

//! (static)
void declareFunction(Type * type, StringId nameId, Function::functionPtr fn) {
	type->setAttribute(nameId, Attribute(new Function(nameId, 0, -1, fn),Attribute::TYPE_ATTR_BIT));
}

//! (static)
void declareFunction(Type * type, const char * name, Function::functionPtr fn) {
	declareFunction(type, StringId(name), fn);
}

//! (static)
void declareFunction(Type * type, StringId nameId, int minParamCount, int maxParamCount, Function::functionPtr fn) {
	type->setAttribute(nameId, Attribute(new Function(nameId, minParamCount, maxParamCount, fn),Attribute::TYPE_ATTR_BIT));
}

//! (static)
void declareFunction(Type * type, const char * name, int minParamCount, int maxParamCount, Function::functionPtr fn) {
	declareFunction(type, StringId(name), minParamCount, maxParamCount, fn);
}

//! (static)
void declareConstant(Type * type, const char * name, const RtValue& value) {
	declareConstant(type, StringId(name), value);
}

//! (static)
void declareConstant(Type * type, StringId nameId, const RtValue& value) {
	type->setAttribute(nameId, Attribute(value._toObject(),Attribute::TYPE_ATTR_BIT|Attribute::CONST_BIT));
}

//! (static)
void declareFunction(Namespace * nameSpace, StringId nameId, Function::functionPtr fn) {
	nameSpace->setAttribute(nameId, Attribute(new Function(nameId, 0, -1, fn)));
}

//! (static)
void declareFunction(Namespace * nameSpace, const char * name, Function::functionPtr fn) {
	declareFunction(nameSpace, StringId(name), fn);
}

//! (static)
void declareFunction(Namespace * nameSpace, StringId nameId, int minParamCount, int maxParamCount, Function::functionPtr fn) {
	nameSpace->setAttribute(nameId, Attribute(new Function(nameId, minParamCount, maxParamCount, fn)));
}

//! (static)
void declareFunction(Namespace * nameSpace, const char * name, int minParamCount, int maxParamCount, Function::functionPtr fn) {
	declareFunction(nameSpace, StringId(name), minParamCount, maxParamCount, fn);
}

//! (static)
void declareConstant(Namespace * nameSpace, const char * name, const RtValue& value) {
	declareConstant(nameSpace, StringId(name), value);
}

//! (static)
void declareConstant(Namespace * nameSpace, StringId nameId, const RtValue& value) {
	nameSpace->setAttribute(nameId, Attribute(value._toObject(),Attribute::CONST_BIT));
}

//! (static)
void declareObjectFunction(Type * type, const char * name, _functionPtr fn){
	const auto nameId = StringId(name);
	type->setAttribute(nameId, Attribute(new Function(nameId, 0, -1, fn)));
}

//! (static)
void initPrintableName(Type * type, const std::string & printableName){
	declareConstant(type, Consts::IDENTIFIER_attr_printableName, String::create(printableName));
}

//! (static)
void initPrintableName(ExtObject * obj, const std::string & printableName){
	obj->setAttribute(Consts::IDENTIFIER_attr_printableName, Attribute(String::create(printableName),Attribute::CONST_BIT));
}

//! (static)
void markAttributeAsObjectAttribute(Type * type, StringId nameId){
	type->setAttribute(nameId, Attribute(type->getAttribute(nameId).getValue(),Attribute::NORMAL_ATTRIBUTE));
}

//! (static)
void markAttributeAsObjectAttribute(Type * type, const char * name){
	markAttributeAsObjectAttribute(type,StringId(name));
}

//! (static)
void copyAttributeAsAlias(Type * type, const char * originalName, const char * aliasName){
	type->setAttribute(StringId(aliasName), type->getAttribute(StringId(originalName)));
}

}
