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
		Namespace * getGlobals()const;

		bool assignToAttribute(ObjPtr obj,StringId attrId,ObjPtr value);
	public:
//		/*! returns the object's attribute with the given id. If the attribute can not be found, NULL is returned.
//			If the attribute can be found but not accessed, a warning is emitted and NULL is returned. */
//		ObjPtr readMemberAttribute(ObjPtr obj,const StringId id);


	// 	@}

	// ------------------------------------------------

	/// @name Execution
	// 	@{
	public:
		ObjPtr getCallingObject()const;

		ObjRef executeFunction(const ObjPtr & fun,const ObjPtr & callingObject,const ParameterValues & params);
		ObjRef createInstance(const EPtr<Type> & type,const ParameterValues & params);
		
		void yieldNext(YieldIterator & yIt);
	//	@}

	// ------------------------------------------------

	/// @name Internal state / Exceptions
	// 	@{
	public:

		void info(const std::string & s);
		void warn(const std::string & s);

		bool checkNormalState()const;

		/*! Creates an exception object including current stack info and
			sets the internal state to STATE_EXCEPTION. Does NOT throw a C++ exception. */
		void setException(const std::string & s);

		/*! Annotates the given Exception with the current stack info and set the internal state
			to STATE_EXCEPTION. Does NOT throw a C++ exception. */
		void setException(Exception * e);
		
		//! (internal) Like 'setException' but does NOT annotate the given exception but just uses it.
		void _setExceptionState(const ObjPtr e);

		//! (internal) 
		void _setExitState(const ObjPtr e);

		/**
		 * Throws a runtime exception (a C++ Exception, not an internal one!).
		 * Should only be used inside of library-functions
		 * (otherwise, they are not handled and the program is likely to crash).
		 * In all other situations try to use setException(...)
		 */
		void throwException(const std::string & s,Object * obj=NULL);

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

#endif // ES_RUNTIME_H
