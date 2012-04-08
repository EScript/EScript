// Runtime.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Runtime.h"
#include "FunctionCallContext.h"
#include "RuntimeInternals.h"

#include "../EScript.h"
#include "../Objects/Values/Void.h"
#include "../Objects/Exception.h"
#include "../Objects/Callables/Function.h"
#include "../Objects/Callables/UserFunction.h"
#include "../Objects/Callables/Delegate.h"
#include "../Objects/YieldIterator.h"
#include "../Parser/Parser.h"
#include "../Utils/Logger.h"
#include <algorithm>
#include <iostream>
#include <list>
#include <sstream>
#include <stack>

namespace EScript{

// ----------------------------------------------------------------------
// ---- Initialization
//! (static)
Type * Runtime::getTypeObject(){
	// [Runtime] ---|> [ExtObject]
	static Type * typeObject=new Type(ExtObject::getTypeObject());
	return typeObject;
}
/**
 * initMembers
 */
void Runtime::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	declareConstant(typeObject,"LOG_DEBUG",Number::create(static_cast<int>(Logger::LOG_DEBUG)));
	declareConstant(typeObject,"LOG_INFO",Number::create(static_cast<int>(Logger::LOG_INFO)));
	declareConstant(typeObject,"LOG_PEDANTIC_WARNING",Number::create(static_cast<int>(Logger::LOG_PEDANTIC_WARNING)));
	declareConstant(typeObject,"LOG_WARNING",Number::create(static_cast<int>(Logger::LOG_WARNING)));
	declareConstant(typeObject,"LOG_ERROR",Number::create(static_cast<int>(Logger::LOG_ERROR)));
	declareConstant(typeObject,"LOG_FATAL",Number::create(static_cast<int>(Logger::LOG_FATAL)));

//	//!	[ESMF] Number Runtime._getStackSize();
//	ESF_DECLARE(typeObject,"_getStackSize",0,0, Number::create(runtime.getStackSize()))
//
//	//!	[ESMF] Number Runtime._getStackSizeLimit();
//	ESF_DECLARE(typeObject,"_getStackSizeLimit",0,0, Number::create(runtime.getStackSizeLimit()))
//
//	//!	[ESMF] void Runtime._setStackSizeLimit(number);
//	ESF_DECLARE(typeObject,"_setStackSizeLimit",1,1,
//				(runtime.setStackSizeLimit(static_cast<size_t>(parameter[0].toInt())),Void::get()) )

	//!	[ESMF] void Runtime.disableLogCounting( );
	ESF_DECLARE(typeObject,"disableLogCounting",0,1, (runtime.disableLogCounting(),Void::get()))


	//!	[ESMF] void Runtime.enableLogCounting( );
	ESF_DECLARE(typeObject,"enableLogCounting",0,1, (runtime.enableLogCounting(),Void::get()))

	//!	[ESMF] void Runtime.exception( [message] );
	ESF_DECLARE(typeObject,"exception",0,1, (runtime.setException(parameter[0].toString()),Void::get()))

	//!	[ESMF] Number Runtime.getLogCounter(Number);
	ESF_DECLARE(typeObject,"getLogCounter",1,1, Number::create(runtime.getLogCounter(static_cast<Logger::level_t>(parameter[0].toInt()))))

	//!	[ESMF] Number Runtime.getLoggingLevel();
	ESF_DECLARE(typeObject,"getLoggingLevel",0,0, Number::create(static_cast<int>(runtime.getLoggingLevel())))

	//!	[ESMF] String Runtime.getStackInfo();
	ESF_DECLARE(typeObject,"getStackInfo",0,0, String::create(runtime.getStackInfo()))

	//!	[ESMF] void Runtime.log(Number,String);
	ESF_DECLARE(typeObject,"log",2,2,
				(runtime.log(static_cast<Logger::level_t>(parameter[0].toInt()),parameter[1].toString()),Void::get()) )

	//!	[ESMF] void Runtime.resetLogCounter(Number);
	ESF_DECLARE(typeObject,"resetLogCounter",1,1,
				(runtime.resetLogCounter(static_cast<Logger::level_t>(parameter[0].toInt())),Void::get()) )

	//!	[ESMF] void Runtime.setLoggingLevel(Number);
	ESF_DECLARE(typeObject,"setLoggingLevel",1,1,
				(runtime.setLoggingLevel(static_cast<Logger::level_t>(parameter[0].toInt())),Void::get()) )

	//!	[ESMF] void Runtime.setTreatWarningsAsError(bool);
	ESF_DECLARE(typeObject,"setTreatWarningsAsError",1,1,
				(runtime.setTreatWarningsAsError(parameter[0].toBool()),Void::get()) )

	//!	[ESMF] void Runtime.warn([message]);
	ESF_DECLARE(typeObject,"warn",0,1, (runtime.warn(parameter[0].toString()),Void::get()))

	// --- internals and experimental functions

	//! [ESF]  Object _callFunction(fun[,obj[,Array params]])
	ES_FUNCTION_DECLARE(typeObject,"_callFunction",1,3, {
		ObjPtr fun(parameter[0]);
		ObjPtr obj(parameter[1].get());

		EPtr<Array> paramArr( (parameter.count()>2) ? assertType<Array>(runtime,parameter[2]) : NULL );
		ParameterValues params(paramArr.isNotNull() ? paramArr->count() : 0);
		if(paramArr.isNotNull()){
			int i=0;
			for(Array::iterator it=paramArr->begin();it!=paramArr->end();++it)
				params.set(i++,*it);
		}
		ObjRef resultRef=runtime.executeFunction(fun.get(),obj.get(),params);
		return resultRef.detachAndDecrease();
	})

	//! [ESF]  Object _getCurrentCaller()
	ESF_DECLARE(typeObject,"_getCurrentCaller",0,0, runtime.getCallingObject().get() )
}

// ----------------------------------------------------------------------
// ---- Main

//! (ctor)
Runtime::Runtime() :
		ExtObject(Runtime::getTypeObject()), internals(new RuntimeInternals(*this)), 
//		stackSizeLimit(512),
		logger(new LoggerGroup(Logger::LOG_WARNING)){

	logger->addLogger("coutLogger",new StdLogger(std::cout));
	//ctor
}

//! (dtor)
Runtime::~Runtime() {
	//dtor
}



bool Runtime::assignToAttribute(ObjPtr obj,StringId attrId,ObjPtr value){
	Attribute * attr = obj->_accessAttribute(attrId,false);
	if(attr == NULL)
		return false;

	if(attr->getProperties()&Attribute::ASSIGNMENT_RELEVANT_BITS){
		if(attr->isConst()){
			setException("Cannot assign to const attribute.");
			return true;
		}else if(attr->isPrivate()){
			if( obj!=getCallingObject() ){
				setException("Cannot assign to private attribute.");
				return true;
			}
		}
		// the attribute is a reference -> do not set the new value object but assign the new value.
		if(attr->isReference()){
			attr->getValue()->_assignValue(value);
			return true;
		}
	}
	attr->setValue(value.get());
	return true;
}

bool Runtime::checkNormalState()const				{	return internals->checkNormalState();	}

ObjRef Runtime::createInstance(const EPtr<Type> & type,const ParameterValues & _params){
	if(!internals->checkNormalState())
		return NULL;
	ParameterValues params(_params);
	RuntimeInternals::executeFunctionResult_t result = internals->startInstanceCreation(type,params);
	ObjRef realResult;
	if(result.second){
		_CountedRef<FunctionCallContext> fcc = result.second;
		realResult = internals->executeFunctionCallContext(fcc);
	}else{
		realResult = result.first;
	}
	// error occured? throw an exception!
	if(internals->getState()==RuntimeInternals::STATE_EXCEPTION){
		realResult = internals->getResult();
		internals->resetState();
		throw(realResult.detachAndDecrease());
	}
	return realResult;
}

ObjRef Runtime::executeFunction(const ObjPtr & fun,const ObjPtr & caller,const ParameterValues & _params){
	if(!internals->checkNormalState())
		return NULL;
	ParameterValues params(_params);
	RuntimeInternals::executeFunctionResult_t result = internals->startFunctionExecution(fun,caller,params);
	ObjRef realResult;
	if(result.second){
		_CountedRef<FunctionCallContext> fcc = result.second;
		realResult = internals->executeFunctionCallContext(fcc);
	}else{
		realResult = result.first;
	}
	// error occured? throw an exception!
	if(internals->getState()==RuntimeInternals::STATE_EXCEPTION){
		realResult = internals->getResult();
		internals->resetState();
		throw(realResult.detachAndDecrease());
	}
	return realResult;
}

ObjPtr Runtime::getCallingObject()const				{	return internals->getCallingObject();	}

std::string Runtime::getCurrentFile()const			{	return internals->getCurrentFile();	}

int Runtime::getCurrentLine()const					{	return internals->getCurrentLine();	}

Namespace * Runtime::getGlobals()const				{	return internals->getGlobals();	}

std::string Runtime::getStackInfo()					{	return internals->getStackInfo();	}

void Runtime::info(const std::string & s)			{	logger->info(s);	}

void Runtime::_setExceptionState(const ObjPtr e)	{	internals->setExceptionState(e);	}

void Runtime::setException(const std::string & s)	{	internals->setException(s);	}

void Runtime::setException(Exception * e)			{	internals->setException(e);	}

void Runtime::_setExitState(const ObjPtr e)			{	internals->setExitState(e);	}

void Runtime::setTreatWarningsAsError(bool b){
	if(b){ // --> disable coutLogger and add throwLogger
		Logger * coutLogger = logger->getLogger("coutLogger");
		if(coutLogger!=NULL)
			coutLogger->setMinLevel(Logger::LOG_ERROR);

		//! ThrowLogger ---|> Logger
		class ThrowLogger : public Logger{
			Runtime & rt;
			virtual void doLog(level_t,const std::string & message){	rt.setException(message);	}
		public:
			ThrowLogger(Runtime & _rt) : Logger(LOG_PEDANTIC_WARNING,LOG_WARNING), rt(_rt){}
		};
		logger->addLogger("throwLogger",new ThrowLogger(*this));
	}else{
		Logger * coutLogger = logger->getLogger("coutLogger");
		if(coutLogger!=NULL)
			coutLogger->setMinLevel(Logger::LOG_ALL);
		logger->removeLogger("throwLogger");
	}
}
void Runtime::throwException(const std::string & s,Object * obj) {
	std::ostringstream os;
	os<<s;
	if(obj) os<<'('<<obj->toString()<<')';
	os<<getStackInfo();
	Exception * e = new Exception(os.str(),getCurrentLine()); // \todo remove line
	e->setFilename(getCurrentFile());
	throw e;
}

void Runtime::warn(const std::string & s) {
	std::ostringstream os;
	os << s;
//	if(getActiveFCC()!=NULL){
//		AST::BlockStatement * b=getCurrentContext()->getCurrentRTB()->getStaticBlock();
//		if(b!=NULL)
//			os<<" ('"<<b->getFilename()<<"':~"<<getCurrentLine()<<")";
			os<<" ('"<<"':~"<<getCurrentLine()<<")";
//	}
	logger->warn(os.str());
}


void Runtime::yieldNext(YieldIterator & yIt){
	_Ptr<FunctionCallContext> fcc = yIt.getFCC();
	if(fcc.isNull()){
		setException("Invalid YieldIterator");
		return;
	}
	ObjRef result = internals->executeFunctionCallContext( fcc );
	// error occured? throw an exception!
	if(internals->getState()==RuntimeInternals::STATE_EXCEPTION){
		result = internals->getResult();
		internals->resetState();
		throw(result.detachAndDecrease());
	}
	
	ERef<YieldIterator> newYieldIterator = result.toType<YieldIterator>();
	
	// function exited with another yield? -> reuse the data for the current iterator
	if(newYieldIterator.isNotNull()){
		yIt.setFCC( newYieldIterator->getFCC() );
		yIt.setValue( newYieldIterator->value() );
	} // function returned without yield? -> update and terminate the current iterator
	else{
		yIt.setFCC( NULL );
		yIt.setValue( result );
	}

}
// ----------------------------------------------------------------------------------
// ---- Logging

//! CountingLogger ---|> Logger
class CountingLogger : public Logger{
	std::map<level_t,uint32_t> counter;
	virtual void doLog(level_t l,const std::string & ){	++counter[l];	}
public:
	CountingLogger() : Logger(LOG_ALL,LOG_NONE){}
	~CountingLogger() {}
	uint32_t get(level_t l)const{
		std::map<level_t,uint32_t>::const_iterator it = counter.find(l);
		return it == counter.end() ? 0 : it->second;
	}
	void reset(level_t l)			{	counter[l] = 0;	}
};

void Runtime::enableLogCounting(){
	if(logger->getLogger("countingLogger")==NULL)
		logger->addLogger("countingLogger",new CountingLogger());
}

void Runtime::disableLogCounting(){
	logger->removeLogger("countingLogger");
}

void Runtime::resetLogCounter(Logger::level_t level){
	CountingLogger * l = dynamic_cast<CountingLogger*>(logger->getLogger("countingLogger"));
	if(l!=NULL)
		l->reset(level);
}

uint32_t Runtime::getLogCounter(Logger::level_t level)const{
	CountingLogger * l = dynamic_cast<CountingLogger*>(logger->getLogger("countingLogger"));
	return l==NULL ? 0 : l->get(level);
}



// ------------------------------------------------------------------

}
