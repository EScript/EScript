// Helper.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Helper.h"
#include "Hashing.h"
#include "../Objects/Callables/UserFunction.h"
#include "../Objects/Exception.h"
#include "../Objects/Namespace.h"
#include "../Objects/Object.h"
#include "../Objects/Type.h"
#include "../Objects/Values/String.h"
#include "../Compiler/Compiler.h"
#include "../Runtime/Runtime.h"
#include "IO/IO.h"
#include "../Consts.h"
#include <sstream>

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
void declareConstant(Type * type, const char * name, Object * value) {
	declareConstant(type, StringId(name), value);
}

//! (static)
void declareConstant(Type * type, StringId nameId, Object * value) {
	type->setAttribute(nameId, Attribute(value,Attribute::TYPE_ATTR_BIT|Attribute::CONST_BIT));
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
void declareConstant(Namespace * nameSpace, const char * name, Object * value) {
	declareConstant(nameSpace, StringId(name), value);
}

//! (static)
void declareConstant(Namespace * nameSpace, StringId nameId, Object * value) {
	nameSpace->setAttribute(nameId, Attribute(value,Attribute::CONST_BIT));
}

//! (static)
void initPrintableName(Type * type, const std::string & printableName){
	declareConstant(type, Consts::IDENTIFIER_attr_printableName,String::create(printableName));
}


//! (static, internal)
void assertParamCount_2(Runtime & runtime, int paramCount, int min, int max) {
	if(min >= 0 && paramCount < min) {
		std::ostringstream sprinter;
		sprinter << "Too few parameters: Expected " << min << ", got " << paramCount << ".";
		ObjPtr caller = runtime.getCallingObject();
		if(caller.isNotNull()) {
			sprinter << caller->toString();
		}
		runtime.throwException(sprinter.str());
	} else  if(max >= 0 && paramCount > max) {
		std::ostringstream sprinter;
		sprinter << "Too many parameters: Expected " << max << ", got " << paramCount << ".";
		ObjPtr caller = runtime.getCallingObject();
		if(caller.isNotNull()) {
			sprinter << caller->toString();
		}
		runtime.warn(sprinter.str());
	}
}

//! (static, internal) Non-inline part of assertType(...)
void assertType_throwError(Runtime & runtime, const ObjPtr & obj,const char * className) {
	runtime.throwException("Wrong object type: "+ (obj.isNull() ? std::string("nullptr") : obj->toDbgString() )+ " is not of type "+className+'.');
}

//! (static)
ObjRef callMemberFunction(Runtime & runtime, ObjPtr obj, StringId fnNameId, const ParameterValues & params) {
	if(obj.isNull())
		runtime.throwException("Can not call member '"+fnNameId.toString()+"' function without object.");
	const Attribute & fun = obj->getAttribute(fnNameId).getValue();
	if(fun.isNull())
		runtime.throwException("No member to call "+obj->toDbgString()+".'"+fnNameId.toString()+"'(...).");
	return runtime.executeFunction(fun.getValue(), obj.get(), params);
}

//! (static)
ObjRef callFunction(Runtime & runtime, Object * function, const ParameterValues & params) {
	if(function == nullptr)
		runtime.throwException("callFunction(nullptr): no function to call.");
	return runtime.executeFunction(function, nullptr, params);
}


////! (static)
//void out(Object * obj) {
//	if(obj == nullptr) {
//		std::cout << "nullptr";
//	} else {
//		std::cout << obj->toString();
//	}
//}


//! (static)
ObjRef _eval(Runtime & runtime, const CodeFragment & code){
	Compiler compiler(runtime.getLogger());
	ERef<UserFunction> script = compiler.compile(code);
	if(script.isNull())
		return ObjRef();
	return runtime.executeFunction(script.get(),nullptr,ParameterValues());
}


//! (static)
ObjRef _loadAndExecute(Runtime & runtime, const std::string & filename) {
	const StringData file = IO::loadFile(filename);
	return _eval(runtime,CodeFragment(StringId(filename),file));
}

//! (static)
std::pair<bool, ObjRef> loadAndExecute(Runtime & runtime, const std::string & filename) {
	try {
		ObjRef result = _loadAndExecute(runtime,filename);
		return std::make_pair(true,result);
	} catch (Object * error) {
		std::ostringstream os;
		os << "Error occurred while loading file '" << filename << "':\n" << error->toString() << std::endl;
		runtime.log(Logger::LOG_ERROR,os.str());
		return std::make_pair(false, error);
	}
//	}catch(...){
//		std::cout << "\nCaught unknown C++ exception." << std::endl;
//		return std::make_pair(false, result.detachAndDecrease());
}


//! (static)
std::pair<bool, ObjRef> eval(Runtime & runtime, const StringData & code,const StringId & fileId) {
	try {
		ObjRef result = _eval(runtime,CodeFragment( (fileId.empty() ? Consts::FILENAME_INLINE : fileId), code));
		return std::make_pair(true,result);
	} catch (Object * error) {
		std::ostringstream os;
		os << "Error occurred while evaluating '" << code.str() << "':\n" << error->toString();
		runtime.log(Logger::LOG_ERROR,os.str());
		return std::make_pair(false, error);
	}
}

//! (static)
std::pair<bool, ObjRef> executeStream(Runtime & runtime, std::istream & stream) {
	std::string streamData;
	while(stream.good()) {
		char buffer[256];
		stream.read(buffer, 256);
		streamData.append(buffer, stream.gcount());
	}
	static const StringId stdinId("stdin");
	return eval(runtime,StringData(streamData),stdinId);
}

}
