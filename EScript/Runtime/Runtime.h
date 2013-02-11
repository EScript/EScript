// Runtime.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_RUNTIME_H
#define ES_RUNTIME_H

#include "../Objects/ExtObject.h"
#include "../Utils/Logger.h"
#include "../Utils/ObjRef.h"
#include "../Utils/ObjArray.h"
#include <stack>
#include <vector>
#include <string>
#include <memory>

namespace EScript {

class Exception;
class RtValue;
class StringData;
class YieldIterator;
class RuntimeInternals;

//! [Runtime] ---|> [ExtObject]
class Runtime : public ExtObject {
		ES_PROVIDES_TYPE_NAME(Runtime)
	public:
		static Type* getTypeObject();
		static void init(EScript::Namespace & globals);

	// ------------------------------------------------

	//! @name Main
	//	@{
	private:
		std::unique_ptr<RuntimeInternals> internals;
	public:
		Runtime();
		virtual ~Runtime();
	// @}

	// ------------------------------------------------

	//! @name Variables
	//	@{
	public:
		Namespace * getGlobals()const;

		bool assignToAttribute(ObjPtr obj,StringId attrId,ObjPtr value);
	//	@}

	// ------------------------------------------------

	//! @name Execution
	//	@{
	public:
		ObjPtr getCallingObject()const;

		//! \note throws an exception (Object *) on failure
		ObjRef executeFunction(const ObjPtr & fun,const ObjPtr & callingObject,const ParameterValues & params);

		//! \note throws an exception (Object *) on failure
		ObjRef createInstance(const EPtr<Type> & type,const ParameterValues & params);

		//! \note throws an exception (Object *) on failure
		void yieldNext(YieldIterator & yIt);

		size_t getStackSize()const;
		size_t _getStackSizeLimit()const;
		void _setStackSizeLimit(const size_t limit);
	//	@}

	// ------------------------------------------------

	//! @name Internal state / Exceptions
	//	@{
	public:
		bool assertNormalState()const __attribute__((deprecated));

		void info(const std::string & s);
		void warn(const std::string & s);

		ObjRef fetchAndClearExitResult();
		
		bool checkNormalState()const;

		/*! Creates an exception object including current stack info and
			sets the internal state to STATE_EXCEPTION. Does NOT throw a C++ exception. */
		void setException(const std::string & s);

		/*! Annotates the given Exception with the current stack info and set the internal state
			to STATE_EXCEPTION. Does NOT throw a C++ exception. */
		void setException(Exception * e);

		//! (internal) Like 'setException' but does NOT annotate the given exception but just uses it.
		void _setExceptionState(ObjRef e);

		//! (internal)
		void _setExitState(ObjRef e);

		/**
		 * Throws a runtime exception (a C++ Exception, not an internal one!).
		 * Should only be used inside of library-functions
		 * (otherwise, they are not handled and the program is likely to crash).
		 * In all other situations try to use setException(...)
		 */
		void throwException(const std::string & s,Object * obj = nullptr);
	//	@}

	// ------------------------------------------------

	//! @name Debugging
	//	@{
	public:
		void enableLogCounting();
		void disableLogCounting();

		std::string getCurrentFile()const;
		int getCurrentLine()const;
		uint32_t getLogCounter(Logger::level_t level)const;
		LoggerGroup * getLogger()const					{	return logger.get();	}
		Logger::level_t getLoggingLevel()				{	return logger->getMinLevel();	}
		std::string getStackInfo();
		std::string getLocalStackInfo();

		void log(Logger::level_t l,const std::string & s)	{	logger->log(l,s);	}
		void resetLogCounter(Logger::level_t level);

		void setAddStackInfoToExceptions(bool b);
		void setLoggingLevel(Logger::level_t level)		{	logger->setMinLevel(level);	}
		void setTreatWarningsAsError(bool b);

	private:
		_CountedRef<LoggerGroup> logger;
	//	@}

};
}

#endif // ES_RUNTIME_H
