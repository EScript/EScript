// Runtime.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef RUNTIME_H
#define RUNTIME_H

#include "FunctionCallContext.h"
#include "../Objects/ExtObject.h"
#include "../Utils/Logger.h"
#include "../Utils/ObjRef.h"
#include "../Utils/ObjArray.h"
#include <stack>
#include <vector>
#include <string>
#include <memory>


namespace EScript {

class Function;
class UserFunction;
class Exception;
class FunctionCallContext;
class StringData;
class YieldIterator;

class RuntimeInternals;


/*! [Runtime] ---|> [ExtObject]    */
class Runtime : public ExtObject  {
		ES_PROVIDES_TYPE_NAME(Runtime)
	public:
		static Type * getTypeObject();
		static void init(EScript::Namespace & globals);

	// ------------------------------------------------

	/// @name Main
	// 	@{
	private:
		std::auto_ptr<RuntimeInternals> internals;
	public:
		Runtime();
		virtual ~Runtime();

	// @}

	// ------------------------------------------------

	/// @name Variables
	// 	@{
	public:
//		Object * getVariable(const StringId id);
		ObjPtr getGlobalVariable(const StringId id);
		Namespace * getGlobals()const;

//		void assignToVariable(const StringId id,Object * value);
		bool assignToAttribute(ObjPtr obj,StringId attrId,ObjPtr value);
	public:
//		/*! returns the object's attribute with the given id. If the attribute can not be found, NULL is returned.
//			If the attribute can be found but not accessed, a warning is emitted and NULL is returned. */
//		ObjPtr readMemberAttribute(ObjPtr obj,const StringId id);

		ERef<Namespace> globals;
	// 	@}

	// ------------------------------------------------

	/// @name General execution
	// 	@{
	public:
		Object * eval(const StringData & code);

		ObjPtr getCallingObject()const 					{  return activeFCCs.empty() ? NULL : activeFCCs.back()->getCaller();	}


		size_t getStackSize()const						{	return activeFCCs.size();	}
		size_t getStackSizeLimit()const					{	return stackSizeLimit;	}
		void setStackSizeLimit(size_t s)				{	stackSizeLimit = s;	}

		static bool checkParameterConstraint(Runtime & rt,const ObjPtr & value,const ObjPtr & constraint);
	private:
		size_t stackSizeLimit;
	//	@}

	// ------------------------------------------------

	/// @name Bytecode execution
	// 	@{
	private:
		typedef std::pair<Object *,FunctionCallContext* >  executeFunctionResult_t;

		/*! (internal)
			Start the execution of a function. A c++ function is executed immediatly and the result is <result,NULL>.
			A UserFunction produces a FunctionCallContext which still has to be executed. The result is then <NULL,fcc>
			\note the @p params value may be altered by this function and should not be used afterwards!	*/
		executeFunctionResult_t startFunctionExecution(const ObjPtr & fun,const ObjPtr & callingObject,ParameterValues & params);

		executeFunctionResult_t startInstanceCreation(EPtr<Type> type,ParameterValues & params);
	
		Object * executeFunctionCallContext(_Ptr<FunctionCallContext> fcc);
		
		
		std::vector<_CountedRef<FunctionCallContext> > activeFCCs;
		
		void pushActiveFCC(const _Ptr<FunctionCallContext> fcc)	{	activeFCCs.push_back(fcc);	}
		void popActiveFCC()										{	activeFCCs.pop_back();	}
		_Ptr<FunctionCallContext> getActiveFCC()const			{	return activeFCCs.empty() ? NULL : activeFCCs.back();	}
		
		int activeInstructionPos;
	public:
		ObjRef executeFunction2(const ObjPtr & fun,const ObjPtr & callingObject,const ParameterValues & params);
		ObjRef createInstance(const EPtr<Type> & type,const ParameterValues & params);
		
		void yieldNext(YieldIterator & yIt);
	//	@}

	// ------------------------------------------------

	/// @name States
	// 	@{
	public:
		enum state_t{
			STATE_NORMAL,STATE_EXITING,STATE_EXCEPTION
		};
		bool assertNormalState() 						{	return state==STATE_NORMAL ? true : stateError();	}

		bool checkNormalState()const					{	return state==STATE_NORMAL;	}
		state_t getState()const							{	return state;	}
		void resetState() {
			state=STATE_NORMAL;
			returnRef=NULL;
		}


		Object * getResult()const 						{	return returnRef.get();	} //! \todo rename to exception

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

		void setExitState(const ObjRef & value) {
			returnRef=value;
			state=STATE_EXITING;
		}
		void setExceptionState(const ObjRef & exceptionObj) {
			returnRef=exceptionObj;
			state=STATE_EXCEPTION;
		}
		
	private:
		bool stateError();
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
