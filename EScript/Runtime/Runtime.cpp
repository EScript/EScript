// Runtime.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Runtime.h"
#include "RuntimeBlock.h"
#include "FunctionCallContext.h"
#include "../EScript.h"
#include "../Objects/AST/ConditionalExpr.h"
#include "../Objects/AST/GetAttribute.h"
#include "../Objects/AST/SetAttribute.h"
#include "../Objects/AST/Block.h"
#include "../Objects/AST/IfControl.h"
#include "../Objects/AST/FunctionCall.h"
#include "../Objects/AST/LogicOp.h"
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
		ObjRef resultRef=runtime.executeFunction(fun.get(),obj.get(),params);
		return resultRef.detachAndDecrease();
	})

	//! [ESF]  Object _getCurrentCaller()
	ESF_DECLARE(typeObject,"_getCurrentCaller",0,0, runtime.getCurrentContext()->getCaller() )
}

// ----------------------------------------------------------------------
// ---- Main

//! (ctor)
Runtime::Runtime() :
		ExtObject(Runtime::getTypeObject()), stackSizeLimit(512),
		state(STATE_NORMAL),logger(new LoggerGroup(Logger::LOG_WARNING)){

	logger->addLogger("coutLogger",new StdLogger(std::cout));

	globals = EScript::getSGlobals()->clone();
	declareConstant(globals.get(),"GLOBALS",globals.get());
	declareConstant(globals.get(),"SGLOBALS",EScript::getSGlobals());

	pushContext(RuntimeContext::create());

	//ctor
}

//! (dtor)
Runtime::~Runtime() {
	declareConstant(globals.get(), "GLOBALS",NULL);
	while(!contextStack.empty())
		popContext();

	//dtor
}

// ----------------------------------------------------------------------
// ---- Variables

Namespace * Runtime::getGlobals()const	{
	return globals.get();
}

Object * Runtime::readMemberAttribute(Object * obj,const StringId id){
	try{
		Attribute * attr = obj->_accessAttribute(id,false);
		if(attr==NULL){
			return NULL;
		}else if(attr->isPrivate() && obj!=getCurrentContext()->getCaller()) {
			warn("Cannot read private attribute.");
			return NULL;
		}
		return attr->getValue();
	}catch(Exception * e){
		ERef<Exception> eHolder(e);
		warn(eHolder->getMessage());
		return NULL;
	}
}


//!	@note Redesign due to [BUG20080324]
//! \note calls setCallingObject
Object * Runtime::getVariable(const StringId id) {
	RuntimeBlock * rtb = getCurrentContext()->getCurrentRTB();
	if(rtb) {
		Object * result=NULL;
		// search for local variable (bla)
		if((result=rtb->getLocalVariable(id)))
			return result;
		// search for member variable (this.bla)
		Object * caller = getCurrentContext()->getCaller();
		if (caller!=NULL){
			if((result=readMemberAttribute(caller,id))){
				setCallingObject(caller);
				return result;
			}
		}
	}
	// search for global var (GLOBALS.bla)
	return globals->getLocalAttribute(id).getValue();
}

Object * Runtime::getGlobalVariable(const StringId id) {
	// \note getLocalAttribute is used to skip the members of Type
	// 	which are otherwise found as false global variables  [BUG20100618]
	return globals->getLocalAttribute(id).getValue();
}

//! redesign because of BUG[20090424]
void Runtime::assignToVariable(const StringId id,Object * value) {
	// search for local variable (bla)
	RuntimeBlock * rtb = getCurrentContext()->getCurrentRTB();
	if (rtb && rtb->assignToVariable(*this,id,value)) {
		// assigned to local variable
		return;
	} else if(  assignToAttribute( globals.get(),id,value)) {
		// assigned to global variable
		return;
	}else{
		warn("Variable '"+id.toString()+"' is not defined, assuming local variable.");
		if(rtb)
			rtb->initLocalVariable(id,value);
	}
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
			if( obj!=getCurrentContext()->getCaller() ){
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

Object * Runtime::eval(const StringData & code){
	ERef<Block> block(new Block());
	static const StringId inline_id("[inline]");
	block->setFilename(inline_id);
	try{
		Parser p(getLogger());
		p.parse(block.get(),code);
	}catch(Exception * e){
		setException(e);
		return NULL;
	}
	pushContext(RuntimeContext::create());
	getCurrentContext()->createAndPushRTB(block.get());// this is later popped implicitly when the context is executed.

	ObjRef resultRef = executeCurrentContext(true);
	popContext();
	block = NULL; // remove possibly pending reference to the result to prevent accidental deletion
	return resultRef.detachAndDecrease();
}

/*! - identify object by internalTypeId (as defined in typeIds.h)
	- dispatch if object is an expression ( 0x20 >= id <0x30 )
	- return ref or copy otherwise. */
Object * Runtime::executeObj(Object * obj){
	int type=obj->_getInternalTypeId();
	if(type<0x020 || type>0x2f){
		return obj->getRefOrCopy();
	}
	Object * exp=obj;

	switch(type){
	case _TypeIds::TYPE_GET_ATTRIBUTE:{
		GetAttribute * ga=static_cast<GetAttribute *>(exp);
		ObjRef resultRef;
		// _.ident
		if (ga->getObjectExpression()==NULL) {
			setCallingObject(NULL);
			resultRef = getVariable(ga->getAttrId());
			if (resultRef.isNull())
				warn("Unknown variable '"+ga->toString()+"'.");

		}// obj.ident
		else {
			ObjRef obj2Ref=executeObj(ga->getObjectExpression());
			if(!assertNormalState(ga))
				return NULL;

			if(obj2Ref.isNull())
				obj2Ref = Void::get();

			setCallingObject(obj2Ref.get());
			resultRef = readMemberAttribute( obj2Ref.get(),ga->getAttrId() );
			if (resultRef.isNull()) {
				warn("Member not set '"+ga->toString()+"\'.");
			}
		}
		return resultRef.detachAndDecrease();
	}
	case _TypeIds::TYPE_FUNCTION_CALL:{
		return executeFunctionCall(static_cast<FunctionCall*>(exp));
	}

	case _TypeIds::TYPE_SET_ATTRIBUTE:{
		SetAttribute * sa=static_cast<SetAttribute *>(exp);
		ObjRef value;
		if (!sa->valueExpr.isNull()) {
			value=executeObj(sa->valueExpr.get());
			if(!assertNormalState(sa))
				return NULL;

			/// Bug[20070703] fixed:
			value = value.isNull() ? Void::get() : value->getRefOrCopy();
		}
		/// Local variable
		if (sa->objExpr.isNull()) {
			assignToVariable(sa->attrId,value.get());
			if(!assertNormalState(sa))
				return NULL;
			return value.detachAndDecrease();
		}
		/// obj.ident
		ObjRef obj2=executeObj(sa->objExpr.get());
		if(!assertNormalState(sa))
			return NULL;

		if(obj2.isNull())
			obj2=Void::get();
		if(sa->assign){
			bool success = true;
			// try to assign the value; this may produce an exception (\see Type::assignToTypeAttribute),
			// which is caught and emitted as warning as this is normally no more critical than trying to assign to a nonexistent attribute.
			try{
				success = assignToAttribute(obj2,sa->attrId,value);
				if(!assertNormalState(sa))
					return NULL;
			}catch(Exception * e){
				ERef<Exception> eHolder(e);
				warn(eHolder->getMessage());
			}
			if(!success){
				warn(std::string("Unknown attribute '")+sa->getAttrName()+"' ("+
						(sa->objExpr.isNull()?"":sa->objExpr->toDbgString())+'.'+sa->getAttrName()+'='+(value.isNull()?"":value->toDbgString())+')');
				if(!obj2->setAttribute(sa->attrId, Attribute(value.get()))){ // fallback: set obj attribute
					warn(std::string("Cannot set object attribute '")+sa->getAttrName()+"' ("+
							(sa->objExpr.isNull()?"":sa->objExpr->toDbgString())+'.'+sa->getAttrName()+'='+(value.isNull()?"":value->toDbgString())+')');
				}
			}
		}else {
			const Attribute::flag_t attrFlags = sa->getAttributeProperties();

			// check for @(override)
			if(attrFlags&Attribute::OVERRIDE_BIT && obj2->_accessAttribute(sa->attrId,false)==NULL){
				warn(std::string("No attribute to override: '")+sa->getAttrName()+"' ("+
						(sa->objExpr.isNull()?"":sa->objExpr->toDbgString())+'.'+sa->getAttrName()+'='+(value.isNull()?"":value->toDbgString())+")");
			}
			if(attrFlags&Attribute::TYPE_ATTR_BIT){
				Type * t=obj2.toType<Type>();
				if(t){
					t->setAttribute(sa->attrId,Attribute(value,attrFlags));
				}else{
					warn(std::string("Can not set typeAttr to non-Type-Object: '")+sa->getAttrName()+"' ("+
							(sa->objExpr.isNull()?"":sa->objExpr->toDbgString())+'.'+sa->getAttrName()+'='+(value.isNull()?"":value->toDbgString())+')'
							+"Setting objAttr instead.");
					if(!obj2->setAttribute(sa->attrId,Attribute(value,attrFlags & ~(Attribute::TYPE_ATTR_BIT)))){ // fallback: set obj attribute
						warn(std::string("Cannot set object attribute '")+sa->getAttrName()+"' ("+
								(sa->objExpr.isNull()?"":sa->objExpr->toDbgString())+'.'+sa->getAttrName()+'='+(value.isNull()?"":value->toDbgString())+')');
					}
				}
			}else { // obj attribute
				if(!obj2->setAttribute(sa->attrId,Attribute(value,attrFlags)))
					warn(std::string("Cannot set object attribute '")+sa->getAttrName()+"' ("+
							(sa->objExpr.isNull()?"":sa->objExpr->toDbgString())+'.'+sa->getAttrName()+'='+(value.isNull()?"":value->toDbgString())+')');

			}

		}
		return value.detachAndDecrease();
	}
	case _TypeIds::TYPE_LOGIC_OP:{
		LogicOp * lop=static_cast<LogicOp *>(exp);
		ObjRef resultRef( executeObj(lop->getLeft()) );
		if(!assertNormalState(lop))
			return NULL;

		bool b=resultRef.toBool();

		if (lop->getOperator() == LogicOp::NOT) {
			resultRef = Bool::create(!b);
			return resultRef.detachAndDecrease();
		} else if (b && lop->getOperator()==LogicOp::OR) {
			resultRef = Bool::create(true);
			return resultRef.detachAndDecrease();
		} else if (!b && lop->getOperator()==LogicOp::AND) {
			resultRef = Bool::create(false);
			return resultRef.detachAndDecrease();
		}
		resultRef = executeObj(lop->getRight());
		if(!assertNormalState(lop))
			return NULL;

		resultRef=Bool::create( resultRef.toBool() );
		return resultRef.detachAndDecrease();
	}
	case _TypeIds::TYPE_CONDITIONAL:{
		ConditionalExpr * cond=static_cast<ConditionalExpr *>(exp);
		if (cond->getCondition()!=NULL) {
			ObjRef conResult = executeObj(cond->getCondition());
			if(! assertNormalState(cond))
				return NULL;

			else if (conResult.toBool())
				return cond->getAction()==NULL ? NULL : executeObj(cond->getAction());
		}
		return cond->getElseAction()==NULL ? NULL : executeObj(cond->getElseAction());
	}
	case _TypeIds::TYPE_BLOCK:{
		return executeBlock(static_cast<Block*>(exp));
	}
	default:{
		break;
	}
	}
	return NULL;
}

//! (internal)
Object * Runtime::executeBlock(Block * block) {
	getCurrentContext()->createAndPushRTB(block);
	ObjRef resultRef = executeCurrentContext(true);
	getCurrentContext()->popRTB();
	return resultRef.detachAndDecrease();
}

//! (internal) used by executeBlock(...), executeCatchBlock(...) and executeContext(...)
Object * Runtime::executeCurrentContext(bool markEntry) {
	int localRTBs=0;

	RuntimeContext * ctxt=getCurrentContext();
	RuntimeBlock * rtb=ctxt->getCurrentRTB();

	const Statement * stmt = NULL;
	stmt = rtb->nextStatement();
	ObjRef resultRef;
	while( rtb!=NULL ){
		if(stmt==NULL){
			if( markEntry && (--localRTBs)<0 )
				break;
			ctxt->popRTB();
			rtb = ctxt->getCurrentRTB();
			if( rtb==NULL )
				break;
			stmt = rtb->nextStatement();
			continue;
		}
		resultRef = NULL;
		try {
			switch( stmt->getType()){
			case Statement::TYPE_EXPRESSION:{
				resultRef=executeObj( stmt->getExpression() );
				stmt = rtb->nextStatement();
				break;
			}
			case Statement::TYPE_BLOCK:{
				Block * block=static_cast<Block*>(stmt->getExpression());
				rtb = ctxt->createAndPushRTB(block);
				++localRTBs;
				stmt = rtb->nextStatement();
				break;
			}
			case Statement::TYPE_IF:{
				IfControl * ifControl = static_cast<IfControl*>(stmt->getExpression());
				resultRef = executeObj( ifControl->getCondition() );
				assertNormalState(ifControl);
				stmt = resultRef.toBool() ? &ifControl->getAction() : &ifControl->getElseAction();
				if(stmt==NULL || !stmt->isValid())
					stmt = rtb->nextStatement();
				break;
			}
			case Statement::TYPE_JUMP_TO_A:{
				rtb->gotoStatement(rtb->getStaticBlock()->getJumpPosA());
				stmt = rtb->nextStatement();
				continue;
			}
			case Statement::TYPE_BREAK:{
				setState(STATE_BREAKING);
				break;
			}
			case Statement::TYPE_CONTINUE:{
				setState(STATE_CONTINUING);
				break;
			}
			case Statement::TYPE_RETURN:{
				ObjRef returnValue;
				if( stmt->getExpression() != NULL )
					returnValue = executeObj(stmt->getExpression());
				if(!assertNormalState(stmt->getExpression()))
					break;
				setReturnState(returnValue);
				break;
			}
			case Statement::TYPE_YIELD:{
				ObjRef returnValue;
				if( stmt->getExpression() != NULL )
					returnValue = executeObj(stmt->getExpression());
				if(!assertNormalState(stmt->getExpression()))
					break;
				setYieldingState(returnValue);
//				stmt = rtb->nextStatement();
				break;
			}
			case Statement::TYPE_EXCEPTION:{
				ObjRef returnValue;
				if( stmt->getExpression() != NULL )
					returnValue = executeObj(stmt->getExpression());
				if(!assertNormalState(stmt->getExpression()))
					break;
				setExceptionState(returnValue);
				break;
			}
			case Statement::TYPE_EXIT:{
				ObjRef returnValue;
				if( stmt->getExpression() != NULL )
					returnValue = executeObj(stmt->getExpression());
				if(!assertNormalState(stmt->getExpression()))
					break;
				setExitState(returnValue);
				break;
			}
			case Statement::TYPE_UNDEFINED:
			default:{
				std::cout << " #unimplementedStmt "<<static_cast<int>(stmt->getType());
				resultRef=executeObj( stmt->getExpression() );
				stmt = rtb->nextStatement();
				break;
			}
			}
		}catch(Object * exceptionObj){
			setExceptionState(exceptionObj);
			resultRef=NULL;
		}
		if(!checkNormalState()){
			switch(getState()){
			case STATE_BREAKING:{
				while( rtb->getStaticBlock()->getBreakPos() == Block::POS_DONT_HANDLE ){
					if( markEntry && (--localRTBs)<0 ){
						setException("No break here!");
						return NULL;
					}
					ctxt->popRTB();
					rtb = ctxt->getCurrentRTB();
					if( rtb==NULL ){
						setException("No break here!");
						return NULL;
					}
				}
				rtb->gotoStatement(rtb->getStaticBlock()->getBreakPos());
				stmt = rtb->nextStatement();
				resetState();
				break;
			}
			case STATE_CONTINUING:{
				while( rtb->getStaticBlock()->getContinuePos() == Block::POS_DONT_HANDLE ){
					if( markEntry && (--localRTBs)<0 ){
						setException("No continue here!");
						return NULL;
					}
					ctxt->popRTB();
					rtb = ctxt->getCurrentRTB();

					if( rtb==NULL ){
						setException("No continue here!");
						return NULL;
					}
				}
				rtb->gotoStatement(rtb->getStaticBlock()->getContinuePos());
				stmt = rtb->nextStatement();
				resetState();
				break;
			}
			case STATE_YIELDING:{
				return NULL;
			}
			case STATE_EXCEPTION:{
				while( rtb->getStaticBlock()->getExceptionPos() == Block::POS_DONT_HANDLE ){
					if( markEntry && (--localRTBs)<0 )
						return NULL;
					ctxt->popRTB();
					rtb = ctxt->getCurrentRTB();
					if( rtb==NULL )
						return NULL;
				}
				rtb->gotoStatement(rtb->getStaticBlock()->getExceptionPos());
				stmt = rtb->nextStatement();
				state = STATE_NORMAL; //! \note the state value is not reset, as it may be needed in a catch block
				break;
			}
			case STATE_NORMAL:
			case STATE_RETURNING:
			case STATE_EXITING:
			default:{
				if(markEntry){
					while( localRTBs>0 ){
						ctxt->popRTB();
						--localRTBs;
					}
				}else{
					while(rtb!=NULL){
						ctxt->popRTB();
						rtb = ctxt->getCurrentRTB();
					}
				}
				return NULL;
			}
			}
		}

	}

	return resultRef.detachAndDecrease();
}

// ----------------------------------------------------------------------
// ---- Function execution

/*!
 - execute functionExpression and determine calling object
 - collect parameters by executing parameter expression
 - execute function by calling executeFunction(...)
*/
Object * Runtime::executeFunctionCall(FunctionCall * fCall){
	size_t numParams=0;
	setCallingObject(NULL);

	// get calling object
	ObjRef funRef=executeObj(fCall->getStatement()); // this sets the new calling object
	if(!assertNormalState(fCall))
		return NULL;

	ObjRef callingObjectRef=getCallingObject();

	// get parameter values
	numParams=fCall->getNumParams();
	ParameterValues paramResults(numParams);

	for ( size_t i=0;i<numParams;++i) {
		Object * paramExpression=fCall->getParamExpression(i);

		if(paramExpression!=NULL){
			ObjRef paramResultRef=executeObj(paramExpression);
			if( !assertNormalState(fCall)){	// error?
				for (size_t j=0; j<i;++j)  // remove already added references
					Object::removeReference(paramResults[j].get());
				return NULL;
			}
			Object * result=paramResultRef.isNull() ? Void::get() : paramResultRef->getRefOrCopy();
			paramResults.set(i,result);
			Object::addReference(result);
		}else{
			// A NULL in the parameter List is only allowed for UserFunction, so we have to check this here...
			Delegate * d=NULL;
			if(funRef.toType<UserFunction>() != NULL ||
					( (d=funRef.toType<Delegate>()) && (dynamic_cast<UserFunction*>(d->getFunction())!=NULL)) ){
				paramResults.set(i,NULL);
			}else{
				paramResults.set(i,Void::get());
				Object::addReference(paramResults[i].get());
			}
		}
	}

	// debug info
	functionCallStack.push_back(FunctionCallInfo(fCall,callingObjectRef.get(),funRef.get(),&paramResults));

	// execute function
	ObjRef resultRef;
	try{
		resultRef=executeFunction(funRef.get(),callingObjectRef.get(),paramResults,fCall->isConstructorCall());
	}catch (Object * o) {
		/// Add additional information to exception
		if(Exception * e=dynamic_cast<Exception *>(o)){
			std::ostringstream os;
			os<<e->getMessage()<<"\tCall:"<<fCall->toString()<<"Line:"<<fCall->getLine();
			e->setMessage(os.str());
		}
		for (size_t i=0; i<numParams;++i)
			Object::removeReference(paramResults[i].get());
		functionCallStack.pop_back();
		throw(o);
	}
	functionCallStack.pop_back();

	// cleanup // TODO STATE???!!!!!
	for (size_t i=0; i<numParams;++i)
		Object::removeReference(paramResults[i].get());
	funRef=NULL; /// As funRef can be == resultRef
	callingObjectRef=NULL; /// As callingObjectRef can be == resultRef

	return resultRef.detachAndDecrease();
}

/*! Dispatch according to type of fun:
	- Function: return result of function
	- UserFunction constructor:
		- return executeUserConstructor(...)
	- UserFunction:
		- createAndPushFunctionCallContext(...)
		- return executeContext(...)
	- Delegate: return executeFunction(...) for contained function.
*/
Object * Runtime::executeFunction(const ObjPtr & fun,const ObjPtr & _callingObject,const ParameterValues & params,bool isConstructorCall/*=false*/){
	if(fun.isNull()){
		warn("Cannot use '"+Void::get()->toDbgString()+"' as a function.");
		return NULL;
	}


	// possibly endless recursion?
	if(getStackSize() >= getStackSizeLimit()){
		setException("Stack size limit reached.");
		return NULL;
	}

	int type=fun->_getInternalTypeId();

	// is  C++ function ?
	if (type==_TypeIds::TYPE_FUNCTION) {
		Function * libfun=static_cast<Function*>(fun.get());

		if(isConstructorCall && _callingObject.toType<Type>()==NULL){
			setException("Can not instantiate non-Type-Object. Hint: Try to check the type you use with 'new'.");
			return NULL;
		}
		{	// check param count
			const int min = libfun->getMinParamCount();
			const int max = libfun->getMaxParamCount();
			if( (min>0 && static_cast<int>(params.count())<min)){
				std::ostringstream sprinter;
				sprinter<<"Too few parameters: Expected " <<min<<", got "<<params.count()<<'.';
				//! \todo improve message
				setException(sprinter.str());
				return NULL;
			} else  if (max>=0 && static_cast<int>(params.count())>max) {
				std::ostringstream sprinter;
				sprinter<<"Too many parameters: Expected " <<max<<", got "<<params.count()<<'.';
				//! \todo improve message
				warn(sprinter.str());
			}
		}
		libfun->increaseCallCounter();

		try {
			if(isConstructorCall){
				// store reference to the new object, so that it is automatically removed if the _initAttributes-call fails with an exception.
				ObjRef newObj = (*libfun->getFnPtr())(*this,_callingObject.get(),params);
				if(newObj.isNull()){
					if(state!=STATE_EXCEPTION){ // the constructor call itself did not set the exception state, but did not return an object.
						setException(std::string("Constructor did not return an object."));
					}
					return NULL;
				}
				// init attribute, etc...
				newObj->_initAttributes(*this);
				return newObj.detachAndDecrease();
			}else{
				return (*libfun->getFnPtr())(*this,_callingObject.get(),params);
			}
		} catch (Exception * e) {
			setExceptionState(e);
			return NULL;
		} catch(const char * message) {
			setException(std::string("C++ exception: ")+message);
			return NULL;
		} catch(const std::string & message) {
			setException(std::string("C++ exception: ") + message);
			return NULL;
		} catch(const std::exception & e) {
			setException(std::string("C++ exception: ") + e.what());
			return NULL;
		} catch (Object * obj) {
			// workaround: this should be covered by catching the Exception* directly, but that doesn't always seem to work!?!
			Exception * e=dynamic_cast<Exception *>(obj);
			if(e){
				setExceptionState(e);
				return NULL;
			}
			const std::string message=(obj?obj->toString():"NULL");
			setException(message);
			return NULL;
		}  catch (...){
			setException("C++ exception");
			return NULL;
		}
	} // is UserFunction?
	else if (type==_TypeIds::TYPE_USER_FUNCTION){
		if (isConstructorCall) {
			return executeUserConstructor(_callingObject,params); // this ufun is not used, as it's origin is not known
		} else { /// !isConstructorCall
			UserFunction * ufun=static_cast<UserFunction*>(fun.get());
			RuntimeContext * fctxt=createAndPushFunctionCallContext(_callingObject,ufun,params);

			// error occurred
			if(fctxt==NULL) {
				if( checkNormalState() ) // no context, but normal state? --> strange things happend
					setException("Could not call function. ");
				return NULL;
			}
			ObjRef result=executeCurrentContext();
			result=NULL;

			if(!checkNormalState()){
				if(getState()==Runtime::STATE_RETURNING){
					result=getResult();
					resetState();
				}else if(getState()==Runtime::STATE_YIELDING){
					YieldIterator * yctxt=new YieldIterator();
					yctxt->setResult(getResult());
					yctxt->setContext(fctxt);
					result=yctxt;
					resetState();
				}else if(!assertNormalState()){
					popContext();
					return NULL;
				}
			}
			popContext();
			return result.detachAndDecrease();
		}
	} // is Delegate?
	else if(type==_TypeIds::TYPE_DELEGATE){
		Delegate * d=static_cast<Delegate*>(fun.get());
		return executeFunction(d->getFunction(),d->getObject(),params,isConstructorCall);
	} else { // function-object has a user defined "_call"-member?
		ObjPtr otherFun = readMemberAttribute(fun.get(),Consts::IDENTIFIER_fn_call);
		if(otherFun.isNotNull()){
			// fun._call( callingObj , param0 , param1 , ... )
			ParameterValues params2(params.count()+1);
			params2.set(0,_callingObject.isNotNull() ? _callingObject : Void::get());
			std::copy(params.begin(),params.end(),params2.begin()+1);

			return executeFunction(otherFun,fun,params2,isConstructorCall);
		}

		warn("Cannot use '"+fun->toDbgString()+"' as a function.");
	}
	return NULL;
}

/*! (internal)	Create a RuntimeContext for a user function and assign the parameterValues to the corresponding local variables.
	\note The calling object is not set here.
	Called by executeFunction(...) and executeUserConstructor(...)	*/
RuntimeContext * Runtime::createAndPushFunctionCallContext(const ObjPtr & _callingObject,UserFunction * ufun,const ParameterValues & paramValues){

	RuntimeContext::RTBRef ctxt = RuntimeContext::create();
	RuntimeBlock * rtb=ctxt->createAndPushRTB(ufun->getBlock());// this is later popped implicitly when the context is executed.
	ctxt->initCaller(_callingObject);

	pushContext(ctxt.get());

	/// Assign parameter values
	UserFunction::parameterList_t * paramExpressions=ufun->getParamList();
	size_t paramExpSize=paramExpressions->size();
	size_t numberOfAssignedParameters=paramExpSize;

	// check if last parameter is a multiParam
	ERef<Array> multiParam;
	if( paramExpSize > 0 && (*paramExpressions)[paramExpSize-1]->isMultiParam() ){
		multiParam=Array::create();
		numberOfAssignedParameters--;
	}else  if (paramValues.count()>paramExpSize) {
		warn("Too many parameters given"); // Todo: more precise warning!
	}

	for (size_t i=0;i<numberOfAssignedParameters;++i) {
		ObjRef valueRef;
		StringId name=(*paramExpressions)[i]->getName();

		if(i<paramValues.count() && !paramValues[i].isNull() ){
			valueRef=paramValues.get(i);
		}/// get default value
		else{
			Object* defaultValueExpression=(*paramExpressions)[i]->getDefaultValueExpression();
			if(defaultValueExpression==NULL){
				warn("Too few parameters given, missing \""+(*paramExpressions)[i]->getName().toString()+"\"");
				valueRef = Void::get(); // init missing value with "void"
			}else{
				valueRef=executeObj(defaultValueExpression);
			}
		}

		/// check type
		Object * typeExpression=(*paramExpressions)[i]->getType();
		if(typeExpression!=NULL && !checkType( name,valueRef.get(),typeExpression)){
			popContext();
			return NULL;
		}
		rtb->initLocalVariable( name,valueRef.get());
	}

	// assign multiParam
	if(!multiParam.isNull()){
		StringId name=(*paramExpressions)[paramExpSize-1]->getName();

		for (size_t i=numberOfAssignedParameters;i<paramValues.count();++i) {
			ObjRef valueRef=paramValues.get(i);

			/// check type
			Object * typeExpression=(*paramExpressions)[paramExpSize-1]->getType();
			if(typeExpression!=NULL && !checkType(name,valueRef.get(),typeExpression)){
				popContext();
				return NULL;
			}
			multiParam->pushBack(valueRef.get());
		}
		rtb->initLocalVariable( name,multiParam.get());
	}

	/// set "thisFn" variable for recursive calls.
	rtb->initLocalVariable( Consts::IDENTIFIER_thisFn,ufun );

	return ctxt.detachAndDecrease();
}

/*! (internal) Called by createAndPushFunctionCallContext(...)*/
bool Runtime::checkType(const StringId & name, Object * obj,Object *typeExpression){
	if(obj==NULL)
		return false;
	ObjRef typeObj=executeObj(typeExpression);
	if(!assertNormalState())
		return false;

	if(Array * typeArray=typeObj.toType<Array>()){
		for(ERef<Iterator> it=typeArray->getIterator();!it->end();it->next()){
			ObjRef expectedType=it->value();
			// is of type or is identical
			if(obj->isA(expectedType.toType<Type>()) || obj->isIdentical(*this,expectedType)){
				return true;
			}
		}
	}else if(obj->isA(typeObj.toType<Type>()) ||  obj->isIdentical(*this,typeObj)){
		return true;
	}
	setException("Wrong value type for parameter '" + name.toString()+"'. "+
		"Expected object of type '"+typeExpression->toString()+"', but received object of type '"+obj->getTypeName()+"'.");
	return false;
}

/*! (internal) Called by executeFunction.
	 seach and stack all constructor method until c++ - constructor is found.
	 C ---|> B ---|> A
	 step along, create runtimeBlocks, calculate parameters, assign parameters
	 execute last c++ constructor to get Object
	 execute other functions for initialization using the given blocks.

*/
Object * Runtime::executeUserConstructor(const ObjPtr & _callingObject,const ParameterValues & params){
	Type * type=_callingObject.toType<Type>();
	if(type==NULL){
		setException("Can not instantiate non-Type-Object. Hint: Try to check the type you use with 'new'.");
		return NULL;
	}

	std::stack<RuntimeContext::RTBRef > consCallStack;
	ObjRef baseObj;
	ParameterValues currentParams = params;
	std::list<ObjRef> tmpRefHolderList;
	for(Type * t= type; t!=NULL ; t = t->getBaseType()){
		Object * currentCons = t->getLocalAttribute(Consts::IDENTIFIER_fn_constructor).getValue();
		// type has no local constructor function -> skip this level
		if(!currentCons)
			continue;

		const internalTypeId_t funType = currentCons->_getInternalTypeId();

		// c++ function found -> stop here
		if(	funType==_TypeIds::TYPE_FUNCTION ){
			Function * baseCons=static_cast<Function*>(currentCons);
			// create real object with baseCons( currentPrams)
			baseObj = executeFunction(baseCons,type,currentParams,true);
			if(baseObj.isNull()){
				// no object created?
				if(getState()!=STATE_EXCEPTION){ // ... altough no exception occured?
					setException("Base constructor did not return an object.");
					return NULL;
				}
				return NULL;
			}
			break;
		}else if(funType!=_TypeIds::TYPE_USER_FUNCTION ){
			setException("Constructor needs to be a function.");
			return NULL;
		}

		UserFunction * uCons = static_cast<UserFunction*>(currentCons);

		/// \note the created RTB must not have a parent:
		RuntimeContext::RTBRef fctxt=createAndPushFunctionCallContext(NULL,uCons,currentParams); // we don't know the baseObj yet.
		if(fctxt==NULL) {
			if( checkNormalState() ) // no context, but normal state? --> strange things happend
				setException("Could not call function. ");
			return NULL;
		}
		consCallStack.push(fctxt);

		/// create new set of params according to super constructor parameters
		std::vector<ObjRef> & sConstrExpressions=uCons->getSConstructorExpressions();
		if(!sConstrExpressions.empty()){
			ParameterValues superConstrParams(sConstrExpressions.size());
			size_t i=0;
			for(std::vector<ObjRef>::iterator it=sConstrExpressions.begin();it!=sConstrExpressions.end();++it){
				Object * expr=it->get();
				ObjRef result;
				if(expr!=NULL){
					result=executeObj(expr);
					/// constructor expressions must not change the state (exception, return, etc..)
					if(!assertNormalState()){
						popContext();
						return NULL;
					}
				}else{
					result=Void::get();
				}
				tmpRefHolderList.push_back(result.get()); /// hold a temporary reference until  all calls are made
				superConstrParams.set(i,result);
				++i;
			}
			currentParams.swap(superConstrParams);
		}else{
			currentParams.clear();
		}
		popContext(); // the RTB is not destroyed here because of the remaining reference (it is re-used later);
		// this is only allowed as it is not part of a RTB-hirarchie

	}
	if(baseObj.isNull()){
		setException( "No c++ base-contructor found.");
		return NULL;
	}

	// init
	while(!consCallStack.empty()){
		RuntimeContext * ctxt=consCallStack.top().get();
		ctxt->initCaller(baseObj);

		pushContext(ctxt);
		ObjRef tmp = executeCurrentContext();
		popContext();
		tmp=NULL;
		if(!checkNormalState()){
			if(getState()==Runtime::STATE_RETURNING){
				tmp=getResult();
				resetState();
			}else if(!assertNormalState()){
				return NULL;
			}
		}

		if ( (!tmp.isNull()) && (!dynamic_cast<Void *>(tmp.get()))) {
			warn("Contructor calls should not return anything!.");
			std::cout<< " #"<<tmp.toString()<<"\n";
		}
		consCallStack.pop();

	}
	return baseObj.detachAndDecrease();
}

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
	if(getCurrentContext()->getCurrentRTB()!=NULL){
		Block * b=getCurrentContext()->getCurrentRTB()->getStaticBlock();
		if(b!=NULL)
			os<<" ('"<<b->getFilename()<<"':~"<<getCurrentLine()<<")";
	}
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
	if(getCurrentContext()->getCurrentRTB()!=NULL){
		Block * b=getCurrentContext()->getCurrentRTB()->getStaticBlock();
		if(b)
			 return b->getFilename();
	}
	return std::string();
}

int Runtime::getCurrentLine()const{
	 int line = getCurrentContext()->getPrevLine();
	 if(line<0 && !functionCallStack.empty()){
		UserFunction * ufun=dynamic_cast<UserFunction *>(functionCallStack.back().function);
		if(ufun!=NULL)
			line = ufun->getBlock()->getLine();
	 }
	 return line;
}

std::string Runtime::getStackInfo(){
	std::ostringstream os;
	os<<"\n\n----------------------\nCall stack:";
	int nr=0;
	const int skipStart = functionCallStack.size()>50 ? 20 : functionCallStack.size()+1;
	const int skipEnd = functionCallStack.size()>50 ? functionCallStack.size()-20 : 0;
	for(std::vector<FunctionCallInfo>::reverse_iterator it=functionCallStack.rbegin();it!=functionCallStack.rend();++it){
		++nr;
		if(nr==skipStart)
			os<<"\n\n ... \n";
		if( nr>=skipStart && nr<skipEnd)
			continue;
		os<<"\n\n"<<nr<<'.';
		FunctionCallInfo & i=*it;
		if(i.funCall!=NULL)
			os<< "\t"<< i.funCall->toDbgString();
		if(i.callingObject!=NULL)
			os<< "\ncaller:\t"<<i.callingObject;
		if(i.function!=NULL)
			os<< "\nfun:\t"<<i.function->toDbgString();
		if(i.parameterValues!=NULL){
			os<< "\nparams:\t";
			for(ParameterValues::iterator pIt=i.parameterValues->begin();pIt!=i.parameterValues->end();++pIt){
				if(pIt!=i.parameterValues->begin())
					os<< ", ";
				if( (*pIt)!=NULL)
					os<<(*pIt)->toDbgString();
			}
		}
	}
	os<<"\n\n----------------------\n";
	return os.str();
}


// ------------------------------------------------------------------
Object * Runtime::executeUserFunction(EPtr<UserFunction> userFunction){
	
	
	_CountedRef<FunctionCallContext> fcc = FunctionCallContext::create(NULL,userFunction);
	InstructionBlock & instructions = fcc->getInstructions();

	while( true ){
		// end of function? continue with calling function 
		if(fcc->getInstructionCursor() >= instructions.getNumInstructions()){
			if(fcc->getParent()){
				fcc = fcc->getParent();
				instructions = fcc->getInstructions();
				fcc->stack_pushVoid();
				continue;
				// push result !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			}else{
				break;
			}
		}
	
		//! \todo this could probably improved by using iterators internally!
		const Instruction & instruction = instructions.getInstruction(fcc->getInstructionCursor());
		fcc->increaseInstructionCursor();

		std::cout << fcc->stack_toDbgString()<<"\n";
		std::cout << instruction.toString(instructions)<<"\n";
		
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
		case Instruction::I_CALL:{ //! \todo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
				paramRefHolder.push_back(paramValue);
				params.set(i,paramValue);
			}
			
			ObjRef fun = fcc->stack_popObject();
			ObjRef caller = fcc->stack_popObject();

			ObjPtr result = executeFunction(fun,caller,params); //! \todo OLD!
			fcc->stack_pushObject(result.get());
			break;
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
		case Instruction::I_JMP:{
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
		case Instruction::I_UNDEFINED:
		case Instruction::I_SET_MARKER:
		default:{
			warn("Unknown Instruction");
		}
		}
		
		//! \todo Check state
	}


	return Void::get();
}







}
