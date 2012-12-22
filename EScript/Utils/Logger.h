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
			LOG_ALL = 0,
			LOG_DEBUG = 1,
			LOG_INFO = 2,
			LOG_PEDANTIC_WARNING = 3,
			LOG_WARNING = 4,
			LOG_ERROR = 5,
			LOG_FATAL = 6,
			LOG_NONE = 10
		};
		Logger(level_t _minLevel = LOG_ALL,level_t _maxLevel = LOG_NONE) : minLevel(_minLevel),maxLevel(_maxLevel){}
		virtual ~Logger(){}

		void debug(const std::string & msg)			{	log(LOG_DEBUG,msg);	}
		void error(const std::string & msg)			{	log(LOG_ERROR,msg);	}
		void fatal(const std::string & msg)			{	log(LOG_FATAL,msg);	}
		level_t getMinLevel()const					{	return minLevel;	}
		level_t getMaxLevel()const					{	return maxLevel;	}
		void info(const std::string & msg)			{	log(LOG_INFO,msg);	}
		void log(level_t l,const std::string & msg)	{	if(testLevel(l))	doLog(l,msg);	}
		void pedantic(const std::string & msg)		{	log(LOG_PEDANTIC_WARNING,msg);	}
		void setMinLevel(level_t l)					{	minLevel = l;	}
		void setMaxLevel(level_t l)					{	maxLevel = l;	}
		void warn(const std::string & msg)			{	log(LOG_WARNING,msg);	}

	private:
		bool testLevel(level_t t)const				{	return	static_cast<int>(t)>=static_cast<int>(minLevel) &&
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
class LoggerGroup : public Logger {
	public:
		LoggerGroup(level_t _minLevel = LOG_ALL,level_t _maxLevel = LOG_NONE) : Logger(_minLevel,_maxLevel){}
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
class StdLogger : public Logger {
	public:
		StdLogger(std::ostream & stream, level_t _minLevel = LOG_ALL,level_t _maxLevel = LOG_NONE) : Logger(_minLevel,_maxLevel),out(stream){}
		virtual ~StdLogger(){}
	private:
		//! ---|> Logger
		virtual void doLog(level_t l,const std::string & msg);
		std::ostream & out;
};

}
#endif // ES_Logger_H
