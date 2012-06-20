// Logger.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Logger.h"
#include <stdexcept>
#include <iostream>

namespace EScript{



// ------------------------------------------------
// LoggerGroup
void LoggerGroup::addLogger(const std::string & name,Logger * logger){
	if(logger==nullptr)
		throw std::invalid_argument("addLogger(nullptr)");
	loggerRegistry[name] = logger;
}

bool LoggerGroup::removeLogger(const std::string & name){
	return loggerRegistry.erase(name)>0;
}
void LoggerGroup::clearLoggers(){
	loggerRegistry.clear();
}
Logger * LoggerGroup::getLogger(const std::string & name){
	const loggerRegistry_t::iterator lbIt = loggerRegistry.lower_bound(name);
	if(lbIt!=loggerRegistry.end() && !(loggerRegistry.key_comp()(name, lbIt->first)) ){
		return lbIt->second.get();
	}
	return nullptr;
}

//! ---|> Logger
void LoggerGroup::doLog(level_t l,const std::string & msg){
	for(const auto & identifierLoggerPair : loggerRegistry) {
		identifierLoggerPair.second->log(l,msg);
	}
}


// ------------------------------------------------
// StdLogger


//! ---|> Logger
void StdLogger::doLog(level_t l,const std::string & msg){
	out << std::endl;
	switch(l){
		case LOG_DEBUG:{
			out << "Debug: ";
			break;
		}
		case LOG_INFO:{
			out << "Debug: ";
			break;
		}
		case LOG_WARNING:{
			out << "Warning: ";
			break;
		}
		case LOG_PEDANTIC_WARNING:{
			out << "Pedantic warning: ";
			break;
		}
		case LOG_ERROR:{
			out << "Error: ";
			break;
		}
		case LOG_FATAL:{
			out << "Fatal error: ";
			break;
		}
		case LOG_ALL:
		case LOG_NONE:
		default:{
			out << "Logging ("<<static_cast<int>(l)<<"):";
		}
	}
	out << msg << std::endl;
}

}
