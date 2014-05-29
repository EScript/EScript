// RuntimeHelper.cpp
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2013-2014 Claudius Jähn <ClaudiusJ@live.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#include "../Basics.h"
#include "../StdObjects.h"

#include "../Objects/Callables/UserFunction.h"
#include "../Objects/Exception.h"
#include "../Compiler/Compiler.h"
#include "../Runtime/Runtime.h"
#include "IO/IO.h"
#include "../Consts.h"
#include <sstream>

namespace EScript {

namespace _Internals{
//! (static, internal)
void assertParamCount_2(Runtime & runtime, int paramCount, int min, int max) {
	if(min >= 0 && paramCount < min) {
		std::ostringstream sprinter;
		sprinter << "Too few parameters: Expected " << min << ", got " << paramCount << ".";
		ObjPtr caller = runtime.getCallingObject();
		if(caller) {
			sprinter << caller->toString();
		}
		runtime.throwException(sprinter.str());
	} else  if(max >= 0 && paramCount > max) {
		std::ostringstream sprinter;
		sprinter << "Too many parameters: Expected " << max << ", got " << paramCount << ".";
		ObjPtr caller = runtime.getCallingObject();
		if(caller) {
			sprinter << caller->toString();
		}
		runtime.warn(sprinter.str());
	}
}

//! (static, internal) Non-inline part of assertType(...)
void assertType_throwError(Runtime & runtime, const ObjPtr & obj,const char * className) {
	runtime.throwException("Wrong object type: "+ obj.toDbgString() + " is not of type "+className+'.');
}
}

//! (static)
ObjRef callMemberFunction(Runtime & runtime, ObjPtr obj, StringId fnNameId, const ParameterValues & params) {
	if(obj.isNull())
		runtime.throwException("Can not call member '"+fnNameId.toString()+"' function without object.");
	const Attribute & fun = obj->getAttribute(fnNameId).getValue();
	if(fun.isNull())
		runtime.throwException("No member to call "+obj.toDbgString()+".'"+fnNameId.toString()+"'(...).");
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
ObjRef _eval(Runtime & runtime, const CodeFragment & code,const std::unordered_map<StringId,ObjRef>& staticVars){
	Compiler compiler(runtime.getLogger());
	std::vector<StringId> staticVarNames;
	for(auto & entry: staticVars)
		staticVarNames.emplace_back(entry.first);
	auto compileUnit = compiler.compile(code,staticVarNames);
	UserFunction * script = compileUnit.first.get();
	if(!script)
		return nullptr;

	// assign injected static variable values
	auto * staticData = compileUnit.second.get();
	if(staticData){
		size_t i=0;
		for(auto & staticVarName: staticData->getStaticVariableNames()){
			const auto it = staticVars.find(staticVarName);
			if(it!=staticVars.end())
				staticData->updateStaticVariable(i,it->second.get());
			++i;
		}
	}
	return runtime.executeFunction(script,nullptr,ParameterValues());
}


//! (static)
ObjRef _loadAndExecute(Runtime & runtime, const std::string & filename,const std::unordered_map<StringId,ObjRef>& staticVars) {
	const StringData file = IO::loadFile(filename);
	return _eval(runtime,CodeFragment(StringId(filename),file),staticVars);
}

//! (static)
std::pair<bool, ObjRef> loadAndExecute(Runtime & runtime, const std::string & filename) {
	const std::unordered_map<StringId,ObjRef> staticVars;
	return loadAndExecute(runtime,filename,staticVars);
}

std::pair<bool, ObjRef> loadAndExecute(Runtime & runtime, const std::string & filename,const std::unordered_map<StringId,ObjRef>& staticVars) {
	try {
		ObjRef result = _loadAndExecute(runtime,filename,staticVars);
		ObjRef exitResult = runtime.fetchAndClearExitResult();
		return std::make_pair(true,exitResult ? exitResult : result);
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
		std::unordered_map<StringId,ObjRef> staticVars;
		ObjRef result = _eval(runtime,CodeFragment( (fileId.empty() ? Consts::FILENAME_INLINE : fileId), code),staticVars);
		return std::make_pair(true,std::move(result));
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

//! (static)
void throwRuntimeException(const std::string & what){
	throw new Exception(what);
}

}
