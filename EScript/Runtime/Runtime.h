// Runtime.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef RUNTIME_H
#define RUNTIME_H

#include "../Objects/ExtObject.h"
#include "RuntimeBlock.h"
#include "../Utils/ObjRef.h"
#include "../Utils/ObjArray.h"
#include <stack>
#include <vector>
#include <string>

namespace EScript {

class Block;
class UserFunction;
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
		Object * executeCatchBlock(Block * block,identifierId varNameId,Object * exception);

		inline void setCallingObject(Object * obj)	{  callingObject=obj;	}
		Object * getCallingObject()const 			{  return callingObject.get();	}

	private:
		inline void pushRTB(RuntimeBlock * rtb)		{	runtimeBlockStack.push(rtb);	}
		inline void popRTB()						{	runtimeBlockStack.pop();		}
		inline RuntimeBlock * getCurrentRTB()const	{	return runtimeBlockStack.top().get();	}
		Object * executeRTB(RuntimeBlock * rtb);

		ObjRef callingObject;
		std::stack<RuntimeBlock::RTBRef> runtimeBlockStack;
	//	@}

	// ------------------------------------------------

	/// @name Function execution
	// 	@{
	public:
		Object * executeFunctionCall(FunctionCall * fCall);
		Object * executeFunction(const ObjPtr & fun,const ObjPtr & callingObject,const ParameterValues & params,bool isConstructor=false);

	private:
		Object * executeUserFunction(RuntimeBlock * rtb,const ObjPtr & _callingObject);
		RuntimeBlock * createUserFunctionRTB(UserFunction * ufun,const ParameterValues & paramValues);
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
			STATE_NORMAL,STATE_BREAKING,STATE_CONTINUE,STATE_RETURNING,STATE_EXITING,STATE_EXCEPTION
		};

		bool checkNormalState()const					{	return state==STATE_NORMAL;	}
		state_t getState()const							{	return state;	}
		void setState(state_t newState)					{	state=newState; /* TODO only when state was 0? */ }
		void resetState() {
			state=STATE_NORMAL;
			returnRef=NULL;
		}
		void setReturnState(const ObjRef & value) {
			returnRef=value;
			state=STATE_RETURNING;
		}
		void setExitState(const ObjRef & value) {
			returnRef=value;
			state=STATE_EXITING;
		}
		void setExceptionState(const ObjRef & exception) {
			returnRef=exception;
			state=STATE_EXCEPTION;
		}
		Object * getResult()const 						{	return returnRef.get();	}
		bool assertNormalState(Object * obj=NULL) 		{	return state==STATE_NORMAL ? true : stateError(obj);	}
		bool stateError(Object * obj);

		void info(const std::string & s);
		void warn(const std::string & s);
		/**
		 * Throws a runtime exception (a C++ Exception, not an internal one!).
		 * Should only be used inside of library-functions
		 * (otherwise, they are not handled and the program is likely to crash).
		 * In all other situations try to use setExceptionState(...)
		 */
		void error(const std::string & s,Object * obj=NULL);

	private:
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

		int getCurrentLine()const						{	return currentLine;	}
		void setCurrentLine(int l)						{	currentLine=l;	}

		std::string getStackInfo();

	private:
		unsigned int errorConfig;
		int currentLine;
	// 	@}

};
}

#endif // CLASS_H
