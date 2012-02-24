// Logger.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_Logger_H
#define ES_Logger_H

#include "EReferenceCounter.h"
#include "ObjRef.h"

#include <map>
#include <string>
#include <ostream>

namespace EScript {

//! [Logger]
class Logger : public EReferenceCounter<Logger> {
	public:
		enum level_t{
			_ALL = 0,
			DEBUG = 1,
			INFO = 2,
			WARNING = 3,
			PEDANTIC_WARNING = 4,
			ERROR = 5,
			FATAL = 6,
			_NONE = 10
		};
		Logger(level_t _minLevel=_ALL,level_t _maxLevel = _ALL) : minLevel(_minLevel),maxLevel(_maxLevel){}
		virtual ~Logger(){}
		
		void debug(const std::string & msg)			{	log(DEBUG,msg);	}
		void error(const std::string & msg)			{	log(ERROR,msg);	}
		void fatal(const std::string & msg)			{	log(FATAL,msg);	}
		level_t getMinLevel()const					{	return minLevel;	}
		level_t getMaxLevel()const					{	return maxLevel;	}
		void info(const std::string & msg)			{	log(INFO,msg);	}
		void log(level_t l,const std::string & msg)	{	if(testLevel(l))	doLog(l,msg);	}
		void pedantic(const std::string & msg)		{	log(PEDANTIC_WARNING,msg);	}
		void warn(const std::string & msg)			{	log(WARNING,msg);	}

		
	private:
		bool testLevel(level_t t)const				{	return 	static_cast<int>(t)>=static_cast<int>(minLevel) && 
																static_cast<int>(t)<=static_cast<int>(maxLevel); }

		//! ---o
		virtual void doLog(level_t l,const std::string & msg) = 0;
		
		level_t minLevel;
		level_t maxLevel;
};

// ------------------------------------------------------------------------------------------------

/*! [LoggerGroup] ---|>[Logger]
		|
		--------> [Logger*]		*/
class LoggerGroup : Logger {
	public:
		LoggerGroup(level_t _minLevel=_ALL,level_t _maxLevel = _ALL) : Logger(_minLevel,_maxLevel){}
		virtual ~LoggerGroup(){}
		
		void addLogger(const std::string & name,Logger * logger);
		bool removeLogger(const std::string & name);
		void clearLoggers();
		Logger * getLogger(const std::string & name);
	private:
		//! ---|> Logger
		virtual void doLog(level_t l,const std::string & msg);
		typedef std::map<std::string, _CountedRef<Logger> > loggerRegistry_t;
		loggerRegistry_t loggerRegistry;
};

// ------------------------------------------------------------------------------------------------

/*! [StdLogger] ---|>[Logger]
		|
		--------> std::ostream		*/
class StdLogger : Logger {
	public:
		StdLogger(std::ostream & stream, level_t _minLevel=_ALL,level_t _maxLevel = _ALL) : Logger(_minLevel,_maxLevel),out(stream){}
		virtual ~StdLogger(){}
	private:
		//! ---|> Logger
		virtual void doLog(level_t l,const std::string & msg);
		std::ostream & out;
};

}
#endif // ES_Logger_H
