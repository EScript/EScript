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

//! [RuntimeInternals]
class RuntimeInternals  {
		Runtime &runtime;

	//! @name Main
	//	@{
		RuntimeInternals(RuntimeInternals & other); // = delete
	public:
		RuntimeInternals(Runtime & rt);
		~RuntimeInternals();

		void warn(const std::string & message)const;
		void setException(const std::string & message)const	{	runtime.setException(message);	}
	// @}

	// --------------------

	//! @name Function execution
	//	@{
	public:
		/*! (internal)
			Start the execution of a function. A c++ function is executed immediatly and the result is <result,nullptr>.
			A UserFunction produces a FunctionCallContext which still has to be executed. The result is then result.isFunctionCallContext() == true
			\note the @p params value may be altered by this function and should not be used afterwards!	*/
		RtValue startFunctionExecution(const ObjPtr & fun,const ObjPtr & callingObject,ParameterValues & params);

		RtValue startInstanceCreation(EPtr<Type> type,ParameterValues & params);

		ObjRef executeFunctionCallContext(_Ptr<FunctionCallContext> fcc);

		ObjPtr getCallingObject()const							{	return activeFCCs.empty() ? nullptr : activeFCCs.back()->getCaller();	}
		size_t getStackSize()const								{	return activeFCCs.size();	}
		size_t _getStackSizeLimit()const						{	return stackSizeLimit;	}
		void _setStackSizeLimit(const size_t limit)				{	stackSizeLimit = limit;	}

	private:
		std::vector<_CountedRef<FunctionCallContext> > activeFCCs;
		size_t stackSizeLimit;

		static bool checkParameterConstraint(Runtime & rt,const RtValue & value,const ObjPtr & constraint);
		_Ptr<FunctionCallContext> getActiveFCC()const			{	return activeFCCs.empty() ? nullptr : activeFCCs.back();	}

		void pushActiveFCC(const _Ptr<FunctionCallContext> & fcc) {
			activeFCCs.push_back(fcc);
			if(activeFCCs.size()>stackSizeLimit) stackSizeError();
		}
		void popActiveFCC()										{	activeFCCs.pop_back();	}
		void stackSizeError();
	// @}

	// --------------------

	//! @name Globals
	//	@{
	public:
		ObjPtr getGlobalVariable(const StringId & id);
		Namespace * getGlobals()const;
	private:
		ERef<Namespace> globals;
	// @}

	// --------------------

	//! @name Information
	//	@{
	public:
		int getCurrentLine()const;
		std::string getCurrentFile()const;

		std::string getStackInfo();
		std::string getLocalStackInfo();
	// @}

	// --------------------

	//! @name Internal state / Exceptions
	//	@{
	public:
		enum state_t{	STATE_NORMAL,STATE_EXITING,STATE_EXCEPTION	};
		bool checkNormalState()const					{	return state==STATE_NORMAL;	}
		
		ObjRef fetchAndClearException(){
			if(state==STATE_EXCEPTION){
				state = STATE_NORMAL;
				return std::move(resultValue);
			}
			return nullptr;
		}
		ObjRef fetchAndClearExitResult(){
			if(state==STATE_EXITING){
				state = STATE_NORMAL;
				return std::move(resultValue);
			}
			return nullptr;
		}

		state_t getState()const							{	return state;	}


		void setAddStackInfoToExceptions(bool b)		{	addStackIngfoToExceptions = b;	}

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
		void throwException(const std::string & s,Object * obj = nullptr);

		void setExitState(ObjRef value) {
			resultValue = std::move(value);
			state = STATE_EXITING;
		}
		void setExceptionState(ObjRef value) {
			resultValue = std::move(value);
			state = STATE_EXCEPTION;
		}

	private:
		state_t state;
		ObjRef resultValue;
		bool addStackIngfoToExceptions;
	// @}

	// --------------------

	//! @name System calls
	//	@{
	//! (interna) Used by the Runtime.
	private:
		typedef RtValue ( * sysFunctionPtr)(RuntimeInternals &,const ParameterValues &);
		std::vector<sysFunctionPtr > systemFunctions;
		void initSystemFunctions();
	public:
		RtValue sysCall(uint32_t sysFnId,ParameterValues & params);
	//	@}
};
}

#endif // ES_RUNTIME_INTERNALS_H
