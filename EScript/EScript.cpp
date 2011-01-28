// EScript.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "EScript.h"
#include "Expressions/Block.h"
#include "Expressions/Identifier.h"
#include "Objects/YieldIterator.h"
#include "Parser/Parser.h"

#include "../E_Libs/StdLib.h"
#ifdef _WIN32
#include "../E_Libs/Win32Lib.h"
#endif
#include "../E_Libs/IOLib.h"
#include "../E_Libs/MathLib.h"


#include <sstream>

using namespace EScript;

//! (static)
void EScript::init() {
	Namespace * SGLOBALS=getSGlobals();

	Object::init(*SGLOBALS);
	Type::init(*SGLOBALS);
	ExtObject::init(*SGLOBALS);

	Number::init(*SGLOBALS);
	NumberRef::init(*SGLOBALS);
	Bool::init(*SGLOBALS);
	String::init(*SGLOBALS);

	Collection::init(*SGLOBALS);
	Iterator::init(*SGLOBALS);
	Array::init(*SGLOBALS);
	Map::init(*SGLOBALS);
	Exception::init(*SGLOBALS);
	Delegate::init(*SGLOBALS);
	Namespace::init(*SGLOBALS);
	YieldIterator::init(*SGLOBALS);

	declareConstant(SGLOBALS,"VERSION",String::create(ES_VERSION));
	declareConstant(SGLOBALS,"SGLOBALS",SGLOBALS);

	Parser::init(*SGLOBALS);
	Runtime::init(*SGLOBALS);

	initLibrary(StdLib::init);
	initLibrary(IOLib::init);
	initLibrary(MathLib::init);
	#ifdef _WIN32
	initLibrary(Win32Lib::init);
	#endif
}

//! (static,internal)
 Namespace * EScript::getSGlobals() {
	static ERef<Namespace> sglobals(new Namespace);
	return sglobals.get();
}


//! (static)
void EScript::initLibrary(libInitFunction * initFunction) {
	(*initFunction)(getSGlobals());
}

//! (static,internal)
void EScript::assertParamCount_2(Runtime & runtime,const ParameterValues & params,int min,int max) {
	if (min>=0 && static_cast<int>(params.count())<min) {
		std::ostringstream sprinter;
		sprinter<<"Too few parameters: Expected " <<min<<", got "<<params.count()<<".";
		Object * c=runtime.getCallingObject();
		if(c) sprinter << c->toString();
		runtime.error(sprinter.str());
	} else  if (max>=0 &&static_cast<int>(params.count())>max) {
		std::ostringstream sprinter;
		sprinter<<"Too many parameters: Expected " <<max<<", got "<<params.count()<<".";
		Object * c=runtime.getCallingObject();
		if(c) sprinter << c->toString();
		runtime.warn(sprinter.str());
	}
}

//! (static,internal) Non-inline part of assertType(...)
void EScript::assertType_throwError(Runtime & runtime,const ObjPtr & obj){
	runtime.error("Wrong Object Type.",obj.get());
}

//! (static)
Object * EScript::callMemberFunction(Runtime & rt,ObjPtr obj,identifierId fnNameId,const ParameterValues & params) {
	if (obj.isNull()) return NULL;
	return rt.executeFunction(obj->getAttribute(fnNameId),obj.get(),params);
}

//! (static)
Object * EScript::callMemberFunction(Runtime & rt,ObjPtr obj,const std::string & fnName,const ParameterValues & params) {
	return callMemberFunction(rt,obj,stringToIdentifierId(fnName),params);
}

//! (static)
Object * EScript::callFunction(Runtime & rt,Object * function,const ParameterValues & params) {
	if (!function) return NULL;
	return rt.executeFunction(function,NULL,params);;
}

//! (static)
void EScript::declareFunction( Type * t,identifierId nameId, Function::functionPtr fn){
	Function * fun=new Function(fn);
	fun->setObjAttribute(Consts::IDENTIFIER_originalId,Identifier::create(nameId));
	t->setTypeAttribute(nameId,fun);
}

//! (static)
void EScript::declareFunction( Type * t,const char *name, Function::functionPtr fn ){
	declareFunction(t,stringToIdentifierId(name),fn);
}

//! (static)
void EScript::declareConstant( Type * type,const char *name, Object * value){
	declareConstant(type,stringToIdentifierId(name),value);
}

//! (static)
void EScript::declareConstant( Type * type,identifierId nameId, Object * value){
	type->setTypeAttribute(nameId,value);
}

//! (static)
void EScript::declareFunction( Namespace * t,identifierId nameId, Function::functionPtr fn){
	Function * fun=new Function(fn);
	fun->setObjAttribute(Consts::IDENTIFIER_originalId,Identifier::create(nameId));
	t->setObjAttribute(nameId,fun);
}

//! (static)
void EScript::declareFunction( Namespace * t,const char *name, Function::functionPtr fn ){
	declareFunction(t,stringToIdentifierId(name),fn);
}

//! (static)
void EScript::declareConstant( Namespace * type,const char *name, Object * value){
	declareConstant(type,stringToIdentifierId(name),value);
}

//! (static)
void EScript::declareConstant( Namespace * type,identifierId nameId, Object * value){
	type->setObjAttribute(nameId,value);
}

//! (static)
void EScript::out(Object * obj) {
	if (!obj)
		std::cout << "NULL";
	else
		std::cout << obj->toString();
}

//! (static)
Block * EScript::loadScriptFile(const string & filename,ERef<Block> block)throw(Exception *) {
	if(block.isNull())
		block=new Block;
	ERef<Parser> pRef=new Parser;
	try{
		pRef->parseFile(block.get(),filename);
	}catch(Exception * e){
	block=NULL;
	pRef=NULL;
		throw (e);
	}
	return block.detachAndDecrease();
}

//! (static)
std::pair<bool,ObjRef> EScript::loadAndExecute(Runtime & runtime,const std::string & filename){
	ObjRef script;
	try {
		script = EScript::loadScriptFile(filename);
	} catch (Exception * error) {
		std::cerr << "\nError occured while loading file "<<filename<<" : \n" << error->toString() << std::endl;
		return std::make_pair(false,error);
	}
	bool success=true;
	ObjRef result;
	try {
		runtime.executeObj(script.get());
		result=runtime.getResult();
		if(runtime.getState()==Runtime::STATE_EXCEPTION){
			std::cout <<"\nException caught (1): \n" << result.toString() <<std::endl;;
			success=false;
		}

	}catch (Object * o) {
		result=o;
		std::cout <<"\nException caught (2): \n" << result.toString() <<std::endl;
		success=false;
	}catch (...) {
		std::cout << "\nCaught unknown C++ expception. "<<std::endl;
		success=false;
	}
	return std::make_pair(success,result);
}

////! (static)
//Map * EScript::getAttributesAsMap(Object * obj){
//    if(!obj)
//		return NULL;
//
//	std::map<identifierId,Object *> attr;
//	obj->getAttributes(attr);
//
//    Map *m=new Map();
//
//    for(std::map<identifierId,Object *>::iterator it=attr.begin();it!=attr.end();++it){
//        m->setValue(String::create( identifierIdToString(it->first) ), it->second->getRefOrCopy());
//    }
//    return m;
//}
