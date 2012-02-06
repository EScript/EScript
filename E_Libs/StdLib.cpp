// StdLib.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "StdLib.h"

#include "../EScript/EScript.h"
#include "../EScript/Parser/Parser.h"
#include "../EScript/Utils/IO/IO.h"
#include "ext/JSON.h"

#include <sstream>
#include <stdlib.h>
#include <ctime>
#include <unistd.h>

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
	if (!o) return;
	if (level>maxLevel) {
		std::cout << " ... \n";
		return;
	}

	if (Array * a=dynamic_cast<Array *>(o)) {
		std::cout << "[\n";
		ERef<Iterator> itRef=a->getIterator();
		int nr=0;
		while (!itRef->end()) {
			ObjRef valueRef=itRef->value();
			ObjRef keyRef=itRef->key();
			if (nr++>0)std::cout << ",\n";
			if (!valueRef.isNull()) {
				for (int i=0;i<level;i++)
					std::cout << "\t";
				std::cout << "["<<keyRef.toString() <<"] : ";
				print_r(valueRef.get(),maxLevel,level+1);

			}
			itRef->next();
		}
		std::cout << "\n";
		for (int i=0;i<level-1;i++)
			std::cout << "\t";
		std::cout << "]";
	} else if (Map * m=dynamic_cast<Map *>(o)) {
		std::cout << "{\n";
		ERef<Iterator> itRef=m->getIterator();
		int nr=0;
		while (!itRef->end()) {
			ObjRef valueRef=itRef->value();
			ObjRef keyRef=itRef->key();
			if (nr++>0)
				std::cout << ",\n";
			if (!valueRef.isNull()) {
				for (int i=0;i<level;i++)
					std::cout << "\t";
				std::cout << "["<<keyRef.toString() <<"] : ";
				print_r(valueRef.get(),maxLevel,level+1);

			}

			itRef->next();
		}
		std::cout << "\n";
		for (int i=0;i<level-1;i++)
			std::cout << "\t";
		std::cout << "}";
	} else {
		if (dynamic_cast<String *>(o))
			std::cout << "\""<<o->toString()<<"\"";
		else std::cout << o->toString();
	}
}

/*! Tries to locate the given __filename__ with the current searchPath set in the runtime.
	@return the path to the file or the original __filename__ if the file could not be found.	*/
static std::string findFile(Runtime & runtime, const std::string & filename){
	static const identifierId seachPathsId=stringToIdentifierId("__searchPaths");

	std::string file(IO::condensePath(filename));
	if( IO::getEntryType(file)!=IO::TYPE_FILE ){
		if(Array * searchPaths = dynamic_cast<Array*>(runtime.getAttribute(seachPathsId))){
			for(ERef<Iterator> itRef=searchPaths->getIterator();!itRef->end();itRef->next()){
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
Object * StdLib::load(Runtime & runtime,const std::string & filename){
	ERef<Block> block;
	try {
		block=EScript::loadScriptFile(findFile(runtime,filename));
	} catch (Exception * e) {
		runtime.setException(e); // adds stack info
		return NULL;
	}
	if (block.isNull())
		return NULL;

	ObjRef resultRef(runtime.executeObj(block.get()));
	/* reset the Block at this point is important as it might hold a reference to the result, which may then
		be destroyed when the function is left after the resultRef-reference has already been decreased. */
	block = NULL;
	if(runtime.getState() == Runtime::STATE_RETURNING){
			resultRef=runtime.getResult();
			runtime.resetState();
			return resultRef.detachAndDecrease();
	}
	return NULL;
}

//! (static)
Object * StdLib::loadOnce(Runtime & runtime,const std::string & filename){
	static const identifierId mapId=stringToIdentifierId("__loadOnce_loadedFiles");

	std::string condensedFilename( IO::condensePath(findFile(runtime,filename)) );
	Map * m=dynamic_cast<Map*>(runtime.getAttribute(mapId));
	if(m==NULL){
		m=Map::create();
		runtime.setObjAttribute(mapId,m);
	}
	ObjRef obj=m->getValue(condensedFilename);
	if(obj.toBool()){ // already loaded?
		return NULL;
	}
	m->setValue(String::create(condensedFilename),Bool::create(true));
	return load(runtime,condensedFilename);
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
	ES_FUNCTION_DECLARE(globals,"addSearchPath",1,1,{
		static const identifierId seachPathsId=stringToIdentifierId("__searchPaths");
		Array * searchPaths = dynamic_cast<Array*>(runtime.getAttribute(seachPathsId));
		if(searchPaths == NULL){
			searchPaths = Array::create();
			runtime.setObjAttribute(seachPathsId,searchPaths);
		}
		searchPaths->pushBack(String::create(parameter[0].toString()));
		return Void::get();
	})

	//! [ESF] void assert( expression[,text])
	ES_FUNCTION_DECLARE(globals,"assert",1,2, {
		assertParamCount(runtime,parameter.count(),1,2);
		if(!parameter[0]->toBool()){
			runtime.setException(parameter.count()>1?parameter[1]->toString():"Assert failed.");
		}
		return NULL;
	})

	//! [ESF]  string chr(number)
	ES_FUNCTION_DECLARE(globals,"chr",1,1,{
		std::ostringstream s;
		s<< static_cast<char>(parameter[0]->toInt());
		return String::create(s.str());
	})


	// clock
	{
	#if defined(_WIN32)
	typedef LARGE_INTEGER timer_t;
	static timer_t frequency;
	if(!QueryPerformanceFrequency(&frequency)) {
		std::cout <<("QueryPerformanceFrequency failed, timer will not work properly!");
	}

	//! [ESF]  number clock()
	ES_FUNCTION_DECLARE(globals,"clock",0,0,{
		LARGE_INTEGER time = _getPerformanceCounter();
		return Number::create( static_cast<double>(time.QuadPart-_clockStart.QuadPart) / static_cast<double>(frequency.QuadPart) );
	})

	#else
	//! [ESF]  number clock()
	ESF_DECLARE(globals,"clock",0,0,Number::create( static_cast<double>(clock())/CLOCKS_PER_SEC))
	#endif
	}

	/*!	[ESF]  Map getDate([time])
		like http://de3.php.net/manual/de/function.getdate.php	*/
	ES_FUNCTION_DECLARE(globals,"getDate",0,1,{
		time_t t=(parameter.count()==0)?time(0):static_cast<time_t>(parameter[0]->toInt());
		tm *d=localtime (& t );
		Map * m=Map::create();
		m->setValue(String::create("seconds"),Number::create(d->tm_sec));
		m->setValue(String::create("minutes"),Number::create(d->tm_min));
		m->setValue(String::create("hours"),Number::create(d->tm_hour));
		m->setValue(String::create("mday"),Number::create(d->tm_mday));
		m->setValue(String::create("mon"),Number::create(d->tm_mon+1));
		m->setValue(String::create("year"),Number::create(d->tm_year+1900));
		m->setValue(String::create("wday"),Number::create(d->tm_wday));
		m->setValue(String::create("yday"),Number::create(d->tm_yday));
		m->setValue(String::create("isdst"),Number::create(d->tm_isdst));
		return m;
	})

	//! [ESF]  string getOS()
	ESF_DECLARE(globals,"getOS",0,0,String::create(StdLib::getOS()))

	//! [ESF] Runtime getRuntime( )
	ESF_DECLARE(globals,"getRuntime",0,0, &runtime)

	//!	[ESF] mixed load(string filename)
	ESF_DECLARE(globals,"load",1,1,StdLib::load(runtime,parameter[0].toString()))

	//!	[ESF] mixed loadOnce(string filename)
	ESF_DECLARE(globals,"loadOnce",1,1,StdLib::loadOnce(runtime,parameter[0].toString()))

	//! [ESF] void out(...)
	ES_FUNCTION_DECLARE(globals,"out",0,-1, {
		for(ParameterValues::const_iterator it=parameter.begin();it!=parameter.end();++it)
			std::cout << (*it).toString();
		std::cout.flush();
		return NULL;
	})

	//!	[ESF]  Block parse(string)
	ES_FUNCTION_DECLARE(globals,"parse",1,1, {
		assertParamCount(runtime,parameter.count(),1,1);
		ERef<Block> block(new Block());
		static const identifierId inline_id = stringToIdentifierId("[inline]");
		block->setFilename(inline_id);
		try{
			Parser p;
			p.parse(block.get(),StringData(parameter[0]->toString()));
		}catch(Exception * e){
			runtime.setException(e); // adds stack info
			return NULL;
		}
		return block.detachAndDecrease();
	})

	//! [ESF]  obj parseJSON(string)
	ESF_DECLARE(globals,"parseJSON",1,1,JSON::parseJSON(parameter[0].toString()))

	//! [ESF] void print_r(...)
	ES_FUNCTION_DECLARE(globals,"print_r",0,-1, {
		std::cout << "\n";
		for(ParameterValues::const_iterator it=parameter.begin();it!=parameter.end();++it) {
			if (!(*it).isNull())
				print_r((*it).get());
		}
		return NULL;
	})

	//!	[ESF]  number system(command)
	ESF_DECLARE(globals,"system",1,1,Number::create(system(parameter[0]->toString().c_str())))

	//!	[ESF] Number exec(String path, Array argv)
	ES_FUNCTION_DECLARE(globals, "exec", 2, 2, {
		Array * array = assertType<Array>(runtime, parameter[1]);
		uint32_t argc = array->size();

		char ** argv  = new char *[argc + 1];
		for(uint_fast32_t i = 0; i < argc; ++i) {
			 std::string arg = array->get(i)->toString();
			 argv[i] = new char[arg.length() + 1];
			 std::copy(arg.begin(), arg.end(), argv[i]);
			 argv[i][arg.length()] = '\0';
		}
		argv[argc] = NULL;

		Number * result = Number::create(execv(parameter[0]->toString().c_str(), argv));

		for(uint_fast32_t i = 0; i < argc; ++i) {
			delete [] argv[i];
		}
		delete [] argv;

		return result;
	})

	//! [ESF]  number time()
	ESF_DECLARE(globals,"time",0,0,Number::create(static_cast<double>(time(NULL))))

	//! [ESF]  string toJSON(obj[,formatted=true])
	ESF_DECLARE(globals,"toJSON",1,2,String::create(JSON::toJSON(parameter[0].get(),parameter[1].toBool(true))))

}


}
