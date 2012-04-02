// Runtime.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Runtime.h"
#include "RuntimeBlock.h"
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
		ObjRef resultRef=runtime.executeFunction(fun.get(),obj.get(),params);
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
	ESF_DECLARE(typeObject,"_getCurrentCaller",0,0, runtime.getCurrentContext()->getCaller() )
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

	pushContext(RuntimeContext::create());

////		std::vector<ERef<Function> > systemFunctions;


	//! init system calls \note the order of the functions MUST correspond to their funcitonId as defined in Consts.h
	{	// SYS_CALL_CREATE_ARRAY = 0;
		struct _{
			ESF( sysCall,0,-1,Array::create(parameter) )
		};
		systemFunctions.push_back(new Function(_::sysCall));
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
		systemFunctions.push_back(new Function(_::sysCall));
	}
	{	// SYS_CALL_THROW_TYPE_EXCEPTION = 2;
		struct _{
			// SYS_CALL_THROW_TYPE_EXCEPTION( expectedType, receivedValue )
			ESF( sysCall,2,2,(
					runtime.setException("Wrong parameter type: Expected '"+parameter[0].toString()+"' but got '"+parameter[1].toString()+"'" ),static_cast<Object*>(NULL)))
		};
		systemFunctions.push_back(new Function(_::sysCall));
	}
	{	// SYS_CALL_THROW = 3;
		struct _{
			// SYS_CALL_THROW( [value] )
			ESF( sysCall,0,1,(runtime.setExceptionState( parameter.count()>0 ? parameter[0] : Void::get() ),static_cast<Object*>(NULL)))
		};
		systemFunctions.push_back(new Function(_::sysCall));
	}	
	{	// SYS_CALL_EXIT = 4;
		struct _{
			// SYS_CALL_EXIT( [value] )
			ESF( sysCall,0,1,(runtime.setExitState( parameter.count()>0 ? parameter[0] : Void::get() ),static_cast<Object*>(NULL)))
		};
		systemFunctions.push_back(new Function(_::sysCall));
	}
	{	// SYS_CALL_GET_ITERATOR = 5;
		struct _{
			ES_FUNCTION( sysCall) {
				assertParamCount(runtime,1,1);
				Object * it=NULL;
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
				return it;
			}
		};
		systemFunctions.push_back(new Function(_::sysCall));
	}

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
	ERef<AST::BlockStatement> block(new AST::BlockStatement());
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
	using namespace AST;

	int type=obj->_getInternalTypeId();
	if(type<0x020 || type>0x2f){
		return obj->getRefOrCopy();
	}
	Object * exp=obj;

	switch(type){
	case _TypeIds::TYPE_GET_ATTRIBUTE_EXPRESSION:{
		AST::GetAttributeExpr * ga=static_cast<AST::GetAttributeExpr *>(exp);
		ObjRef resultRef;
		// _.ident
		if (ga->getObjectExpression()==NULL) {
			setCallingObject(NULL);
			resultRef = getVariable(ga->getAttrId());
			if (resultRef.isNull())
				warn("Unknown variable '"+ga->toString()+"'.");

		}// obj.ident
		else {
			ObjRef obj2Ref=executeObj(ga->getObjectExpression().get());
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
	case _TypeIds::TYPE_FUNCTION_CALL_EXPRESSION:{
		return executeFunctionCall(static_cast<AST::FunctionCallExpr*>(exp));
	}

	case _TypeIds::TYPE_SET_ATTRIBUTE_EXPRESSION:{
		AST::SetAttributeExpr * sa=static_cast<AST::SetAttributeExpr *>(exp);
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
	case _TypeIds::TYPE_LOGIC_OP_EXPRESSION:{
		LogicOpExpr * lop=static_cast<LogicOpExpr *>(exp);
		ObjRef resultRef( executeObj(lop->getLeft().get()) );
		if(!assertNormalState(lop))
			return NULL;

		bool b=resultRef.toBool();

		if (lop->getOperator() == LogicOpExpr::NOT) {
			resultRef = Bool::create(!b);
			return resultRef.detachAndDecrease();
		} else if (b && lop->getOperator()==LogicOpExpr::OR) {
			resultRef = Bool::create(true);
			return resultRef.detachAndDecrease();
		} else if (!b && lop->getOperator()==LogicOpExpr::AND) {
			resultRef = Bool::create(false);
			return resultRef.detachAndDecrease();
		}
		resultRef = executeObj(lop->getRight().get());
		if(!assertNormalState(lop))
			return NULL;

		resultRef=Bool::create( resultRef.toBool() );
		return resultRef.detachAndDecrease();
	}
	case _TypeIds::TYPE_CONDITIONAL_EXPRESSION:{
		ConditionalExpr * cond=static_cast<ConditionalExpr *>(exp);
		if (cond->getCondition()!=NULL) {
			ObjRef conResult = executeObj(cond->getCondition().get());
			if(! assertNormalState(cond))
				return NULL;

			else if (conResult.toBool())
				return cond->getAction().isNull() ? NULL : executeObj(cond->getAction().get());
		}
		return cond->getElseAction().isNull() ? NULL : executeObj(cond->getElseAction().get());
	}
	case _TypeIds::TYPE_BLOCK_STATEMENT:{
		return executeBlock(static_cast<BlockStatement*>(exp));
	}
	default:{
		break;
	}
	}
	return NULL;
}

//! (internal)
Object * Runtime::executeBlock(AST::BlockStatement * block) {
	getCurrentContext()->createAndPushRTB(block);
	ObjRef resultRef = executeCurrentContext(true);
	getCurrentContext()->popRTB();
	return resultRef.detachAndDecrease();
}

//! (internal) used by executeBlock(...), executeCatchBlock(...) and executeContext(...)
Object * Runtime::executeCurrentContext(bool markEntry) {
	using namespace AST;
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
				resultRef=executeObj( stmt->getExpression().get() );
				stmt = rtb->nextStatement();
				break;
			}
			case Statement::TYPE_BLOCK:{
				BlockStatement * block=static_cast<BlockStatement*>(stmt->getExpression().get());
				rtb = ctxt->createAndPushRTB(block);
				++localRTBs;
				stmt = rtb->nextStatement();
				break;
			}
			case Statement::TYPE_IF:{
				IfStatement * ifControl = static_cast<IfStatement*>(stmt->getExpression().get());
				resultRef = executeObj( ifControl->getCondition().get() );
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
					returnValue = executeObj(stmt->getExpression().get());
				if(!assertNormalState(stmt->getExpression().get()))
					break;
				setReturnState(returnValue);
				break;
			}
			case Statement::TYPE_YIELD:{
				ObjRef returnValue;
				if( stmt->getExpression() != NULL )
					returnValue = executeObj(stmt->getExpression().get());
				if(!assertNormalState(stmt->getExpression().get()))
					break;
				setYieldingState(returnValue);
//				stmt = rtb->nextStatement();
				break;
			}
			case Statement::TYPE_THROW:{
				ObjRef returnValue;
				if( stmt->getExpression() != NULL )
					returnValue = executeObj(stmt->getExpression().get());
				if(!assertNormalState(stmt->getExpression().get()))
					break;
				setExceptionState(returnValue);
				break;
			}
			case Statement::TYPE_EXIT:{
				ObjRef returnValue;
				if( stmt->getExpression() != NULL )
					returnValue = executeObj(stmt->getExpression().get());
				if(!assertNormalState(stmt->getExpression().get()))
					break;
				setExitState(returnValue);
				break;
			}
			case Statement::TYPE_UNDEFINED:
			default:{
				std::cout << " #unimplementedStmt "<<static_cast<int>(stmt->getType());
				resultRef=executeObj( stmt->getExpression().get() );
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
				while( rtb->getStaticBlock()->getBreakPos() == BlockStatement::POS_DONT_HANDLE ){
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
				while( rtb->getStaticBlock()->getContinuePos() == BlockStatement::POS_DONT_HANDLE ){
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
				while( rtb->getStaticBlock()->getExceptionPos() == BlockStatement::POS_DONT_HANDLE ){
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
Object * Runtime::executeFunctionCall(AST::FunctionCallExpr * fCall){
	size_t numParams=0;
	setCallingObject(NULL);

	// get calling object
	ObjRef funRef=executeObj(fCall->getGetFunctionExpression().get()); // this sets the new calling object
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
					yctxt->setValue(getResult());
//					yctxt->setContext(fctxt);
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
	if(getCurrentContext()->getCurrentRTB()!=NULL){
		AST::BlockStatement * b=getCurrentContext()->getCurrentRTB()->getStaticBlock();
		if(b)
			 return b->getFilename();
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
					push TypeOrObj
					push false
			*/
			ObjRef localVariable = fcc->getLocalVariable(instruction.getValue_uint32());
			ObjRef typeOrObj = fcc->stack_popObject();
			if(localVariable->isA(typeOrObj.toType<Type>()) || localVariable->isIdentical(*this,typeOrObj)){
				fcc->stack_pushBool(true);
			}else{
				fcc->stack_pushObject(typeOrObj);
				fcc->stack_pushBool(false);
			}
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
