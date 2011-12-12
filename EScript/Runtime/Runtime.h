// Runtime.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef RUNTIME_H
#define RUNTIME_H

#include "../Objects/ExtObject.h"
//#include "RuntimeBlock.h"
#include "RuntimeContext.h"
#include "../Utils/ObjRef.h"
#include "../Utils/ObjArray.h"
#include <stack>
#include <vector>
#include <string>

namespace EScript {

class Block;
class UserFunction;
class Exception;
class FunctionCall;

/*! [Runtime] ---|> [ExtObject]    */
class Runtime : public ExtObject  {
		ES_PROVIDES_TYPE_NAME(Runtime)
	public:
		static Type* typeObject;
		static void init(EScript::Namespace & globals);

	// ------------------------------------------------

	/// @name Main
	// 	@{
	public:
		Runtime();
		virtual ~Runtime();

	// @}

	// ------------------------------------------------

	/// @name Variables
	// 	@{
	public:
		/*! returns the object's attribute with the given id. If the attribute can not be found, NULL is returned.
			If the attribute can be found but not accessed, a warning is emitted and NULL is returned. */
		Object * getMemberAttribute(Object * obj,const identifierId id);
		Object * getVariable(const identifierId id);
		Object * getGlobalVariable(const identifierId id);
		Namespace * getGlobals()const;

		void assignToVariable(const identifierId id,Object * value);
		void assignToVariable(const std::string & name,Object * value) {
			assignToVariable(EScript::stringToIdentifierId(name),value);
		}
	public:
		ERef<Namespace> globals;
	// 	@}

	// ------------------------------------------------

	/// @name General execution
	// 	@{
	public:
		Object * executeObj(Object * obj);
		Object * executeBlock(Block * block);

		void setCallingObject(Object * obj)				{  callingObject=obj;	}
		Object * getCallingObject()const 				{  return callingObject.get();	}

		RuntimeContext * getCurrentContext()const		{	return contextStack.top().get();	}
		void pushContext(RuntimeContext * s)			{	contextStack.push(s);	}
		void popContext()								{	contextStack.pop();		}
		Object * executeCurrentContext(bool markEntry=false);

		size_t getStackSize()const						{	return contextStack.size();	}
		size_t getStackSizeLimit()const					{	return stackSizeLimit;	}
		void setStackSizeLimit(size_t s)				{	stackSizeLimit = s;	}

	private:
		ObjRef callingObject;
		std::stack<RuntimeContext::RTBRef> contextStack;
		size_t stackSizeLimit;
	//	@}

	// ------------------------------------------------

	/// @name Function execution
	// 	@{
	public:
		Object * executeFunctionCall(FunctionCall * fCall);
		Object * executeFunction(const ObjPtr & fun,const ObjPtr & callingObject,const ParameterValues & params,bool isConstructor=false);

	private:
		RuntimeContext * createAndPushFunctionCallContext(const ObjPtr & callingObject,UserFunction * ufun,const ParameterValues & paramValues);
		Object * executeUserConstructor(const ObjPtr & _callingObject,const ParameterValues & paramValues);
		bool checkType(const identifierId & name,Object * obj,Object *typeExpression);
		/*! (internal) Used to track the status of the active function calls (for stack traces) */
		struct FunctionCallInfo{
			FunctionCall * funCall;
			Object * callingObject;
			Object * function;
			ParameterValues * parameterValues;

			FunctionCallInfo(FunctionCall * _funCall,Object * _callingObject,Object * _function,ParameterValues * _pValues):
				funCall(_funCall),callingObject(_callingObject),function(_function),parameterValues(_pValues) { }
		};
		std::vector<FunctionCallInfo> functionCallStack;
	//	@}

	// ------------------------------------------------

	/// @name States
	// 	@{
	public:
		enum state_t{
			STATE_NORMAL,STATE_BREAKING,STATE_CONTINUING,STATE_RETURNING,STATE_YIELDING,STATE_EXITING,STATE_EXCEPTION
		};
		bool assertNormalState(Object * obj=NULL) 		{	return state==STATE_NORMAL ? true : stateError(obj);	}
		
		bool checkNormalState()const					{	return state==STATE_NORMAL;	}
		state_t getState()const							{	return state;	}
		void resetState() {
			state=STATE_NORMAL;
			returnRef=NULL;
		}


		Object * getResult()const 						{	return returnRef.get();	}

		void info(const std::string & s);
		void warn(const std::string & s);

		/*! Creates an exception object including current stack info and
			sets the state to STATE_EXCEPTION. Does NOT throw a C++ exception. */
		void setException(const std::string & s);
		
		/*! Annotates the given Exception with the current stack info and set the state 
			to STATE_EXCEPTION. Does NOT throw a C++ exception. */
		void setException(Exception * e);

		/**
		 * Throws a runtime exception (a C++ Exception, not an internal one!).
		 * Should only be used inside of library-functions
		 * (otherwise, they are not handled and the program is likely to crash).
		 * In all other situations try to use setException(...) 
		 */
		void throwException(const std::string & s,Object * obj=NULL);

	private:
		void setState(state_t newState)					{	state=newState; /* TODO only when state was 0? */ }
		void setReturnState(const ObjRef & value) {
			returnRef=value;
			state=STATE_RETURNING;
		}
		void setYieldingState(const ObjRef & value) {
			returnRef=value;
			state=STATE_YIELDING;
		}
		void setExitState(const ObjRef & value) {
			returnRef=value;
			state=STATE_EXITING;
		}
		void setExceptionState(const ObjRef & exceptionObj) {
			returnRef=exceptionObj;
			state=STATE_EXCEPTION;
		}		
		bool stateError(Object * obj);
		state_t state;
		ObjRef returnRef;
	// 	@}

	// ------------------------------------------------

	/// @name Debugging
	// 	@{
	public:
		static const unsigned int ES_IGNORE_WARNINGS=1<<0;
		static const unsigned int ES_TREAT_WARNINGS_AS_ERRORS=1<<1;

		void setErrorConfig(unsigned int _errorConfig)	{	errorConfig=_errorConfig;	}
		unsigned int getErrorConfig()					{	return this->errorConfig;	}

		int getCurrentLine()const;
		std::string getCurrentFile()const;

		std::string getStackInfo();

	private:
		unsigned int errorConfig;
	// 	@}

};
}

#endif // CLASS_H
