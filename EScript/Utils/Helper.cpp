// Helper.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Helper.h"
#include "Hashing.h"
#include "../Objects/Exception.h"
#include "../Objects/Namespace.h"
#include "../Objects/Object.h"
#include "../Objects/Type.h"
#include "../Parser/Compiler.h"
#include "../Parser/Parser.h"
#include "../Runtime/Runtime.h"
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
	if (min >= 0 && paramCount < min) {
		std::ostringstream sprinter;
		sprinter << "Too few parameters: Expected " << min << ", got " << paramCount << ".";
		ObjPtr caller = runtime.getCallingObject();
		if(caller.isNotNull()) {
			sprinter << caller->toString();
		}
		runtime.throwException(sprinter.str());
	} else  if (max >= 0 && paramCount > max) {
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
	runtime.throwException("Wrong object type: "+ (obj.isNull() ? std::string("NULL") : obj->toDbgString() )+ " is not of type "+className+'.');
}

//! (static)
ObjRef callMemberFunction(Runtime & rt, ObjPtr obj, StringId fnNameId, const ParameterValues & params) {
	if (obj.isNull()) {
		return NULL;
	}
	return rt.executeFunction2(obj->getAttribute(fnNameId).getValue(), obj.get(), params);
}

//! (static)
ObjRef callMemberFunction(Runtime & rt, ObjPtr obj, const std::string & fnName, const ParameterValues & params) {
	return callMemberFunction(rt, obj, StringId(fnName), params);
}

//! (static)
ObjRef callFunction(Runtime & rt, Object * function, const ParameterValues & params) {
	if (function == NULL) {
		return NULL;
	}
	return rt.executeFunction2(function, NULL, params);;
}


//! (static)
void out(Object * obj) {
	if (obj == NULL) {
		std::cout << "NULL";
	} else {
		std::cout << obj->toString();
	}
}

//! (static)
std::pair<bool, ObjRef> loadAndExecute(Runtime & runtime, const std::string & filename) {
	ERef<UserFunction> script;
	try {
		Compiler compiler(runtime.getLogger());
		script = compiler.compileFile(filename);
	} catch (Object * error) {
		std::cerr << "\nError occurred while loading file '" << filename << "':\n" << error->toString() << std::endl;
		return std::make_pair(false, error);
	}
	ObjRef result;
	try {
		result = runtime.executeFunction2(script.get(),NULL,ParameterValues());	//! \todo handle exceptions
	}catch(Object * error){
		std::cerr << "\nError occurred while executing file '" << filename << "':\n" << error->toString() << std::endl;
		return std::make_pair(false, error);
//	}catch(...){
//		std::cout << "\nCaught unknown C++ exception." << std::endl;
//		return std::make_pair(false, result.detachAndDecrease());
	}
	
	return std::make_pair(true,result.detachAndDecrease());
}
//////////
//////////Object * EScript::eval(const StringData & code){
//////////	ERef<AST::BlockStatement> block(new AST::BlockStatement());
//////////	static const StringId inline_id("[inline]");
//////////	block->setFilename(inline_id);
//////////	try{
//////////		Parser p(getLogger());
//////////		p.parse(block.get(),code);
//////////	}catch(Exception * e){
//////////		setException(e);
//////////		return NULL;
//////////	}
//////////	pushContext(RuntimeContext::create());
//////////	getCurrentContext()->createAndPushRTB(block.get());// this is later popped implicitly when the context is executed.
//////////
//////////	ObjRef resultRef = executeCurrentContext(true);
//////////	popContext();
//////////	block = NULL; // remove possibly pending reference to the result to prevent accidental deletion
//////////	return resultRef.detachAndDecrease();
//////////}

//////////! (static)
////////std::pair<bool, ObjRef> executeStream(Runtime & runtime, std::istream & stream) {
////////	ERef<AST::BlockStatement> rootBlock = new AST::BlockStatement;
////////	rootBlock->setFilename(StringId("stdin"));
////////
////////	std::string streamData;
////////	while(stream.good()) {
////////		char buffer[256];
////////		stream.read(buffer, 256);
////////		streamData.append(buffer, stream.gcount());
////////	}
////////
////////	try {
////////		Parser parser(runtime.getLogger());
////////		parser.parse(rootBlock.get(), StringData(streamData));
////////	} catch (Exception * error) {
////////		return std::make_pair(false, error);
////////	}
////////
////////	return execute(runtime, rootBlock.get());
////////}

}
