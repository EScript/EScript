// Runtime.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Runtime.h"
#include "../EScript.h"
#include "../Objects/Internals/ConditionalExpr.h"
#include "../Objects/Internals/GetAttribute.h"
#include "../Objects/Internals/SetAttribute.h"
#include "../Objects/Internals/Block.h"
#include "../Objects/Internals/IfControl.h"
#include "../Objects/Internals/FunctionCall.h"
#include "../Objects/Internals/LogicOp.h"
#include "../Objects/Internals/Statement.h"
#include "RuntimeBlock.h"
#include "../Objects/Void.h"
#include "../Objects/Exception.h"
#include "../Objects/Function.h"
#include "../Objects/UserFunction.h"
#include "../Objects/Delegate.h"
#include "../Objects/YieldIterator.h"
#include <sstream>
#include <stack>
using namespace EScript;


// ----------------------------------------------------------------------
// ---- Initialization

Type* Runtime::typeObject=NULL;

/**
 * initMembers
 */
void Runtime::init(EScript::Namespace & globals) {
	// [Runtime] ---|> [ExtObject]
	typeObject=new Type(ExtObject::getTypeObject());
	declareConstant(&globals,getClassName(),typeObject);

	typeObject->setTypeAttribute("IGNORE_WARNINGS",Number::create(Runtime::ES_IGNORE_WARNINGS));
	typeObject->setTypeAttribute("TREAT_WARNINGS_AS_ERRORS",Number::create(Runtime::ES_TREAT_WARNINGS_AS_ERRORS));


	//!	[ESMF] Number Runtime._getErrorConfig();
	ESF_DECLARE(typeObject,"_getErrorConfig",0,0, Number::create(runtime.getErrorConfig()))

	//!	[ESMF] Number Runtime._getStackSize();
	ESF_DECLARE(typeObject,"_getStackSize",0,0, Number::create(runtime.getStackSize()))

	//!	[ESMF] Number Runtime._getStackSizeLimit();
	ESF_DECLARE(typeObject,"_getStackSizeLimit",0,0, Number::create(runtime.getStackSizeLimit()))

	//!	[ESMF] void Runtime._setErrorConfig(number);
	ESF_DECLARE(typeObject,"_setErrorConfig",1,1,
				(runtime.setErrorConfig(static_cast<unsigned int>(parameter[0].toInt())),Void::get()) )

	//!	[ESMF] void Runtime._setStackSizeLimit(number);
	ESF_DECLARE(typeObject,"_setStackSizeLimit",1,1,
				(runtime.setStackSizeLimit(static_cast<size_t>(parameter[0].toInt())),Void::get()) )

	//!	[ESMF] void Runtime.exception( [message] );
	ESF_DECLARE(typeObject,"exception",0,1, (runtime.exception(parameter[0].toString()),Void::get()))

	//!	[ESMF] String Runtime.getStackInfo();
	ESF_DECLARE(typeObject,"getStackInfo",0,0, String::create(runtime.getStackInfo()))

	//!	[ESMF] void Runtime.warn([message]);
	ESF_DECLARE(typeObject,"warn",0,1, (runtime.warn(parameter[0].toString()),Void::get()))

}

// ----------------------------------------------------------------------
// ---- Main

//! (ctor)
Runtime::Runtime() :
		ExtObject(Runtime::typeObject), stackSizeLimit(512),
		state(STATE_NORMAL),errorConfig(0){ //,currentLine(0) {

	globals=EScript::getSGlobals()->clone();
	globals->setObjAttribute(stringToIdentifierId( "GLOBALS"),globals.get());
	globals->setObjAttribute(stringToIdentifierId( "SGLOBALS"),EScript::getSGlobals());

	pushContext(RuntimeContext::create());

	//ctor
}

//! (dtor)
Runtime::~Runtime() {
	globals->setObjAttribute(stringToIdentifierId( "GLOBALS"),NULL);
	while(!contextStack.empty())
		popContext();

	//dtor
}

// ----------------------------------------------------------------------
// ---- Variables

Namespace * Runtime::getGlobals()const	{
	return globals.get();
}

//!	@note Redesign due to [BUG20080324]
//! \note calls setCallingObject
Object * Runtime::getVariable(const identifierId id) {
	RuntimeBlock * rtb = getCurrentContext()->getCurrentRTB();
	if(rtb) {
		Object * result=NULL;
		// search for local variable (bla)
		if((result=rtb->getLocalVariable(id)))
			return result;
		// search for member variable (this.bla)
		Object * caller = getCurrentContext()->getCaller();
		if (caller!=NULL){
			if((result=caller->getAttribute(id))){
				setCallingObject(caller);
				return result;
			}
		}
	}
	// search for global var (GLOBALS.bla)
	return globals->getObjAttribute(id);
}

Object * Runtime::getGlobalVariable(const identifierId id) {
	// \note getObjAttribute is used to skip the members of Type
	// 	which are otherwise found as false global variables  [BUG20100618]
	return globals->getObjAttribute(id);
}

//! redesign because of BUG[20090424]
void Runtime::assignToVariable(const identifierId id,Object * value) {
	// search for local variable (bla)
	RuntimeBlock * rtb = getCurrentContext()->getCurrentRTB();
	if (rtb && rtb->assignToVariable(id,value)) {
		// assigned to local variable
		return;
	} else if(  globals->assignAttribute(id,value)) {
		// assigned to global variable
		return;
	}else{
		warn("Variable '"+identifierIdToString(id)+"' is not defined, assuming local variable.");
		if(rtb)
			rtb->initLocalVariable(id,value);
	}
}

// ----------------------------------------------------------------------
// ---- General execution

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
				warn("Unknown Variable:"+ga->toString());

		}// obj.ident
		else {
			ObjRef obj2Ref=executeObj(ga->getObjectExpression());
			if(!assertNormalState(ga))
				return NULL;

			if(obj2Ref.isNull())
				obj2Ref = Void::get();

			setCallingObject(obj2Ref.get());
			resultRef = obj2Ref->getAttribute(ga->getAttrId());
			if (resultRef.isNull()) {
				warn("Member not set:"+ga->toString());
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
			return value.detachAndDecrease();
		}
		/// obj.ident
		ObjRef obj2=executeObj(sa->objExpr.get());
		if(!assertNormalState(sa))
			return NULL;

		if(obj2.isNull())
			obj2=Void::get();
		if(sa->assignType == SetAttribute::ASSIGN){
			if(!obj2->assignAttribute(sa->attrId,value.get())){
				warn(std::string("Unkown attribute \"")+sa->getAttrName()+"\" ("+
						(sa->objExpr.isNull()?"":sa->objExpr->toDbgString())+"."+sa->getAttrName()+"="+(value.isNull()?"":value->toDbgString())+")");
				if(!obj2->setObjAttribute(sa->attrId,value.get())){
					warn(std::string("Can't set object attribute \"")+sa->getAttrName()+"\" ("+
							(sa->objExpr.isNull()?"":sa->objExpr->toDbgString())+"."+sa->getAttrName()+"="+(value.isNull()?"":value->toDbgString())+")");
				}
			}
		}else if(sa->assignType == SetAttribute::SET_OBJ_ATTRIBUTE){
			if(!obj2->setObjAttribute(sa->attrId,value.get()))
				warn(std::string("Can't set object attribute \"")+sa->getAttrName()+"\" ("+
						(sa->objExpr.isNull()?"":sa->objExpr->toDbgString())+"."+sa->getAttrName()+"="+(value.isNull()?"":value->toDbgString())+")");
		}else if(sa->assignType == SetAttribute::SET_TYPE_ATTRIBUTE){
			Type * t=obj2.toType<Type>();
			if(t){
				t->setTypeAttribute(sa->attrId,value.get());
			}else{
				warn(std::string("Can not set typeAttr to non-Type-Object: \"")+sa->getAttrName()+"\" ("+
						(sa->objExpr.isNull()?"":sa->objExpr->toDbgString())+"."+sa->getAttrName()+"="+(value.isNull()?"":value->toDbgString())+")"
						+"Setting objAttr instead.");
				if(!obj2->setObjAttribute(sa->attrId,value.get())){
					warn(std::string("Can't set object attribute \"")+sa->getAttrName()+"\" ("+
							(sa->objExpr.isNull()?"":sa->objExpr->toDbgString())+"."+sa->getAttrName()+"="+(value.isNull()?"":value->toDbgString())+")");
				}
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
			default:{
				std::cout << " #unimplementedStmt "<<stmt->getType();
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
					if( markEntry && (--localRTBs)<0 )
						return NULL;
					ctxt->popRTB();
					rtb = ctxt->getCurrentRTB();
					if( rtb==NULL )// warning?
						return NULL;
				}
				rtb->gotoStatement(rtb->getStaticBlock()->getBreakPos());
				stmt = rtb->nextStatement();
				resetState();
				break;
			}
			case STATE_CONTINUING:{
				while( rtb->getStaticBlock()->getContinuePos() == Block::POS_DONT_HANDLE ){
					if( markEntry && (--localRTBs)<0 )
						return NULL;
					ctxt->popRTB();
					rtb = ctxt->getCurrentRTB();
					if( rtb==NULL )// warning?
						return NULL;
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
				state = STATE_NORMAL; //! \note the state value is not resetted, as it may be needed in a catch block
				break;
			}
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
	- Funtion: return result of function
	- UserFunction constructor:
	    - return executeUserConstructor(...)
	- UserFunction:
		- createAndPushFunctionCallContext(...)
		- return executeContext(...)
	- Delegate: return executeFunction(...) for contained function.
*/
Object * Runtime::executeFunction(const ObjPtr & fun,const ObjPtr & _callingObject,const ParameterValues & params,bool isConstructorCall/*=false*/){
	if(fun.isNull())
		return NULL;

	// possibly endless recursion?
	if(getStackSize() >= getStackSizeLimit()){
		exception("Stack size limit reached.");
		return NULL;
	}

	// is  C++ function ?
	int type=fun->_getInternalTypeId();

	if (type==_TypeIds::TYPE_FUNCTION) {
		Function * libfun=static_cast<Function*>(fun.get());

		if(isConstructorCall && _callingObject.toType<Type>()==NULL){
			exception("Can not instantiate non-Type-Object. Hint: Try to check the type you use with 'new'.");
			return NULL;
		}
		try {
			return (*libfun->getFnPtr())(*this,_callingObject.get(),params);
		} catch (Exception * e) {
			setExceptionState(e);
			return NULL;
		} catch(const char * message) {
			exception(std::string("C++ exception: ")+message);
			return NULL;
		} catch (Object * obj) {
			// workaround: this should be covered by catching the Exception* directly, but that doesn't always seem to work!?!
			Exception * e=dynamic_cast<Exception *>(obj);
			if(e){
				setExceptionState(e);
				return NULL;
			}
			std::string message=(obj?obj->toString():"NULL");
			exception(message);
			return NULL;
		}  catch (...){
			exception("C++ exception");
			return NULL;
		}
	} // is UserFunction?
	else if (type==_TypeIds::TYPE_USER_FUNCTION){
		if (isConstructorCall) {
			return executeUserConstructor(_callingObject,params); // this ufun is not used, as it's origin is not known
		} else { /// !isConstructorCall
			UserFunction * ufun=static_cast<UserFunction*>(fun.get());
			RuntimeContext * fctxt=createAndPushFunctionCallContext(_callingObject,ufun,params);
			
			// error occured
			if(fctxt==NULL) {
				if( checkNormalState() ) // no context, but normal state? --> strange things happend
					exception("Could not call function. ");
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
	} else {
		warn("No function to call.");
	}
	return NULL;//resultRef.detachAndDecrease();
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
		identifierId name=(*paramExpressions)[i]->getName();

		if(i<paramValues.count() && !paramValues[i].isNull() ){
			valueRef=paramValues.get(i);
		}/// get default value
		else{
			Object* defaultValueExpression=(*paramExpressions)[i]->getDefaultValueExpression();
			if(defaultValueExpression==NULL){
				warn("Too few parameters given, missing \""+EScript::identifierIdToString((*paramExpressions)[i]->getName())+"\"");
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
		identifierId name=(*paramExpressions)[paramExpSize-1]->getName();

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
bool Runtime::checkType(const identifierId & name, Object * obj,Object *typeExpression){
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
	exception("Wrong value type for parameter '" + EScript::identifierIdToString(name)+"'. "+
		"Expected object of type '"+typeExpression->toString()+"', but received object of type '"+obj->getTypeName()+"'.");
	return false;
}

/*! (internal) Called by executeFunction.
	 seach and stack all constructor method until c++ - constructor is found.
	 C ---|> B ---|> A
	 step along, create runtimeBlocks, calculate parametres, assign parameters
	 execute last c++ constructor to get Object
	 execute other funcitons for initialization using the given blocks.

*/
Object * Runtime::executeUserConstructor(const ObjPtr & _callingObject,const ParameterValues & params){
	Type * type=_callingObject.toType<Type>();
	if(type==NULL){
		exception("Can not instantiate non-Type-Object. Hint: Try to check the type you use with 'new'.");
		return NULL;
	}

	std::stack<RuntimeContext::RTBRef > consCallStack;
	ObjRef baseObj;
	ParameterValues currentParams = params;
	std::list<ObjRef> tmpRefHolderList;
	for(Type * t= type; t!=NULL ; t = t->getBaseType()){
		Object * currentCons = t->getLocalAttribute(Consts::IDENTIFIER_fn_constructor);
		// type has no local constructor function -> skip this level
		if(!currentCons)
			continue;

		internalTypeId_t funType = currentCons->_getInternalTypeId();

		// c++ function found -> stop here
		if(	funType==_TypeIds::TYPE_FUNCTION ){
			Function * baseCons=static_cast<Function*>(currentCons);
			// create real object with baseCons( currentPrams)
			baseObj = executeFunction(baseCons,type,currentParams,true);
			break;
		}else if(funType!=_TypeIds::TYPE_USER_FUNCTION ){
			error("Constructor needs to be a function");
			return NULL;
		}

		UserFunction * uCons = static_cast<UserFunction*>(currentCons);

		/// \note the created RTB must not have a parent:
		RuntimeContext::RTBRef fctxt=createAndPushFunctionCallContext(NULL,uCons,currentParams); // we don't know the baseObj yet.
		if(fctxt==NULL) {
			if( checkNormalState() ) // no context, but normal state? --> strange things happend
				exception("Could not call function. ");
			return NULL;
		}
		consCallStack.push(fctxt);

		/// create new set of params according to super constructor parameters
		std::vector<ObjRef> & sConstrExpressions=uCons->getSConstructorExpressions();
		if(sConstrExpressions.size()>0){
			currentParams=ParameterValues(sConstrExpressions.size());
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
				currentParams.set(i,result);
				++i;
			}
		}else{
			currentParams=ParameterValues();
		}
		popContext(); // the RTB is not destroyed here because of the remaining reference (it is re-used later);
		// this is only allowed as it is not part of a RTB-hirarchie

	}
	if(baseObj.isNull()){
		error( "No c++ base-contructor found.");
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
			exception("No return here!"+(obj?" ["+obj->toString()+"]":""));
			break;
		}
		case STATE_BREAKING:{
			exception("No break here!"+(obj?" ["+obj->toString()+"]":""));
			break;
		}
		case STATE_CONTINUING:{
			exception("No continue here!"+(obj?" ["+obj->toString()+"]":""));
			break;
		}
		case STATE_YIELDING:{
			exception("No yield here!"+(obj?" ["+obj->toString()+"]":""));
			break;
		}
		case STATE_EXITING:{
			break;
		}
		case STATE_EXCEPTION:{
			// we are already in an exception state...
			break;
		}
	}
	return false;
}


void Runtime::info(const std::string & s) {
	std::cout << "\n Note: "<<s;
}


void Runtime::warn(const std::string & s) {
	if(getErrorConfig()&ES_IGNORE_WARNINGS) return;
	else if(getErrorConfig()&ES_TREAT_WARNINGS_AS_ERRORS) {
		exception(s);
		return;
	}
	std::cout << "\n WARNING: "<< s << std::endl;
	if(getCurrentContext()->getCurrentRTB()!=NULL){
		Block * b=getCurrentContext()->getCurrentRTB()->getStaticBlock();
		if(b)
			std::cout<<"\tFile:"<<b->getFilename()<<" near line "<<getCurrentLine()<<"\n";
//        std::cout << " *** "<<b->getFilename()<<":"<<b->toString();
	}
}

void Runtime::exception(const std::string & s) {
	Exception * e = new Exception(s,getCurrentLine());
	e->setStackInfo(getStackInfo());
	e->setFilename(getCurrentFile());
	setExceptionState(e);
}

void Runtime::error(const std::string & s,Object * obj) {
	std::ostringstream os;
	os<<s;
	if(obj) os<<"("<<obj->toString()<<")";
	if(getCurrentContext()->getCurrentRTB()!=NULL){
		Block * b=getCurrentContext()->getCurrentRTB()->getStaticBlock();
		if(b)
			 os<<"\tFile:"<<b->getFilename()<<" near line "<<getCurrentLine()<<"\n";
	}
	os<<getStackInfo();
	Exception * e = new Exception(os.str(),getCurrentLine()); // \todo remove line 
	e->setFilename(getCurrentFile());
	throw e;
}


// ----------------------------------------------------------------------
// ---- Debugging

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
		os<<"\n\n"<<nr<<".";
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
