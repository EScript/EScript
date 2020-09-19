// Runtime.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2012-2013 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
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

#if defined(__GNUC__)
#define DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED __declspec(deprecated)
#else
#pragma message("WARNING: DEPRECATED not set for this compiler")
#define DEPRECATED
#endif

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
		ESCRIPTAPI static Type* getTypeObject();
		ESCRIPTAPI static void init(EScript::Namespace & globals);

	// ------------------------------------------------

	//! @name Main
	//	@{
	private:
		std::unique_ptr<RuntimeInternals> internals;
	public:
		ESCRIPTAPI Runtime();
		ESCRIPTAPI virtual ~Runtime();
	// @}

	// ------------------------------------------------

	//! @name Variables
	//	@{
	public:
		ESCRIPTAPI Namespace * getGlobals()const;

		ESCRIPTAPI bool assignToAttribute(ObjPtr obj,StringId attrId,ObjPtr value);
	//	@}

	// ------------------------------------------------

	//! @name Execution
	//	@{
	public:
		ESCRIPTAPI ObjPtr getCallingObject()const;

		//! \note throws an exception (Object *) on failure
		ESCRIPTAPI ObjRef executeFunction(const ObjPtr & fun,const ObjPtr & callingObject,const ParameterValues & params);

		//! \note throws an exception (Object *) on failure
		ESCRIPTAPI ObjRef createInstance(const EPtr<Type> & type,const ParameterValues & params);

		//! \note throws an exception (Object *) on failure
		ESCRIPTAPI void yieldNext(YieldIterator & yIt);

		ESCRIPTAPI size_t getStackSize()const;
		ESCRIPTAPI size_t _getStackSizeLimit()const;
		ESCRIPTAPI void _setStackSizeLimit(const size_t limit);
	//	@}

	// ------------------------------------------------

	//! @name Internal state / Exceptions
	//	@{
	public:
		ESCRIPTAPI DEPRECATED bool assertNormalState()const;

		ESCRIPTAPI void info(const std::string & s);
		ESCRIPTAPI void warn(const std::string & s);

		ESCRIPTAPI ObjRef fetchAndClearExitResult();

		ESCRIPTAPI bool checkNormalState()const;

		/*! Creates an exception object including current stack info and
			sets the internal state to STATE_EXCEPTION. Does NOT throw a C++ exception. */
		ESCRIPTAPI void setException(const std::string & s);

		/*! Annotates the given Exception with the current stack info and set the internal state
			to STATE_EXCEPTION. Does NOT throw a C++ exception. */
		ESCRIPTAPI void setException(Exception * e);

		//! (internal) Like 'setException' but does NOT annotate the given exception but just uses it.
		ESCRIPTAPI void _setExceptionState(ObjRef e);

		//! (internal)
		ESCRIPTAPI void _setExitState(ObjRef e);

		/**
		 * Throws a runtime exception (a C++ Exception, not an internal one!).
		 * Should only be used inside of library-functions
		 * (otherwise, they are not handled and the program is likely to crash).
		 * In all other situations try to use setException(...)
		 */
		ESCRIPTAPI void throwException(const std::string & s,Object * obj = nullptr);
	//	@}

	// ------------------------------------------------

	//! @name Debugging
	//	@{
	public:
		ESCRIPTAPI void enableLogCounting();
		ESCRIPTAPI void disableLogCounting();

		ESCRIPTAPI std::string getCurrentFile()const;
		ESCRIPTAPI int getCurrentLine()const;
		ESCRIPTAPI uint32_t getLogCounter(Logger::level_t level)const;
		LoggerGroup * getLogger()const					{	return logger.get();	}
		Logger::level_t getLoggingLevel()				{	return logger->getMinLevel();	}
		ESCRIPTAPI std::string getStackInfo();
		ESCRIPTAPI std::string getLocalStackInfo();

		void log(Logger::level_t l,const std::string & s)	{	logger->log(l,s);	}
		ESCRIPTAPI void resetLogCounter(Logger::level_t level);

		ESCRIPTAPI void setAddStackInfoToExceptions(bool b);
		void setLoggingLevel(Logger::level_t level)		{	logger->setMinLevel(level);	}
		ESCRIPTAPI void setTreatWarningsAsError(bool b);

	private:
		_CountedRef<LoggerGroup> logger;
	//	@}

};
}

#endif // ES_RUNTIME_H
