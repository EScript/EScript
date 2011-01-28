// Runtime.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Runtime.h"
#include "../EScript.h"
#include "../Statements/Block.h"
#include "../Statements/FunctionCall.h"
#include "RuntimeBlock.h"
#include "../Objects/Void.h"
#include "../Objects/Exception.h"
#include "../Objects/Function.h"
#include "../Objects/UserFunction.h"
#include "../Objects/Delegate.h"
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

	//!	[ESMF] void Runtime.esf_Runtime_setErrorConfig(number);
	ESF_DECLARE(typeObject,"_setErrorConfig",1,1,
				(runtime.setErrorConfig(static_cast<unsigned int>(parameter[0].toInt())),Void::get()) )

	//!	[ESMF] number Runtime.esf_Runtime_getErrorConfig();
	ESF_DECLARE(typeObject,"_getErrorConfig",0,0,
				Number::create(runtime.getErrorConfig()))

	//!	[ESMF] String Runtime.getStackInfo();
	ESF_DECLARE(typeObject,"getStackInfo",0,0,
				String::create(runtime.getStackInfo()))
}

// ----------------------------------------------------------------------
// ---- Main

//! (ctor)
Runtime::Runtime():ExtObject(Runtime::typeObject), state(STATE_NORMAL),errorConfig(0),currentLine(0) {
	runtimeBlockStack.push(0);

	globals=EScript::getSGlobals()->clone();
	globals->setObjAttribute(stringToIdentifierId( "GLOBALS"),globals.get());
	globals->setObjAttribute(stringToIdentifierId( "SGLOBALS"),EScript::getSGlobals());
	//ctor
}

//! (dtor)
Runtime::~Runtime() {
	globals->setObjAttribute(stringToIdentifierId( "GLOBALS"),NULL);
	//dtor
}

// ----------------------------------------------------------------------
// ---- Variables

Namespace * Runtime::getGlobals()const	{
	return globals.get();
}

//!	@note Redesign due to [BUG20080324]
Object * Runtime::getVariable(const identifierId id) {
	RuntimeBlock * rtb = getCurrentRTB();
	if(rtb) {
		Object * result=NULL;

	// search for local variable (bla)
		if((result=rtb->getLocalVariable(id)))
			return result;

	// search for member variable (this.bla)
		if (rtb->getCaller()){
			if((result=rtb->getCaller()->getAttribute(id))){
				setCallingObject(rtb->getCaller());
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
	RuntimeBlock * rtb = getCurrentRTB();
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


Object * Runtime::executeObj(Object * obj){
	return obj->execute(*this);
}

Object * Runtime::executeBlock(Block * block) {
	RuntimeBlock * rtb=RuntimeBlock::create(block,getCurrentRTB());
	setCurrentLine(block->getLine());
	pushRTB(rtb);
	ObjRef resultRef=executeRTB(rtb);
	popRTB();
	return resultRef.detachAndDecrease();
}


Object * Runtime::executeCatchBlock(Block * block,identifierId varNameId,Object * exception) {
	RuntimeBlock * rtb=RuntimeBlock::create(block,getCurrentRTB());
	if(varNameId!=IDENTIFIER_emptyStr && exception){
		rtb->assignToVariable(varNameId,exception);
	}
	pushRTB(rtb);
	ObjRef resultRef=executeRTB(rtb);
	popRTB();
	return resultRef.detachAndDecrease();
}

//! (internal) used by executeBlock(...), executeCatchBlock(...) and executeUserFunction(...)
Object * Runtime::executeRTB(RuntimeBlock * rtb) {
	ObjRef resultRef;
	for ( Block::statementCursor c = rtb->getStaticBlock()->getStatements().begin();
			c != rtb->getStaticBlock()->getStatements().end(); ++c) {
		if ((*c)==NULL) continue;
		resultRef=executeObj( *c );
		if(!checkNormalState()){
			return NULL;
		}
	}
	return resultRef.detachAndDecrease();
}

// ----------------------------------------------------------------------
// ---- Function execution

Object * Runtime::executeFunctionCall(FunctionCall * fCall){
	size_t numParams=0;
	setCallingObject(NULL);
	setCurrentLine(fCall->getLine());

	ObjRef funRef=executeObj(fCall->getStatement()); // this sets the new calling object
	if(!assertNormalState(fCall))
		return NULL;

	ObjRef callingObjectRef=getCallingObject();


	numParams=fCall->getNumParams();
//	callInfo->parameterValues=ParameterValues(numParams);
//    ParameterValues & paramResults=callInfo->parameterValues;
//	callInfo->parameterValues=ParameterValues(numParams);
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
	functionCallStack.push_back(FunctionCallInfo(fCall,callingObjectRef.get(),funRef.get(),&paramResults));

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
		for (size_t i=0; i<numParams;++i) {
			Object::removeReference(paramResults[i].get());
		}
		throw(o);
	}
	functionCallStack.pop_back();

	for (size_t i=0; i<numParams;++i) {
		Object::removeReference(paramResults[i].get());
	}

	// TODO STATE!!!!!
	funRef=NULL; /// As funRef can be == resultRef
	callingObjectRef=NULL; /// As callingObjectRef can be == resultRef
	return resultRef.detachAndDecrease();
}

//! \note	The reference-counter of the result is increased.
Object * Runtime::executeFunction(const ObjPtr & fun,const ObjPtr & _callingObject,const ParameterValues & params,bool isConstructorCall/*=false*/){
	ObjRef resultRef;
	Function * libfun=fun.toType<Function>();
	if (libfun && libfun->getFnPtr()) {
	if(isConstructorCall && _callingObject.toType<Type>()==NULL){
			setExceptionState(new Exception("Can not instantiate non-Type-Object. Hint: Try to check the type you use with 'new'."+getStackInfo(),getCurrentLine()));
			return NULL;
		}
		try {
			resultRef=(*libfun->getFnPtr())(*this,_callingObject.get(),params);
		} catch (Exception * e) {
			setExceptionState(e);
			return NULL;
		} catch(const char * message) {
		std::string exceptionMessage("C++ exception: ");
		exceptionMessage += message;
		setExceptionState(new Exception(exceptionMessage));
		return NULL;
		} catch (Object * obj) {
		// workaround: this should be covered by catching the Exception* directly, but that doesn't always seem to work!?!
		Exception * e=dynamic_cast<Exception *>(obj);
		if(e){
		setExceptionState(e);
		return NULL;
		}
		std::string message=(obj?obj->toString():"NULL")+getStackInfo();
			setExceptionState(new Exception(message));
			return NULL;
		}  catch (...){
			setExceptionState(new Exception("C++ exception"+getStackInfo()));
			return NULL;
		}
	} else if (UserFunction * ufun=fun.toType<UserFunction>()) {
		if (isConstructorCall) {
			resultRef=executeUserConstructor(_callingObject,params); // this ufun is not used, as it's origin is not known
		} else { /// !isConstructorCall
			RuntimeBlock * rtb=createUserFunctionRTB(ufun,params);
			if(rtb==NULL) {// error occured
				return NULL;
			}
			resultRef=executeUserFunction(rtb,_callingObject); // ,ufun->isLambda()
		}
	} else if(Delegate * d=fun.toType<Delegate>()){
		resultRef=executeFunction(d->getFunction(),d->getObject(),params,isConstructorCall);
	} else {
		warn("No function to call.");
	}
	return resultRef.detachAndDecrease();
}

/*! (internal)	Create a RuntimeBlock for a user function and assign the parameterValues to the corresponding local variables.
	\note The calling object is not set here.
	Called by executeFunction(...) and executeUserConstructor(...)	*/
RuntimeBlock * Runtime::createUserFunctionRTB(UserFunction * ufun,const ParameterValues & paramValues){
	RuntimeBlock::RTBRef rtbRef;
//    if(ufun->isLambda()){
//        rtbRef=RuntimeBlock::create(dynamic_cast<Block *>(ufun->getBlock()),getCurrentRTB(),NULL/*_callingObject.get()*/);
//    }else{
	rtbRef=RuntimeBlock::create(dynamic_cast<Block *>(ufun->getBlock()),NULL,NULL/*_callingObject.get()*/);
//    }

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
		return NULL;
		}
		rtbRef->initLocalVariable( name,valueRef.get());
	}

	// assign multiParam
	if(!multiParam.isNull()){
		identifierId name=(*paramExpressions)[paramExpSize-1]->getName();

		for (size_t i=numberOfAssignedParameters;i<paramValues.count();++i) {
			ObjRef valueRef=paramValues.get(i);

			/// check type
			Object * typeExpression=(*paramExpressions)[paramExpSize-1]->getType();
			if(typeExpression!=NULL && !checkType(name,valueRef.get(),typeExpression)){
				return NULL;
			}
			multiParam->pushBack(valueRef.get());
		}
		rtbRef->initLocalVariable( name,multiParam.get());
	}

	/// set "thisFn" variable for recursive calls.
	rtbRef->initLocalVariable( Consts::IDENTIFIER_thisFn,ufun);

	return rtbRef.detachAndDecrease();
}

/*! (internal) Called by createUserFunctionRTB(...)*/
bool Runtime::checkType(const identifierId & name, Object * obj,Object *typeExpression){
	if(typeExpression==NULL||obj==NULL)
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
	setExceptionState(new Exception("Wrong parameter type for parameter " + EScript::identifierIdToString(name)+"\n"+
		"Expected: "+typeExpression->toString()+"\tRecieved: "+obj->getTypeName()+" \t"+getStackInfo()));
	return false;
}

/*! (internal) Called by executeFunction(...) and executeUserConstructor(...)
	Note: The reference-counter of the result is NOT increased.	*/
Object * Runtime::executeUserFunction(RuntimeBlock * rtb,const ObjPtr & _callingObject) { //,bool lambda
	rtb->setCaller(_callingObject.get());
	pushRTB(rtb);
	ObjRef resultRef;
	try {
		resultRef=executeRTB(rtb);
	} catch (Object * exception) {
		popRTB();
		throw(exception);
	}
//	if(!lambda) // if this is  a lambda function, the result is the last value.
	resultRef=NULL;
	bool noReturn=true;
	if(!checkNormalState()){
		if(getState()==Runtime::STATE_RETURNING){
			resultRef=returnRef;
			resetState();
			noReturn=false;
		}else if(!assertNormalState()){
			popRTB();
			return NULL;
		}
	}
	if(noReturn && resultRef.isNull()){ /// if no return is used, return void
		resultRef=Void::get();
	}
	popRTB(); // the rtb is normally destroyed here, as the last reference is removed.
	return resultRef.detachAndDecrease();
}

/*! (internal) Called by executeFunction.
	Note: The reference-counter of the result is NOT increased.	*/
Object * Runtime::executeUserConstructor(const ObjPtr & _callingObject,const ParameterValues & params){

	// seach and stack all constructor method until c++ - constructor is found.
	// C ---|> B ---|> A
	// step along, create runtimeBlocks, calculate parametres, assign parameters
	// execute last c++ constructor to get Object
	// execute other funcitons for initialization using the given blocks.

	Type * type=_callingObject.toType<Type>();
	if(type==NULL){
		setExceptionState(new Exception("Can not instantiate non-Type-Object. Hint: Try to check the type you use with 'new'."+getStackInfo(),getCurrentLine()));
		return NULL;
	}

	std::stack<RuntimeBlock::RTBRef > consCallStack;
	ObjRef baseObj;
	ParameterValues currentParams = params;
	std::list<ObjRef> tmpRefHolderList;
	for(Type * t= type; t!=NULL ; t = t->getBaseType()){
		Object * currentCons = t->getLocalAttribute(Consts::IDENTIFIER_fn_constructor);
		// type has no local constructor function -> skip this level
		if(!currentCons)
			continue;

		// c++ function found -> stop here
		if(	Function * baseCons=dynamic_cast<Function*>(currentCons)){
			// create real object with baseCons( currentPrams)
			baseObj = executeFunction(baseCons,type,currentParams,true);
			break;
		}
		UserFunction * uCons = dynamic_cast<UserFunction*>(currentCons);
		if(uCons==NULL){
			error("Constructor needs to be a function");
			return NULL;
		}
		/// \note the created RTB must not have a parent:
		RuntimeBlock::RTBRef funRTB=createUserFunctionRTB(uCons,currentParams);
		consCallStack.push(funRTB);

		/// create new set of params according to super constructor parameters
		std::vector<ObjRef> & sConstrExpressions=uCons->getSConstructorExpressions();
		if(sConstrExpressions.size()>0){
			currentParams=ParameterValues(sConstrExpressions.size());
			size_t i=0;
			pushRTB(funRTB.get());
			for(std::vector<ObjRef>::iterator it=sConstrExpressions.begin();it!=sConstrExpressions.end();++it){
				Object * expr=it->get();
				ObjRef result;
				if(expr!=NULL){
					result=executeObj(expr);
					/// constructor expressions must not change the state (exception, return, etc..)
					if(!assertNormalState()){
						popRTB();
						return NULL;
					}
				}else{
					result=Void::get();
				}
				tmpRefHolderList.push_back(result.get()); /// hold a temporary reference until  all calls are made
				currentParams.set(i,result);
				++i;
			}
			popRTB(); // the RTB is not destroyed here because of the remaining reference (it is re-used later);
			// this is only allowed as it is not part of a RTB-hirarchie
		}else{
			currentParams=ParameterValues();
		}

	}
	if(baseObj.isNull()){
		error( "No c++ base-contructor found.");
		return NULL;
	}

	// init
	while(!consCallStack.empty()){
		RuntimeBlock * rtb=consCallStack.top().get();

		ObjRef tmp = executeUserFunction(rtb,baseObj); // here the RTB is poped and destroyed... //,false
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


bool Runtime::stateError(Object * obj){
	switch(getState()){
		case STATE_NORMAL:{
			return true;
		}
		case STATE_RETURNING:{
			setExceptionState(new Exception("No return here!"+(obj?" ["+obj->toString()+"]":"")+getStackInfo(),getCurrentLine()));
			break;
		}
		case STATE_BREAKING:{
			setExceptionState(new Exception("No break here!"+(obj?" ["+obj->toString()+"]":"")+getStackInfo(),getCurrentLine()));
			break;
		}
		case STATE_CONTINUE:{
			setExceptionState(new Exception("No continue here!"+(obj?" ["+obj->toString()+"]":"")+getStackInfo(),getCurrentLine()));
			break;
		}
		case STATE_EXITING:{
//			setExceptionState(new Exception("No exit here!"+(obj?" ["+obj->toString()+"]":"")+getStackInfo(),getCurrentLine()));
//			break;
		}
		case STATE_EXCEPTION:{
			// we are already in an exception state...
		}
	}
	return false;
}


void Runtime::info(const std::string & s) {
	std::cout << "\n Note: "<<s;
}


void Runtime::warn(const std::string & s) {
	if(getErrorConfig()&ES_IGNORE_WARNINGS) return;
	if(getErrorConfig()&ES_TREAT_WARNINGS_AS_ERRORS) {
		setExceptionState(new Exception(s));
		return;
	}
	std::cout << "\n WARNING: "<< s << std::endl;
	if(getCurrentRTB()!=NULL){
		Block * b=getCurrentRTB()->getStaticBlock();
		if(b)
			std::cout<<"\tFile:"<<b->getFilename()<<" near line "<<getCurrentLine()<<"\n";
//        std::cout << " *** "<<b->getFilename()<<":"<<b->toString();
	}
}


void Runtime::error(const std::string & s,Object * obj) {
	std::ostringstream os;
	os<<s;
	if(obj) os<<"("<<obj->toString()<<")";
	if(getCurrentRTB()!=NULL){
		Block * b=getCurrentRTB()->getStaticBlock();
		if(b)
			 os<<"\tFile:"<<b->getFilename()<<" near line "<<getCurrentLine()<<"\n";
	}
	os<<getStackInfo();
	throw(new Exception(os.str(),getCurrentLine()));
}

// ----------------------------------------------------------------------
// ---- Debugginh

std::string Runtime::getStackInfo(){
	std::ostringstream os;
	os<<"\n\n----------------------\nCall stack:\n";
	int nr=0;
	for(std::vector<FunctionCallInfo>::reverse_iterator it=functionCallStack.rbegin();it!=functionCallStack.rend();++it){
		if(it!=functionCallStack.rbegin())
			os<<"\n-----------\n";
		os<<(++nr)<<".\n";
		FunctionCallInfo & i=*it;
		if(i.funCall!=NULL)
			os<< "call:\t"<< i.funCall->toDbgString();
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
	os<<"\n----------------------\n";
	return os.str();
}
