// Runtime.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef RUNTIME_H
#define RUNTIME_H

#include "RuntimeContext.h"
#include "../Objects/ExtObject.h"
#include "../Utils/Logger.h"
#include "../Utils/ObjRef.h"
#include "../Utils/ObjArray.h"
#include <stack>
#include <vector>
#include <string>

namespace EScript {
namespace AST{
class BlockStatement;
class FunctionCallExpr;
}
class UserFunction;
class Exception;
class FunctionCallContext;
class StringData;


/*! [Runtime] ---|> [ExtObject]    */
class Runtime : public ExtObject  {
		ES_PROVIDES_TYPE_NAME(Runtime)
	public:
		static Type * getTypeObject();
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
		Object * getVariable(const StringId id);
		Object * getGlobalVariable(const StringId id);
		Namespace * getGlobals()const;

		void assignToVariable(const StringId id,Object * value);
		bool assignToAttribute(ObjPtr obj,StringId attrId,ObjPtr value);
	public:
		/*! returns the object's attribute with the given id. If the attribute can not be found, NULL is returned.
			If the attribute can be found but not accessed, a warning is emitted and NULL is returned. */
		Object * readMemberAttribute(Object * obj,const StringId id);

		ERef<Namespace> globals;
	// 	@}

	// ------------------------------------------------

	/// @name General execution
	// 	@{
	public:
		Object * eval(const StringData & code);
		Object * executeObj(Object * obj);

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
		Object * executeBlock(AST::BlockStatement * block);

		ObjRef callingObject;
		std::stack<RuntimeContext::RTBRef> contextStack;
		size_t stackSizeLimit;
	//	@}

	// ------------------------------------------------

	/// @name Bytecode execution
	// 	@{	
		typedef std::pair<Object *,FunctionCallContext* >  executeFunctionResult_t;
		executeFunctionResult_t startFunctionExecution(FunctionCallContext & fcc,const ObjPtr & fun,const ObjPtr & callingObject,const ParameterValues & params);
	public:
		Object * executeUserFunction(EPtr<UserFunction> userFunctions);
		
	//	@}

	// ------------------------------------------------

	/// @name Function execution
	// 	@{
	public:
		Object * executeFunctionCall(AST::FunctionCallExpr * fCall);
		Object * executeFunction(const ObjPtr & fun,const ObjPtr & callingObject,const ParameterValues & params,bool isConstructor=false);
	private:
		RuntimeContext * createAndPushFunctionCallContext(const ObjPtr & callingObject,UserFunction * ufun,const ParameterValues & paramValues);
		Object * executeUserConstructor(const ObjPtr & _callingObject,const ParameterValues & paramValues);
		bool checkType(const StringId & name,Object * obj,Object *typeExpression);
		/*! (internal) Used to track the status of the active function calls (for stack traces) */
		struct FunctionCallInfo{
			AST::FunctionCallExpr * funCall;
			Object * callingObject;
			Object * function;
			ParameterValues * parameterValues;

			FunctionCallInfo(AST::FunctionCallExpr * _funCall,Object * _callingObject,Object * _function,ParameterValues * _pValues):
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
		Logger::level_t getLoggingLevel()				{	return logger->getMinLevel();	}
		void setLoggingLevel(Logger::level_t level)		{	logger->setMinLevel(level);	}
		void setTreatWarningsAsError(bool b);

		int getCurrentLine()const;
		std::string getCurrentFile()const;
		LoggerGroup * getLogger()const					{	return logger.get();	}

		std::string getStackInfo();

		void log(Logger::level_t l,const std::string & s)	{	logger->log(l,s);	}

		void enableLogCounting();
		void disableLogCounting();
		void resetLogCounter(Logger::level_t level);
		uint32_t getLogCounter(Logger::level_t level)const;
	private:
		_CountedRef<LoggerGroup> logger;
	// 	@}

};
}

#endif // CLASS_H
