#include "EScript.h"
#include "Statements/Block.h"
#include "Statements/Identifier.h"
//#include "Statements/SetAttribute.h"
#include "Objects/Script.h"
#include "Parser/Parser.h"

#include "../E_Libs/StdLib.h"
#ifdef _WIN32
#include "../E_Libs/Win32Lib.h"
#endif
#include "../E_Libs/IOLib.h"
#include "../E_Libs/MathLib.h"


#include <sstream>

using namespace EScript;


/*! (static,internal) */
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

/*! (static,internal) Non-inline part of assertType(...)   */
void EScript::assertType_throwError(Runtime & runtime,const ObjPtr & obj){
    runtime.error("Wrong Object Type.",obj.get());
}

/*! (static) */
Object * EScript::callMemberFunction(Runtime & rt,ObjPtr obj,identifierId fnNameId,const ParameterValues & params) {
    if (obj.isNull()) return NULL;
    return rt.executeFunction(obj->getAttribute(fnNameId),obj.get(),params);
}

/*! (static) */
Object * EScript::callMemberFunction(Runtime & rt,ObjPtr obj,const std::string & fnName,const ParameterValues & params) {
    return callMemberFunction(rt,obj,stringToIdentifierId(fnName),params);
}

/*! (static) */
Object * EScript::callFunction(Runtime & rt,Object * function,const ParameterValues & params) {
    if (!function) return NULL;
    return rt.executeFunction(function,NULL,params);;
}

// ---------------------------------------------------------------------------------------------------------

/*! (static) */
void EScript::initLibrary(libInitFunction * initFunction) {
    if (Script::SGLOBALS.isNull()) {
        throw("Script not initialized! Call Script::init().");
    }
    (*initFunction)(Script::SGLOBALS.get());
}

/*! (static) */
void EScript::init() {
    Script::init();
    Parser::init(*Script::SGLOBALS.get());
    Runtime::init(*Script::SGLOBALS.get());

    initLibrary(StdLib::init);
    initLibrary(IOLib::init);
    initLibrary(MathLib::init);
	#ifdef _WIN32
    initLibrary(Win32Lib::init);
	#endif
}

/*! (static) */
void EScript::declareFunction( Type * t,identifierId nameId, Function::functionPtr fn){
	Function * fun=new Function(fn);
	fun->setObjAttribute(Consts::IDENTIFIER_originalId,Identifier::create(nameId));
	t->setTypeAttribute(nameId,fun);
}

/*! (static) */
void EScript::declareFunction( Type * t,const char *name, Function::functionPtr fn ){
	declareFunction(t,stringToIdentifierId(name),fn);
}

/*! (static) */
void EScript::declareConstant( Type * type,const char *name, Object * value){
	declareConstant(type,stringToIdentifierId(name),value);
}

/*! (static) */
void EScript::declareConstant( Type * type,identifierId nameId, Object * value){
	type->setTypeAttribute(nameId,value);
}

/*! (static) */
void EScript::declareFunction( Namespace * t,identifierId nameId, Function::functionPtr fn){
	Function * fun=new Function(fn);
	fun->setObjAttribute(Consts::IDENTIFIER_originalId,Identifier::create(nameId));
	t->setObjAttribute(nameId,fun);
}

/*! (static) */
void EScript::declareFunction( Namespace * t,const char *name, Function::functionPtr fn ){
	declareFunction(t,stringToIdentifierId(name),fn);
}

/*! (static) */
void EScript::declareConstant( Namespace * type,const char *name, Object * value){
	declareConstant(type,stringToIdentifierId(name),value);
}

/*! (static) */
void EScript::declareConstant( Namespace * type,identifierId nameId, Object * value){
	type->setObjAttribute(nameId,value);
}

/*! (static) */
void EScript::out(Object * obj) {
    if (!obj)
        std::cout << "NULL";
    else
        std::cout << obj->toString();
}

/*! (static) */
Block * EScript::loadScriptFile(const string & filename,Block * b){
    if(!b)
        b=new Block;
    ERef<Parser> pRef=new Parser;
    try{
        pRef->parseFile(b,filename.c_str());
    }catch(Exception * e){
        std::cout << e->toString() <<"\n";
        throw (e);
    }

    return b;
}

///*! (static) */
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
