// StdLib.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "StdLib.h"

#include "../EScript/Basics.h"
#include "../EScript/StdObjects.h"
#include "../EScript/Objects/Callables/UserFunction.h"
#include "../EScript/Compiler/Compiler.h"
#include "../EScript/Compiler/Parser.h"
#include "../EScript/Utils/IO/IO.h"
#include "../EScript/Consts.h"
#include "ext/JSON.h"

#include <sstream>
#include <stdlib.h>
#include <ctime>
#include <unistd.h>
#include <cstdlib>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace EScript{

std::string StdLib::getOS(){
	#if defined(_WIN32) || defined(_WIN64)
	return std::string("WINDOWS");
	#elif defined(__APPLE__)
	return std::string("MAC OS");
	#elif defined(__linux__)
	return std::string("LINUX");
	#elif defined(__unix__)
	return std::string("UNIX");
	#else
	return std::string("UNKNOWN");
	#endif
}

//! (static)
void StdLib::print_r(Object * o,int maxLevel,int level) {
	if(!o) return;
	if(level>maxLevel) {
		std::cout << " ... " << std::endl;
		return;
	}

	if(Array * a = dynamic_cast<Array *>(o)) {
		std::cout << "[\n";
		ERef<Iterator> itRef = a->getIterator();
		int nr = 0;
		while(!itRef->end()) {
			ObjRef valueRef = itRef->value();
			ObjRef keyRef = itRef->key();
			if(nr++>0)std::cout << ",\n";
			if(!valueRef.isNull()) {
				for(int i = 0;i<level;++i)
					std::cout << "\t";
				std::cout << "["<<keyRef.toString() <<"] : ";
				print_r(valueRef.get(),maxLevel,level+1);

			}
			itRef->next();
		}
		std::cout << "\n";
		for(int i = 0;i<level-1;++i)
			std::cout << "\t";
		std::cout << "]";
	} else if(Map * m = dynamic_cast<Map *>(o)) {
		std::cout << "{\n";
		ERef<Iterator> itRef = m->getIterator();
		int nr = 0;
		while(!itRef->end()) {
			ObjRef valueRef = itRef->value();
			ObjRef keyRef = itRef->key();
			if(nr++>0)
				std::cout << ",\n";
			if(!valueRef.isNull()) {
				for(int i = 0;i<level;++i)
					std::cout << "\t";
				std::cout << "["<<keyRef.toString() <<"] : ";
				print_r(valueRef.get(),maxLevel,level+1);

			}

			itRef->next();
		}
		std::cout << "\n";
		for(int i = 0;i<level-1;++i)
			std::cout << "\t";
		std::cout << "}";
	} else {
		if(dynamic_cast<String *>(o))
			std::cout << "\""<<o->toString()<<"\"";
		else std::cout << o->toString();
	}
	std::cout.flush();
}

/*! Tries to locate the given __filename__ with the current searchPath set in the runtime.
	@return the path to the file or the original __filename__ if the file could not be found.	*/
static std::string findFile(Runtime & runtime, const std::string & filename){
	static const StringId seachPathsId("__searchPaths");

	std::string file(IO::condensePath(filename));
	if( IO::getEntryType(file)!=IO::TYPE_FILE ){
		if(Array * searchPaths = dynamic_cast<Array*>(runtime.getAttribute(seachPathsId).getValue())){
			for(ERef<Iterator> itRef = searchPaths->getIterator();!itRef->end();itRef->next()){
				ObjRef valueRef = itRef->value();
				std::string s(IO::condensePath(valueRef.toString()+'/'+filename));
				if( IO::getEntryType(s)==IO::TYPE_FILE ){
					file = s;
					break;
				}
			}
		}
	}
	return file;
}

//! (static)
ObjRef StdLib::loadOnce(Runtime & runtime,const std::string & filename){
	static const StringId mapId("__loadOnce_loadedFiles");

	std::string condensedFilename( IO::condensePath(findFile(runtime,filename)) );
	Map * m = dynamic_cast<Map*>(runtime.getAttribute(mapId).getValue());
	if(m==nullptr){
		m = Map::create();
		runtime.setAttribute(mapId, Attribute(m));
	}
	ObjRef obj = m->getValue(condensedFilename);
	if(obj.toBool()){ // already loaded?
		return nullptr;
	}
	m->setValue(create(condensedFilename), create(true));
	return _loadAndExecute(runtime,condensedFilename);
}

#if defined(_WIN32)
static LARGE_INTEGER _getPerformanceCounter();

// execute this as soon as possible (when the global static variables are initialized)
static LARGE_INTEGER _clockStart = _getPerformanceCounter();

// wrapper for the windows high performance timer.
LARGE_INTEGER _getPerformanceCounter(){
	LARGE_INTEGER c;
	QueryPerformanceCounter(&c);
	return c;
}

#endif

// -------------------------------------------------------------
//! init  (globals)
void StdLib::init(EScript::Namespace * globals) {

	/*!	[ESF] void addSearchPath(path)
		Adds a search path which is used for load(...) and loadOnce(...)	*/
	ES_FUNCTION(globals,"addSearchPath",1,1,{
		static const StringId seachPathsId("__searchPaths");
		Array * searchPaths = dynamic_cast<Array*>(rt.getAttribute(seachPathsId).getValue());
		if(searchPaths == nullptr){
			searchPaths = Array::create();
			rt.setAttribute(seachPathsId, Attribute(searchPaths));
		}
		searchPaths->pushBack(String::create(parameter[0].toString()));
		return nullptr;
	})

	//! [ESF] void assert( expression[,text])
	ES_FUNCTION(globals,"assert",1,2, {
		if(!parameter[0].toBool()){
			rt.setException(parameter.count()>1?parameter[1].toString():"Assert failed.");
		}
		return nullptr;
	})

	//! [ESF]  string chr(number)         UNICODE_TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	ES_FUN(globals,"chr",1,1,std::string(1, static_cast<char>(parameter[0].to<int>(rt))))

	// clock
	{
	#if defined(_WIN32)
	typedef LARGE_INTEGER timer_t;
	static timer_t frequency;
	if(!QueryPerformanceFrequency(&frequency)) {
		std::cout <<("QueryPerformanceFrequency failed, timer will not work properly!");
	}

	//! [ESF]  number clock()
	ES_FUNCTION(globals,"clock",0,0,{
		LARGE_INTEGER time = _getPerformanceCounter();
		return static_cast<double>(time.QuadPart-_clockStart.QuadPart) / static_cast<double>(frequency.QuadPart);
	})

	#else
	//! [ESF]  number clock()
	ES_FUN(globals,"clock",0,0,static_cast<double>(clock())/CLOCKS_PER_SEC)
	#endif
	}

	//!	[ESF]  Object eval(string)
	ES_FUN(globals,"eval",1,1,
				_eval(rt,CodeFragment(Consts::FILENAME_INLINE, StringData(parameter[0].toString()))))

	/*!	[ESF]  Map getDate([time])
		like http://de3.php.net/manual/de/function.getdate.php	*/
	ES_FUNCTION(globals,"getDate",0,1,{
		time_t t=(parameter.count()==0)?time(nullptr):static_cast<time_t>(parameter[0].to<int>(rt));
		tm *d = localtime (& t );
		Map * m = Map::create();
		m->setValue(create("seconds"),	create(d->tm_sec));
		m->setValue(create("minutes"),	create(d->tm_min));
		m->setValue(create("hours"),	create(d->tm_hour));
		m->setValue(create("mday"),		create(d->tm_mday));
		m->setValue(create("mon"),		create(d->tm_mon+1));
		m->setValue(create("year"),		create(d->tm_year+1900));
		m->setValue(create("wday"),		create(d->tm_wday));
		m->setValue(create("yday"),		create(d->tm_yday));
		m->setValue(create("isdst"),	create(d->tm_isdst));
		return m;
	})

	//! [ESF] string|void getEnv(String)
	ES_FUNCTION(globals,"getEnv",1,1,{
		const char * value = std::getenv(parameter[0].toString().c_str());
		if(value==nullptr)
			return nullptr;
		return std::string(value);
	})

	//! [ESF]  string getOS()
	ES_FUN(globals,"getOS",0,0,StdLib::getOS())

	//! [ESF] Runtime getRuntime( )
	ES_FUN(globals,"getRuntime",0,0, &rt)

	//!	[ESF] mixed load(string filename)
	ES_FUN(globals,"load",1,1,_loadAndExecute(rt,findFile(rt,parameter[0].toString())))

	//!	[ESF] mixed loadOnce(string filename)
	ES_FUN(globals,"loadOnce",1,1,StdLib::loadOnce(rt,parameter[0].toString()))

	//! [ESF]  Number ord(String)                  UNICODE_TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	ES_FUN(globals,"ord",1,1,static_cast<int>(parameter[0].toString().c_str()[0] ))

	//! [ESF] void out(...)
	ES_FUNCTION(globals,"out",0,-1, {
		for(const auto & param : parameter) {
			std::cout << param.toString();
		}
		std::cout.flush();
		return nullptr;
	})

	//! [ESF] void outln(...)
	ES_FUNCTION(globals,"outln",0,-1, {
		for(const auto & param : parameter) {
			std::cout << param.toString();
		}
		std::cout << std::endl;
		return nullptr;
	})

	//!	[ESF]  BlockStatement parse(string) @deprecated
	ES_FUNCTION(globals,"parse",1,1, {
		ERef<UserFunction> script;

		Compiler compiler(rt.getLogger());
		script = compiler.compile(CodeFragment(Consts::FILENAME_INLINE, StringData(parameter[0].toString())));

		return script.detachAndDecrease();
	})
	//! [ESF]  obj parseJSON(string)
	ES_FUN(globals,"parseJSON",1,1,JSON::parseJSON(parameter[0].toString()))

	//! [ESF] void print_r(...)
	ES_FUNCTION(globals,"print_r",0,-1, {
		std::cout << "\n";
		for(const auto & param : parameter) {
			if(!param.isNull()) {
				print_r(param.get());
			}
		}
		return nullptr;
	})

	//!	[ESF]  number system(command)
	ES_FUN(globals,"system",1,1,system(parameter[0].toString().c_str()))

	//!	[ESF] Number exec(String path, Array argv)
	ES_FUNCTION(globals, "exec", 2, 2, {
		Array * array = assertType<Array>(rt, parameter[1]);
		uint32_t argc = array->size();

		char ** argv  = new char *[argc + 1];
		for(uint_fast32_t i = 0; i < argc; ++i) {
			 std::string arg = array->get(i)->toString();
			 argv[i] = new char[arg.length() + 1];
			 std::copy(arg.begin(), arg.end(), argv[i]);
			 argv[i][arg.length()] = '\0';
		}
		argv[argc] = nullptr;

		Number * result = create(execv(parameter[0].toString().c_str(), argv));

		for(uint_fast32_t i = 0; i < argc; ++i) {
			delete [] argv[i];
		}
		delete [] argv;

		return result;
	})

	//! [ESF]  number time()
	ES_FUN(globals,"time",0,0,static_cast<double>(time(nullptr)))

	//! [ESF]  string toJSON(obj[,formatted = true])
	ES_FUN(globals,"toJSON",1,2,JSON::toJSON(parameter[0].get(),parameter[1].toBool(true)))

}


}
