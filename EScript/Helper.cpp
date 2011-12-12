// Helper.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Helper.h"
#include "Objects/Exception.h"
#include "Objects/Namespace.h"
#include "Objects/Object.h"
#include "Objects/Type.h"
#include "Parser/Parser.h"
#include "Runtime/Runtime.h"
#include "Utils/Hashing.h"
#include <sstream>

namespace EScript {

//! (static)
void declareFunction(Type * type, identifierId nameId, Function::functionPtr fn) {
	type->setTypeAttribute(nameId, new Function(nameId, 0, -1, fn));
}

//! (static)
void declareFunction(Type * type, const char * name, Function::functionPtr fn) {
	declareFunction(type, stringToIdentifierId(name), fn);
}

//! (static)
void declareFunction(Type * type, identifierId nameId, int minParamCount, int maxParamCount, Function::functionPtr fn) {
	type->setTypeAttribute(nameId, new Function(nameId, minParamCount, maxParamCount, fn));
}

//! (static)
void declareFunction(Type * type, const char * name, int minParamCount, int maxParamCount, Function::functionPtr fn) {
	declareFunction(type, stringToIdentifierId(name), minParamCount, maxParamCount, fn);
}

//! (static)
void declareConstant(Type * type, const char * name, Object * value) {
	declareConstant(type, stringToIdentifierId(name), value);
}

//! (static)
void declareConstant(Type * type, identifierId nameId, Object * value) {
	type->setTypeAttribute(nameId, value);
}

//! (static)
void declareFunction(Namespace * nameSpace, identifierId nameId, Function::functionPtr fn) {
	nameSpace->setObjAttribute(nameId, new Function(nameId, 0, -1, fn));
}

//! (static)
void declareFunction(Namespace * nameSpace, const char * name, Function::functionPtr fn) {
	declareFunction(nameSpace, stringToIdentifierId(name), fn);
}

//! (static)
void declareFunction(Namespace * nameSpace, identifierId nameId, int minParamCount, int maxParamCount, Function::functionPtr fn) {
	nameSpace->setObjAttribute(nameId, new Function(nameId, minParamCount, maxParamCount, fn));
}

//! (static)
void declareFunction(Namespace * nameSpace, const char * name, int minParamCount, int maxParamCount, Function::functionPtr fn) {
	declareFunction(nameSpace, stringToIdentifierId(name), minParamCount, maxParamCount, fn);
}

//! (static)
void declareConstant(Namespace * nameSpace, const char * name, Object * value) {
	declareConstant(nameSpace, stringToIdentifierId(name), value);
}

//! (static)
void declareConstant(Namespace * nameSpace, identifierId nameId, Object * value) {
	nameSpace->setObjAttribute(nameId, value);
}

//! (static, internal)
void assertParamCount_2(Runtime & runtime, int paramCount, int min, int max) {
	if (min >= 0 && paramCount < min) {
		std::ostringstream sprinter;
		sprinter << "Too few parameters: Expected " << min << ", got " << paramCount << ".";
		Object * object = runtime.getCallingObject();
		if(object != NULL) {
			sprinter << object->toString();
		}
		runtime.error(sprinter.str());
	} else  if (max >= 0 && paramCount > max) {
		std::ostringstream sprinter;
		sprinter << "Too many parameters: Expected " << max << ", got " << paramCount << ".";
		Object * object = runtime.getCallingObject();
		if(object != NULL) {
			sprinter << object->toString();
		}
		runtime.warn(sprinter.str());
	}
}

//! (static, internal) Non-inline part of assertType(...)
void assertType_throwError(Runtime & runtime, const ObjPtr & obj) {
	runtime.error("Wrong Object Type.", obj.get());
}

//! (static)
Object * callMemberFunction(Runtime & rt, ObjPtr obj, identifierId fnNameId, const ParameterValues & params) {
	if (obj.isNull()) {
		return NULL;
	}
	return rt.executeFunction(obj->getAttribute(fnNameId), obj.get(), params);
}

//! (static)
Object * callMemberFunction(Runtime & rt, ObjPtr obj, const std::string & fnName, const ParameterValues & params) {
	return callMemberFunction(rt, obj, stringToIdentifierId(fnName), params);
}

//! (static)
Object * callFunction(Runtime & rt, Object * function, const ParameterValues & params) {
	if (function == NULL) {
		return NULL;
	}
	return rt.executeFunction(function, NULL, params);;
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
Block * loadScriptFile(const std::string & filename) throw (Exception *) {
	ERef<Block> block = new Block;
	block->setFilename(stringToIdentifierId(filename));
	Parser parser;
	try {
		parser.parseFile(block.get(), filename);
	} catch(Exception * e) {
		block = NULL;
		throw (e);
	}
	return block.detachAndDecrease();
}

//! (static)
std::pair<bool, ObjRef> loadAndExecute(Runtime & runtime, const std::string & filename) {
	ObjRef script;
	try {
		script = loadScriptFile(filename);
	} catch (Exception * error) {
		std::cerr << "\nError occured while loading file \"" << filename << "\":\n" << error->toString() << std::endl;
		return std::make_pair(false, error);
	}
	bool success = true;
	ObjRef result;
	try {
		runtime.executeObj(script.get());
		result = runtime.getResult();
		if(runtime.getState() == Runtime::STATE_EXCEPTION) {
			std::cout << "\nException caught (1):\n" << result.toString() << std::endl;
			success = false;
		}
	} catch (Object * o) {
		result = o;
		std::cout << "\nException caught (2):\n" << result.toString() << std::endl;
		success = false;
	} catch (...) {
		std::cout << "\nCaught unknown C++ exception." << std::endl;
		success = false;
	}
	return std::make_pair(success, result);
}

}
