// RuntimeInternals.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_RUNTIME_INTERNALS_H
#define ES_RUNTIME_INTERNALS_H

#include "FunctionCallContext.h"
#include "Runtime.h"

namespace EScript {
class Function;

/*! [RuntimeInternals] */
class RuntimeInternals  {
		Runtime &runtime;

	/// @name Main
	// 	@{
		RuntimeInternals(RuntimeInternals & other); // = delete
	public:
		RuntimeInternals(Runtime & rt);
		~RuntimeInternals();
		
		void warn(const std::string & message)const			{	runtime.warn(message);	}
		void setException(const std::string & message)const	{	runtime.setException(message);	}
	// @}

	// --------------------
	
	/// @name Function execution
	// 	@{
	public:
		typedef std::pair<Object *,FunctionCallContext* >  executeFunctionResult_t;

		/*! (internal)
			Start the execution of a function. A c++ function is executed immediatly and the result is <result,NULL>.
			A UserFunction produces a FunctionCallContext which still has to be executed. The result is then <NULL,fcc>
			\note the @p params value may be altered by this function and should not be used afterwards!	*/
		executeFunctionResult_t startFunctionExecution(const ObjPtr & fun,const ObjPtr & callingObject,ParameterValues & params);

		executeFunctionResult_t startInstanceCreation(EPtr<Type> type,ParameterValues & params);
	
		Object * executeFunctionCallContext(_Ptr<FunctionCallContext> fcc);
		
		ObjPtr getCallingObject()const 							{  return activeFCCs.empty() ? NULL : activeFCCs.back()->getCaller();	}

	private:
		std::vector<_CountedRef<FunctionCallContext> > activeFCCs;
		int activeInstructionPos;	
	
		static bool checkParameterConstraint(Runtime & rt,const ObjPtr & value,const ObjPtr & constraint);
		_Ptr<FunctionCallContext> getActiveFCC()const			{	return activeFCCs.empty() ? NULL : activeFCCs.back();	}

		void pushActiveFCC(const _Ptr<FunctionCallContext> fcc)	{	activeFCCs.push_back(fcc);	}
		void popActiveFCC()										{	activeFCCs.pop_back();	}
		
	// @}

	// --------------------
	
	/// @name Globals
	// 	@{	
	public:
		ObjPtr getGlobalVariable(const StringId id);
		Namespace * getGlobals()const;
	private:
		ERef<Namespace> globals;
	// @}

	// --------------------
	
	/// @name Information
	// 	@{	
	public:
		int getCurrentLine()const;
		std::string getCurrentFile()const;
	// @}

	// --------------------
	
	/// @name Internal state
	// 	@{
	public:
		enum state_t{	STATE_NORMAL,STATE_EXITING,STATE_EXCEPTION	};
		bool checkNormalState()const					{	return state==STATE_NORMAL;	}
		ObjPtr getResult()const							{	return resultValue;	}
		state_t getState()const							{	return state;	}
		void resetState() {
			state = STATE_NORMAL;
			resultValue = NULL;
		}
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

		void setExitState(const ObjPtr & value) {
			resultValue = value;
			state = STATE_EXITING;
		}
		void setExceptionState(const ObjPtr & exceptionObj) {
			resultValue = exceptionObj;
			state = STATE_EXCEPTION;
		}
		
	private:
		state_t state;
		ObjRef resultValue;
	// @}

	// --------------------
	
	/// @name System calls
	// 	@{
	//! (interna) Used by the Runtime.
	private:
		std::vector<ERef<Function> > systemFunctions;
		void initSystemFunctions();
	public:
		Object * sysCall(uint32_t sysFnId,ParameterValues & params);
	//	@}
};
}

#endif // ES_RUNTIME_INTERNALS_H
