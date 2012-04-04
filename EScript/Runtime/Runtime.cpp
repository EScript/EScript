// Runtime.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Runtime.h"
#include "FunctionCallContext.h"
#include "../EScript.h"
#include "../Objects/AST/ConditionalExpr.h"
#include "../Objects/AST/GetAttributeExpr.h"
#include "../Objects/AST/SetAttributeExpr.h"
#include "../Objects/AST/BlockStatement.h"
#include "../Objects/AST/IfStatement.h"
#include "../Objects/AST/FunctionCallExpr.h"
#include "../Objects/AST/LogicOpExpr.h"
#include "../Objects/AST/Statement.h"
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

	//!	[ESMF] Number Runtime._getStackSize();
	ESF_DECLARE(typeObject,"_getStackSize",0,0, Number::create(runtime.getStackSize()))

	//!	[ESMF] Number Runtime._getStackSizeLimit();
	ESF_DECLARE(typeObject,"_getStackSizeLimit",0,0, Number::create(runtime.getStackSizeLimit()))

	//!	[ESMF] void Runtime._setStackSizeLimit(number);
	ESF_DECLARE(typeObject,"_setStackSizeLimit",1,1,
				(runtime.setStackSizeLimit(static_cast<size_t>(parameter[0].toInt())),Void::get()) )

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
		ObjRef resultRef=runtime.executeFunction2(fun.get(),obj.get(),params);
		return resultRef.detachAndDecrease();
	})
	
	//! [ESF]  Object _callFunction2(fun[,obj[,Array params]])
	ES_FUNCTION_DECLARE(typeObject,"_callFunction2",1,3, {
		ObjPtr fun(parameter[0]);
		ObjPtr obj(parameter[1].get());

		EPtr<Array> paramArr( (parameter.count()>2) ? assertType<Array>(runtime,parameter[2]) : NULL );
		ParameterValues params(paramArr.isNotNull() ? paramArr->count() : 0);
		if(paramArr.isNotNull()){
			int i=0;
			for(Array::iterator it=paramArr->begin();it!=paramArr->end();++it)
				params.set(i++,*it);
		}
		ObjRef resultRef=runtime.executeFunction2(fun.get(),obj.get(),params);
		return resultRef.detachAndDecrease();
	})

	//! [ESF]  Object _getCurrentCaller()
	ESF_DECLARE(typeObject,"_getCurrentCaller",0,0, runtime.getCallingObject().get() )
}

// ----------------------------------------------------------------------
// ---- Main

//! (ctor)
Runtime::Runtime() :
		ExtObject(Runtime::getTypeObject()), stackSizeLimit(512),activeInstructionPos(-1),
		state(STATE_NORMAL),logger(new LoggerGroup(Logger::LOG_WARNING)){

	logger->addLogger("coutLogger",new StdLogger(std::cout));

	globals = EScript::getSGlobals()->clone();
	declareConstant(globals.get(),"GLOBALS",globals.get());
	declareConstant(globals.get(),"SGLOBALS",EScript::getSGlobals());

	systemFunctions.resize(7);

	//! init system calls \note the order of the functions MUST correspond to their funcitonId as defined in Consts.h
	{	// SYS_CALL_CREATE_ARRAY = 0;
		struct _{
			ESF( sysCall,0,-1,Array::create(parameter) )
		};
		systemFunctions[Consts::SYS_CALL_CREATE_ARRAY] = new Function(_::sysCall);
	}	
	{	// SYS_CALL_CREATE_MAP = 1;
		struct _{
			ES_FUNCTION( sysCall) {
					if ( (parameter.count()%2)==1 ) runtime.warn("Map: Last parameter ignored!");
					Map * a=Map::create();
					for (ParameterValues::size_type i=0;i<parameter.count();i+=2)
						a->setValue(parameter[i],parameter[i+1]);
				return a;			
			}
		};
		systemFunctions[Consts::SYS_CALL_CREATE_MAP] = new Function(_::sysCall);
	}
	{	// SYS_CALL_THROW_TYPE_EXCEPTION( expectedType, receivedValue )
		struct _{
			ES_FUNCTION( sysCall) {
				assertParamCount(runtime,2,-1);
				std::ostringstream os;
				os << "Wrong parameter type: Expected ";
				for(size_t i = 0;i<parameter.size()-1;++i ){
					if(i>0) os <<", ";
					os<<(parameter[i].isNotNull() ? parameter[i]->toDbgString() : "???");
				}
				os << " but got " << parameter[parameter.size()-1]->toDbgString()<<".";
				runtime.setException(os.str());
				return static_cast<Object*>(NULL);
			}
		};
		systemFunctions[Consts::SYS_CALL_THROW_TYPE_EXCEPTION] = new Function(_::sysCall);
	}
	{	// SYS_CALL_THROW( [value] )
		struct _{
			ESF( sysCall,0,1,(runtime.setExceptionState( parameter.count()>0 ? parameter[0] : Void::get() ),static_cast<Object*>(NULL)))
		};
		systemFunctions[Consts::SYS_CALL_THROW] = new Function(_::sysCall);
	}	
	{	// SYS_CALL_EXIT( [value] )
		struct _{
			ESF( sysCall,0,1,(runtime.setExitState( parameter.count()>0 ? parameter[0] : Void::get() ),static_cast<Object*>(NULL)))
		};
		systemFunctions[Consts::SYS_CALL_EXIT] = new Function(_::sysCall);
	}
	{	// SYS_CALL_GET_ITERATOR = 5;
		struct _{
			ES_FUNCTION( sysCall) {
				assertParamCount(runtime,1,1);
				ObjRef it;
				if(	Collection * c=parameter[0].toType<Collection>()){
					it = c->getIterator();
				}else if(parameter[0].toType<YieldIterator>()){
					it = parameter[0].get();
				}else {
					it = callMemberFunction(runtime,parameter[0] ,Consts::IDENTIFIER_fn_getIterator,ParameterValues());
				}
				if(it==NULL){
					runtime.setException("Could not get iterator from '" + parameter[0]->toDbgString() + '\'');
					return NULL;
				}
				return it.detachAndDecrease();
			}
		};
		systemFunctions[Consts::SYS_CALL_GET_ITERATOR] = new Function(_::sysCall);
	}
	{	// SYS_CALL_TEST_ARRAY_PARAMETER_CONSTRAINTS( expectedTypes*, Array receivedValue )
		struct _{
			ES_FUNCTION( sysCall) {
				assertParamCount(runtime,2,-1);
				const size_t constraintEnd = parameter.size()-1;

				Array * values = assertType<Array>(runtime,parameter[constraintEnd]);
				
				for(Array::iterator it = values->begin();it!=values->end();++it){
					bool success = false;
					for(size_t i = 0; i<constraintEnd; ++i){
						if(Runtime::checkParameterConstraint(runtime,*it,parameter[i])){
							success = true;
							break;
						}
					}
					if(!success){
						std::ostringstream os;
						os << "Wrong parameter type: Expected ";
						for(size_t i = 0;i<constraintEnd;++i ){
							if(i>0) os <<", ";
							os<<(parameter[i].isNotNull() ? parameter[i]->toDbgString() : "???");
						}
						os << " but got " << (*it)->toDbgString()<<".";
						runtime.setException(os.str());
						return static_cast<Object*>(NULL);
					}
				}
				return static_cast<Object*>(NULL);
			}
		};
		systemFunctions[Consts::SYS_CALL_TEST_ARRAY_PARAMETER_CONSTRAINTS] = new Function(_::sysCall);
	}

	//ctor
}

//! (dtor)
Runtime::~Runtime() {
	declareConstant(globals.get(), "GLOBALS",NULL);
	//dtor
}

// ----------------------------------------------------------------------
// ---- Variables

Namespace * Runtime::getGlobals()const	{
	return globals.get();
}

//ObjPtr Runtime::readMemberAttribute(ObjPtr obj,const StringId id){
//	try{
//		Attribute * attr = obj->_accessAttribute(id,false);
//		if(attr==NULL){
//			return NULL;
//		}else if(attr->isPrivate() && obj!=getCallingObject()) {
//			warn("Cannot read private attribute.");
//			return NULL;
//		}
//		return attr->getValue();
//	}catch(Exception * e){
//		ERef<Exception> eHolder(e);
//		warn(eHolder->getMessage());
//		return NULL;
//	}
//}

ObjPtr Runtime::getGlobalVariable(const StringId id) {
	// \note getLocalAttribute is used to skip the members of Type
	// 	which are otherwise found as false global variables  [BUG20100618]
	return globals->getLocalAttribute(id).getValue();
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

// ----------------------------------------------------------------------
// ---- General execution


// ----------------------------------------------------------------------
// ---- States

//! (internal)
bool Runtime::stateError(Object * obj){
	switch(getState()){
		case STATE_NORMAL:{
			return true;
		}
		case STATE_RETURNING:{
			setException("No return here!"+(obj?" ["+obj->toString()+']':""));
			break;
		}
		case STATE_BREAKING:{
			setException("No break here!"+(obj?" ["+obj->toString()+']':""));
			break;
		}
		case STATE_CONTINUING:{
			setException("No continue here!"+(obj?" ["+obj->toString()+']':""));
			break;
		}
		case STATE_YIELDING:{
			setException("No yield here!"+(obj?" ["+obj->toString()+']':""));
			break;
		}
		case STATE_EXITING:{
			break;
		}
		case STATE_EXCEPTION:{
			// we are already in an exception state...
			break;
		}
		default:
			setException("Unknown runtime state.");
	}
	return false;
}


void Runtime::info(const std::string & s) {
	logger->info(s);
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



void Runtime::setException(const std::string & s) {
	Exception * e = new Exception(s,getCurrentLine());
	e->setFilename(getCurrentFile());
	setException(e);
}

void Runtime::setException(Exception * e){
	e->setStackInfo(getStackInfo());
	setExceptionState(e);
}

void Runtime::throwException(const std::string & s,Object * obj) {
	std::ostringstream os;
	os<<s;
	if(obj) os<<"("<<obj->toString()<<')';
	os<<getStackInfo();
	Exception * e = new Exception(os.str(),getCurrentLine()); // \todo remove line
	e->setFilename(getCurrentFile());
	throw e;
}

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

// ----------------------------------------------------------------------
// ---- Debugging

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

std::string Runtime::getCurrentFile()const{
	if(getActiveFCC().isNotNull()){
		return getActiveFCC()->getUserFunction()->getFilename();
	}
	return std::string();
}

int Runtime::getCurrentLine()const{
	if(getActiveFCC().isNotNull() && activeInstructionPos>=0){
		return getActiveFCC()->getInstructions().getLine(activeInstructionPos);
	}else{
		return -1;
	}
}

std::string Runtime::getStackInfo(){
//	std::ostringstream os;
//	os<<"\n\n----------------------\nCall stack:";
//	int nr=0;
//	const int skipStart = functionCallStack.size()>50 ? 20 : functionCallStack.size()+1;
//	const int skipEnd = functionCallStack.size()>50 ? functionCallStack.size()-20 : 0;
//	for(std::vector<FunctionCallInfo>::reverse_iterator it=functionCallStack.rbegin();it!=functionCallStack.rend();++it){
//		++nr;
//		if(nr==skipStart)
//			os<<"\n\n ... \n";
//		if( nr>=skipStart && nr<skipEnd)
//			continue;
//		os<<"\n\n"<<nr<<'.';
//		FunctionCallInfo & i=*it;
//		if(i.funCall!=NULL)
//			os<< "\t"<< i.funCall->toDbgString();
//		if(i.callingObject!=NULL)
//			os<< "\ncaller:\t"<<i.callingObject;
//		if(i.function!=NULL)
//			os<< "\nfun:\t"<<i.function->toDbgString();
//		if(i.parameterValues!=NULL){
//			os<< "\nparams:\t";
//			for(ParameterValues::iterator pIt=i.parameterValues->begin();pIt!=i.parameterValues->end();++pIt){
//				if(pIt!=i.parameterValues->begin())
//					os<< ", ";
//				if( (*pIt)!=NULL)
//					os<<(*pIt)->toDbgString();
//			}
//		}
//	}
//	os<<"\n\n----------------------\n";
//	return os.str();
	return "STACKINFO"; //! \todo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}
// ---------------------------------------------------------------
// helper

//! (static)			
bool Runtime::checkParameterConstraint(Runtime & rt,const ObjPtr & value,const ObjPtr & constraint){
	return value->isA(constraint.toType<Type>()) || value->isIdentical(rt,constraint);
}


// ------------------------------------------------------------------
ObjRef Runtime::executeFunction2(const ObjPtr & fun,const ObjPtr & caller,const ParameterValues & _params){
	assertNormalState();
	ParameterValues params(_params);
	executeFunctionResult_t result = startFunctionExecution(fun,caller,params);
	ObjRef realResult;
	if(result.second){
		_CountedRef<FunctionCallContext> fcc = result.second;
		realResult = executeFunctionCallContext(fcc);
	}else{
		realResult = result.first;
	}
	// error occured? throw an exception!
	if(getState()==STATE_EXCEPTION){
		realResult = getResult();
		resetState();
		throw(realResult.detachAndDecrease());
	}
	return realResult;
}

ObjRef Runtime::createInstance(const EPtr<Type> & type,const ParameterValues & _params){
	assertNormalState();
	ParameterValues params(_params);
	executeFunctionResult_t result = startInstanceCreation(type,params);
	ObjRef realResult;
	if(result.second){
		_CountedRef<FunctionCallContext> fcc = result.second;
		realResult = executeFunctionCallContext(fcc);
	}else{
		realResult = result.first;
	}
	// error occured? throw an exception!
	if(getState()==STATE_EXCEPTION){
		realResult = getResult();
		resetState();
		throw(realResult.detachAndDecrease());
	}
	return realResult;
}

//! (internal)
Object * Runtime::executeFunctionCallContext(_Ptr<FunctionCallContext> fcc){

	fcc->enableStopExecutionAfterEnding();
	pushActiveFCC(fcc);
	InstructionBlock * instructions = &fcc->getInstructions();

	while( true ){
		// end of function? continue with calling function
		if(fcc->getInstructionCursor() >= instructions->getNumInstructions()){
			ObjRef result = fcc->getLocalVariable(Consts::LOCAL_VAR_INDEX_internalResult);
			if(fcc->isConstructorCall()){
				if(result.isNotNull()){
					warn("Constructors should not return a value.");
				}
				// \note the local variable $0 contains the created object, "fcc->getCaller()" contains the instanciated Type-Object.
				result = fcc->getLocalVariable(Consts::LOCAL_VAR_INDEX_this); 
			}
			if(fcc->isExecutionStoppedAfterEnding()){
				return result.detachAndDecrease();
			}
			popActiveFCC();
			fcc = getActiveFCC();
			if(fcc.isNull()){ //! just to be safe (should never occur)
				return result.detachAndDecrease();
			}
			
			instructions = &fcc->getInstructions();
			if(fcc->isAwaitingCaller()){
				fcc->initCaller(result);
			}else{
				fcc->stack_pushObject(result);
			}
			continue;
		}

		//! \todo this could probably improved by using iterators internally!
		activeInstructionPos = fcc->getInstructionCursor();
		const Instruction & instruction = instructions->getInstruction(activeInstructionPos);
		fcc->increaseInstructionCursor();
//
//		std::cout << fcc->stack_toDbgString()<<"\n";
//		std::cout << instruction.toString(*instructions)<<"\n";

		// --------------------------------------------------------------------------------------------------------------
		// Instructio execution...
		
		switch(instruction.getType()){

		case Instruction::I_ASSIGN_ATTRIBUTE:{
			/*	value = popValueObject
				object = popObject
				if object.identifier and not const and not private then object.identifier = value	*/
			ObjRef value = fcc->stack_popObjectValue();
			ObjRef obj = fcc->stack_popObject();

			Attribute * attr = obj->_accessAttribute(instruction.getValue_Identifier(),false);

			if(attr){
				if(attr->getProperties()&Attribute::ASSIGNMENT_RELEVANT_BITS){
					if(attr->isConst()){
						setException("Cannot assign to const attribute.");
						break;
					}else if(attr->isPrivate() && fcc->getCaller()!=obj ) {
						setException("Cannot access private attribute from outside of it owning object.");
						break;
					}
				}
				attr->setValue(value.get());
			}else{
				warn("Attribute not found..."); //! \todo proper warning!
			}
			continue;
		}
		case Instruction::I_ASSIGN_LOCAL:{
			/* 	assignLocal (uint32_t) variableIndex
				------------
				pop value
				$variableIndex = value	*/
			fcc->assignToLocalVariable(instruction.getValue_uint32(), fcc->stack_popObjectValue());
			continue;
		}

		case Instruction::I_ASSIGN_VARIABLE:{
			/*	value = popValueObject
				if caller.identifier then caller.identifier = value
				else if globals.identifier then globals.identigier = value
				else warning */
			ObjRef value = fcc->stack_popObjectValue();

			Attribute * attr = fcc->getCaller().isNotNull() ?
					fcc->getCaller()->_accessAttribute(instruction.getValue_Identifier(),false) :
					globals->_accessAttribute(instruction.getValue_Identifier(),true);
			if(attr){
				if(attr->isConst()){
					setException("Cannot assign to const attribute.");
				}else{
					attr->setValue(value.get());
				}
			}else{
				warn("Attribute not found..."); //! \todo proper warning!
			}
			continue;
		}
		case Instruction::I_CALL:{ 
			/*	call (uint32_t) numParams
				-------------
				pop numParams * parameters
				pop function
				pop object
				call the function
				push result (or jump to exception point)	*/
			const uint32_t numParams = instruction.getValue_uint32();

			std::vector<ObjRef> paramRefHolder; //! \todo why doesn't the ParameterValues keep a reference?
			paramRefHolder.reserve(numParams);
			ParameterValues params(numParams);
			for(int i=numParams-1;i>=0;--i ){
				Object * paramValue = fcc->stack_popObjectValue();
				params.set(i,paramValue);
				paramRefHolder.push_back(paramValue);
			}

			ObjRef fun = fcc->stack_popObject();
			ObjRef caller = fcc->stack_popObject();


			// returnValue , newUserFunctionCallContext
			executeFunctionResult_t result = startFunctionExecution(fun,caller,params);
			if(result.second){
				fcc = result.second;
				pushActiveFCC(fcc);
				instructions = &fcc->getInstructions();
			}else{
				fcc->stack_pushObject(result.first);
			}
			break;
		}
		case Instruction::I_CREATE_INSTANCE:{
			/*	create (uint32_t) numParams
				-------------
				pop numParams * parameters
				pop object
				call object._constructor
				push result (or jump to exception point)	*/
				

			// get the parameters for the first constructor
			const uint32_t numParams = instruction.getValue_uint32();
			std::vector<ObjRef> paramRefHolder; //! \todo why doesn't the ParameterValues keep a reference?
			paramRefHolder.reserve(numParams);
			ParameterValues params(numParams);
			for(int i=numParams-1;i>=0;--i ){
				Object * paramValue = fcc->stack_popObjectValue();
				params.set(i,paramValue);
				paramRefHolder.push_back(paramValue);
			}

			// collect constructor functions
			ObjRef caller = fcc->stack_popObject();
			EPtr<Type> type = caller.toType<Type>();
			if(type.isNull()){
				setException("Can't instanciate object not of type 'Type'");
				break;
			}

			executeFunctionResult_t result = startInstanceCreation(type,params);
			if(result.second){
				fcc = result.second;
				pushActiveFCC(fcc);
				instructions = &fcc->getInstructions();
			}else{
				fcc->stack_pushObject(result.first);
			}

			break;
		}
		case Instruction::I_CHECK_TYPE:{
			/*	checkType localVariableIdx
				--------------
				pop (Object) TypeOrObj
				if(localVar ---|> Type || localVar == Obj)
					push true
				else 
					push false
			*/
			const ObjPtr localVariable = fcc->getLocalVariable(instruction.getValue_uint32());
			const ObjRef typeOrObj = fcc->stack_popObject();
			fcc->stack_pushBool( checkParameterConstraint(*this,localVariable,typeOrObj) );
			continue;
		}
		case Instruction::I_DUP:{
			// duplicate topmost stack entry
			fcc->stack_dup();
			continue;
		}
		case Instruction::I_FIND_VARIABLE:{
			/*	if caller.Identifier -> push (caller, caller.Identifier)
				else push (GLOBALS, GLOBALS.Identifier) (or NULL,NULL + Warning) 	*/
			if(fcc->getCaller().isNotNull()){
				const Attribute & attr = fcc->getCaller()->getAttribute(instruction.getValue_Identifier());
				if(attr.isNotNull()){
					fcc->stack_pushObject(fcc->getCaller());
					fcc->stack_pushObject(attr.getValue());
					continue;
				}
			}
			ObjPtr obj = getGlobalVariable(instruction.getValue_Identifier());
			if(obj.isNotNull()){
				fcc->stack_pushObject(globals.get());
				fcc->stack_pushObject(obj);
			}else{
				warn("Variable not found: "); //! \todo proper warning!
				fcc->stack_pushVoid();
				fcc->stack_pushVoid();
			}
			break;
		}
		case Instruction::I_GET_ATTRIBUTE:{ //! \todo check for @(private)
			/*	pop Object
				push Object.Identifier (or NULL + Warning)	*/
			ObjRef obj = fcc->stack_popObject();
			const Attribute & attr = obj->getAttribute(instruction.getValue_Identifier());
			if(attr.isNull()) {
				warn("Attribute not found: "); //! \todo add proper warning
				fcc->stack_pushVoid();
			}else{
				fcc->stack_pushObject( attr.getValue() );
			}
			continue;
		}
		case Instruction::I_GET_VARIABLE:{
			/*	if caller.Identifier -> push (caller.Identifier)
				else push (GLOBALS.Identifier) (or NULL + Warning) 	*/
			if(fcc->getCaller().isNotNull()){
				const Attribute & attr = fcc->getCaller()->getAttribute(instruction.getValue_Identifier());
				if(attr.isNotNull()){
					fcc->stack_pushObject(attr.getValue());
					continue;
				}
			}
			ObjPtr obj = getGlobalVariable(instruction.getValue_Identifier());
			if(obj.isNotNull()){
				fcc->stack_pushObject(obj);
			}else{
				warn("Variable not found: "); //! \todo proper warning!
				fcc->stack_pushVoid();
			}
			break;
		}
		case Instruction::I_GET_LOCAL_VARIABLE:{
			/* 	getLocalVariable (uint32_t) variableIndex
				------------
				push $variableIndex	*/
			fcc->stack_pushObject( fcc->getLocalVariable(instruction.getValue_uint32()).get() );
			continue;
		}
		case Instruction::I_INIT_CALLER:{
			const uint32_t numParams = instruction.getValue_uint32();

			if(fcc->stack_empty()){ // no constructor call
				if(numParams>0){
					warn("Calling constructor function with @(super) attribute as normal function.");
				}
//				fcc->initCaller(fcc->getCaller()); ???????
				continue;
			}

			// get super constructor parameters
			std::vector<ObjRef> paramRefHolder; //! \todo why doesn't the ParameterValues keep a reference?
			paramRefHolder.reserve(numParams);
			ParameterValues params(numParams);
			for(int i=numParams-1;i>=0;--i ){
				Object * paramValue = fcc->stack_popObjectValue();
				params.set(i,paramValue);
				paramRefHolder.push_back(paramValue);
			}

			// call super constructor
			ObjRef superConstructor = fcc->stack_popObjectValue();
			executeFunctionResult_t result = startFunctionExecution(superConstructor,fcc->getCaller(),params);
			std::vector<ObjPtr> constructors;
			while(!fcc->stack_empty()){
				constructors.push_back(fcc->stack_popObject());
			}
			
			if(result.second){
				// pass remaining super constructors to new calling context
				fcc = result.second;
				pushActiveFCC(fcc);
				instructions = &fcc->getInstructions();
				for(std::vector<ObjPtr>::const_reverse_iterator it = constructors.rbegin();it!=constructors.rend();++it) 
					fcc->stack_pushObject( *it );
				fcc->enableAwaitingCaller();
				fcc->markAsConstructorCall();
			}else{
				ObjPtr newObj = result.first;
				if(newObj.isNull()){
					if(state!=STATE_EXCEPTION) // if an exception occured in the constructor, the result may be NULL
						setException("Constructor did not create an Object."); //! \todo improve message!
					break;
				}
				fcc->stack_pushObject(newObj);
				// init attribute
				newObj->_initAttributes(*this); //! \todo catch exceptions!!!!!!!!
			}

			break;
		}		
		case Instruction::I_JMP:{
			fcc->setInstructionCursor( instruction.getValue_uint32() );
			continue;
		}
		case Instruction::I_JMP_IF_SET:{
			/* 	jmpIfSet (uint32) targetAddress
				-------------
				pop (uint32) local variable index
				jmp if variable != NULL */
			if( fcc->getLocalVariable( fcc->stack_popUInt32() ).isNotNull() )
				fcc->setInstructionCursor( instruction.getValue_uint32() );
			continue;
		}
		case Instruction::I_JMP_ON_TRUE:{
			if(fcc->stack_popBool())
				fcc->setInstructionCursor( instruction.getValue_uint32() );
			continue;
		}
		case Instruction::I_JMP_ON_FALSE:{
			if(!fcc->stack_popBool())
				fcc->setInstructionCursor( instruction.getValue_uint32() );
			continue;
		}
		case Instruction::I_NOT:{
			/*	not
				-------
				bool b = popBool
				push !b	*/
			fcc->stack_pushBool( !fcc->stack_popBool() );
			continue;
		}
		case Instruction::I_POP:{
			// remove entry from stack
			fcc->stack_pop();
			continue;
		}
		case Instruction::I_PUSH_BOOL:{
			fcc->stack_pushBool( instruction.getValue_Bool() );
			continue;
		}
		case Instruction::I_PUSH_ID:{
			fcc->stack_pushIdentifier( instruction.getValue_Identifier() );
			continue;
		}
		case Instruction::I_PUSH_FUNCTION:{
			fcc->stack_pushFunction( instruction.getValue_uint32() );
			continue;
		}
		case Instruction::I_PUSH_NUMBER:{
			fcc->stack_pushNumber( instruction.getValue_Number() );
			continue;
		}
		case Instruction::I_PUSH_STRING:{
			fcc->stack_pushStringIndex( instruction.getValue_uint32() );
			continue;
		}
		case Instruction::I_PUSH_UINT:{
			fcc->stack_pushUInt32( instruction.getValue_uint32() );
			continue;
		}
		case Instruction::I_PUSH_VOID:{
			fcc->stack_pushVoid( );
			continue;
		}
		case Instruction::I_RESET_LOCAL_VARIABLE:{
			// $localVarId = NULL
			fcc->assignToLocalVariable(instruction.getValue_uint32(), NULL);
			continue;
		}
		case Instruction::I_SET_ATTRIBUTE:{
			/*	setAttribute identifierId
				-------------
				properies = pop uint32
				obj = pop object
				value = pop parameterObj
				obj.identifier @(properties) := value	*/

			const uint32_t properties = fcc->stack_popUInt32();
			ObjRef obj = fcc->stack_popObject();
			ObjRef value = fcc->stack_popObjectValue();
			if(!obj->setAttribute(instruction.getValue_Identifier(),Attribute(value,properties))){
				warn("Could not set Attribute..."); //! \todo proper warning!
			}
			continue;
		}
		case Instruction::I_SET_EXCEPTION_HANDLER:{
			fcc->setExceptionHandlerPos(instruction.getValue_uint32());
			continue;
		}
		case Instruction::I_SYS_CALL:{
			/*	sysCall (uint32_t) numParams
				-------------
				pop functionId
				pop numParams * parameters
				sysCall functionId,parameters
				push result (or jump to exception point)	*/
			const uint32_t numParams = instruction.getValue_uint32();
			const uint32_t funId = fcc->stack_popUInt32();
			
			std::vector<ObjRef> paramRefHolder; //! \todo why doesn't the ParameterValues keep a reference?
			paramRefHolder.reserve(numParams);
			ParameterValues params(numParams);
			for(int i=numParams-1;i>=0;--i ){
				Object * paramValue = fcc->stack_popObjectValue();
				params.set(i,paramValue);
				paramRefHolder.push_back(paramValue);
			}
			fcc->stack_pushObject(sysCall(funId,params));
			break;
		}
		case Instruction::I_YIELD:{
			/*	yield
				-------------
				pop result	*/
			ObjRef value = fcc->stack_popObjectValue();
			ERef<YieldIterator> yIt = new YieldIterator();
			yIt->setFCC(fcc);
			yIt->setValue(value);
			if(fcc->isExecutionStoppedAfterEnding()){
				popActiveFCC();
				return yIt.detachAndDecrease();
			} // continue with the next fcc...
			else{
				popActiveFCC();
				fcc = getActiveFCC();
				if(fcc.isNull())
					return NULL;
				instructions = &fcc->getInstructions();
				fcc->stack_pushObject(yIt.get());
			}
			break;
		}
		case Instruction::I_UNDEFINED:
		case Instruction::I_SET_MARKER:
		default:{
			warn("Unknown Instruction");
		}
		}
		// --------------------------------------------------------------------------------------------------------------
		if(getState()==STATE_NORMAL){
			continue;
		}else if(getState()==STATE_EXCEPTION){
			while(true){
				fcc->stack_clear(); // remove current stack content
				
				// catch-block available?
				if(fcc->getExceptionHandlerPos()!=Instruction::INVALID_JUMP_ADDRESS){
					fcc->assignToLocalVariable(Consts::LOCAL_VAR_INDEX_internalResult,getResult()); // ___result = exceptionResult
					resetState();
					fcc->setInstructionCursor(fcc->getExceptionHandlerPos());
					break;
				} // execution stops here? Keep the exception-state and return;
				else if(fcc->isExecutionStoppedAfterEnding()){
					popActiveFCC();
					return NULL;
				} // continue with the next fcc...
				else{
					popActiveFCC();
					fcc = getActiveFCC();
					if(fcc.isNull())
						return NULL;
					instructions = &fcc->getInstructions();
				}
			}
		} else if(getState()==STATE_EXITING){
			while(true){
				// execution stops here? Keep the exiting-state and return;
				if(fcc->isExecutionStoppedAfterEnding()){
					popActiveFCC();
					return NULL;
				} // continue with the next fcc...
				else{
					popActiveFCC();
					fcc = getActiveFCC();
					if(fcc.isNull())
						return NULL;
					instructions = &fcc->getInstructions();
				}
			}
		}
	}
	return Void::get();
}

//! (internal)
Runtime::executeFunctionResult_t Runtime::startFunctionExecution(const ObjPtr & fun,const ObjPtr & _callingObject,ParameterValues & params){
	ObjPtr result;
	switch( fun->_getInternalTypeId() ){
		case _TypeIds::TYPE_USER_FUNCTION:{
			UserFunction * userFunction = static_cast<UserFunction*>(fun.get());
			_CountedRef<FunctionCallContext> fcc = FunctionCallContext::create(userFunction,_callingObject);

			// check for too few parameter values -> throw exception
			if(userFunction->getMinParamCount()>=0 && params.size()<static_cast<size_t>(userFunction->getMinParamCount())){
				setException("Too few parameters given."); //! \todo improve error message (which parameters are missing?)
				return std::make_pair(result.get(),static_cast<FunctionCallContext*>(NULL));
			}

			ParameterValues::const_iterator paramsEnd;

			const int maxParamCount = userFunction->getMaxParamCount();
			if( maxParamCount<0 ){ // multiParameter
				ERef<Array> multiParamArray = Array::create();
				// copy values into multiParamArray
				if(params.size()>=userFunction->getParamCount()){
					for(size_t i = userFunction->getParamCount()-1;i<params.size();++i){
						multiParamArray->pushBack(params[i]);
					}
					paramsEnd = params.begin()+(userFunction->getParamCount()-1);

				}else{
					paramsEnd = params.end();
				}
				// directly assign to last parameter
				fcc->assignToLocalVariable(Consts::LOCAL_VAR_INDEX_firstParameter + (userFunction->getParamCount()-1) ,multiParamArray.get());
			} // too many parameters
			else if( params.size()>static_cast<size_t>(maxParamCount) ){
				warn("Too many parameters given.");  //! \todo improve warning message (how many?)
				paramsEnd = params.begin()+maxParamCount; // std::next(...)
			} // normal parameter count range
			else{
				paramsEnd = params.end();
			}

			// init $thisFn
			fcc->assignToLocalVariable(Consts::LOCAL_VAR_INDEX_thisFn,fun);

			uint32_t i = Consts::LOCAL_VAR_INDEX_firstParameter;
			for(ParameterValues::const_iterator it = params.begin(); it!= paramsEnd; ++it){
				fcc->assignToLocalVariable(i++,*it);
			}

			return std::make_pair(result.get(),fcc.detachAndDecrease());
		}
		case _TypeIds::TYPE_DELEGATE:{
			Delegate * delegate = static_cast<Delegate*>(fun.get());
			return startFunctionExecution(delegate->getFunction(),delegate->getObject(),params);
		}
		case _TypeIds::TYPE_FUNCTION:{ // is  C++ function ?
			Function * libfun=static_cast<Function*>(fun.get());
			{	// check param count
				const int min = libfun->getMinParamCount();
				const int max = libfun->getMaxParamCount();
				if( (min>0 && static_cast<int>(params.count())<min)){
					std::ostringstream sprinter;
					sprinter<<"Too few parameters: Expected " <<min<<", got "<<params.count()<<'.';
					//! \todo improve message
					setException(sprinter.str());
					return std::make_pair(result.get(),static_cast<FunctionCallContext*>(NULL));
				} else  if (max>=0 && static_cast<int>(params.count())>max) {
					std::ostringstream sprinter;
					sprinter<<"Too many parameters: Expected " <<max<<", got "<<params.count()<<'.';
					//! \todo improve message
					warn(sprinter.str());
				}
			}
			libfun->increaseCallCounter();

			try {
				result = (*libfun->getFnPtr())(*this,_callingObject.get(),params);
			} catch (Exception * e) {
				setExceptionState(e);
			} catch(const char * message) {
				setException(std::string("C++ exception: ")+message);
			} catch(const std::string & message) {
				setException(std::string("C++ exception: ") + message);
			} catch(const std::exception & e) {
				setException(std::string("C++ exception: ") + e.what());
			} catch (Object * obj) {
				// workaround: this should be covered by catching the Exception* directly, but that doesn't always seem to work!?!
				Exception * e=dynamic_cast<Exception *>(obj);
				if(e){
					setExceptionState(e);
				}else{
					const std::string message=(obj?obj->toString():"NULL");
					setException(message);
				}
			}  catch (...){
				setException("C++ exception");
			}
			break;
		}

		default:{
			// function-object has a user defined "_call"-member?
			const Attribute & attr = fun->getAttribute(Consts::IDENTIFIER_fn_call);		//! \todo check for @(private)
			
			if(attr.getValue()){
				// fun._call( callingObj , param0 , param1 , ... )
				ParameterValues params2(params.count()+1);
				params2.set(0,_callingObject.isNotNull() ? _callingObject : Void::get());
				std::copy(params.begin(),params.end(),params2.begin()+1);

				return startFunctionExecution(attr.getValue(),fun,params2);
			}

			warn("Cannot use '"+fun->toDbgString()+"' as a function.");
	
		}
	}

	return std::make_pair(result.get(),static_cast<FunctionCallContext*>(NULL));
}


//! (internal)
Runtime::executeFunctionResult_t Runtime::startInstanceCreation(EPtr<Type> type,ParameterValues & params){
	static const executeFunctionResult_t failureResult = std::make_pair(static_cast<Object*>(NULL),static_cast<FunctionCallContext*>(NULL));
	ObjRef createdObject;
	
	// collect constructors
	std::vector<ObjPtr> constructors;
	do{
		const Attribute * ctorAttr = type->_accessAttribute(Consts::IDENTIFIER_fn_constructor,true);
		if(ctorAttr==NULL){
			type = type->getBaseType();
			continue;
		}else if(constructors.empty() && ctorAttr->isPrivate()){ // first constructor must not be private!
			setException("Can't instanciate Type with private _contructor."); //! \todo check this!
			return failureResult; // failure
		}else{
			ObjPtr fun = ctorAttr->getValue();
			const internalTypeId_t funType = fun->_getInternalTypeId();
			
			if(funType==_TypeIds::TYPE_USER_FUNCTION){
				constructors.push_back(fun);
				type = type->getBaseType();
				continue;
			}else if(_TypeIds::TYPE_FUNCTION){
				constructors.push_back(fun);
			}else{
				setException("Constructor has to be a UserFunction or a Function.");
				return failureResult; // failure
			}
			break;
		}
	}while(type.isNotNull());
	
	if(constructors.empty()) // failure
		return failureResult;
	
	{ // call the first constructor and pass the other constructor-functions by adding them to the stack
		ObjRef fun = constructors.front();
		
		executeFunctionResult_t result = startFunctionExecution(fun,type.get(),params);
		if(result.second){
			FunctionCallContext * fcc = result.second;
			for(std::vector<ObjPtr>::const_reverse_iterator it = constructors.rbegin();(it+1)!=constructors.rend();++it) //! \todo c++11 use std::next
				fcc->stack_pushObject( *it );
			fcc->enableAwaitingCaller();
			fcc->markAsConstructorCall();
			return std::make_pair(static_cast<Object*>(NULL),fcc);
		}else{
			createdObject = result.first;
			if(createdObject.isNull()){
				if(state!=STATE_EXCEPTION) // if an exception occured in the constructor, the result may be NULL
					setException("Constructor did not create an Object."); //! \todo improve message!
				return failureResult;
			}
			
			// init attribute
			createdObject->_initAttributes(*this); //! \todo catch exceptions!!!!!!!!
			return std::make_pair(createdObject.detachAndDecrease(),static_cast<FunctionCallContext*>(NULL));
		}
	}
	return failureResult;
}


//! (internal)
Object * Runtime::sysCall(uint32_t sysFnId,ParameterValues & params){
	Function * fn = NULL;
	if(sysFnId<systemFunctions.size()){
		fn = systemFunctions.at(sysFnId).get();
	}
	if(!fn){
		setException("Unknown systemCall."); // \todo improve message
		return NULL;
	}
	return fn->getFnPtr()(*this,NULL,params);
}

void Runtime::yieldNext(YieldIterator & yIt){
	_Ptr<FunctionCallContext> fcc = yIt.getFCC();
	if(fcc.isNull()){
		setException("Invalid YieldIterator");
		return;
	}
	ObjRef result = executeFunctionCallContext( fcc );
	// error occured? throw an exception!
	if(getState()==STATE_EXCEPTION){
		result = getResult();
		resetState();
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

}
