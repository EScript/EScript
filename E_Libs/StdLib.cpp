// StdLib.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "StdLib.h"

#include "../EScript/EScript.h"
#include "../EScript/Parser/Parser.h"
#include "../EScript/Utils/FileUtils.h"
#include "ext/JSON.h"

#include <sstream>
#include <stdlib.h>
#include <ctime>
using namespace EScript;

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

//! (static)
Object * StdLib::load(Runtime & runtime,const std::string & filename){
	ERef<Block> bRef(new Block());
	try {
		bRef=EScript::loadScriptFile(filename,bRef.get());
	} catch (Exception * e) {
		runtime.setExceptionState(e);
	}
	if (bRef.isNull())
		return NULL;

	ObjRef resultRef(runtime.executeObj(bRef.get()));
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

	std::string condensedFilename( FileUtils::condensePath(filename) );
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

// -------------------------------------------------------------

//! init  (globals)
void StdLib::init(EScript::Namespace * globals) {

	//! [ESF] void assert( expression[,text])
	ES_FUNCTION_DECLARE(globals,"assert",1,2, {
		assertParamCount(runtime,parameter.count(),1,2);
		if(!parameter[0]->toBool()){
			runtime.error(parameter.count()>1?parameter[1]->toString():"Assert failed.");
		}
		return NULL;
	})

	//! [ESF]  string chr(number)
	ES_FUNCTION_DECLARE(globals,"chr",1,1,{
		std::ostringstream s;
		s<< static_cast<char>(parameter[0]->toInt());
		return String::create(s.str());
	})

	//! [ESF]  number clock()
	ESF_DECLARE(globals,"clock",0,0,Number::create( static_cast<double>(clock())/CLOCKS_PER_SEC))

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
	ES_FUNCTION_DECLARE(globals,"getOS",0,0,{
		#if defined(_WIN32) || defined(_WIN64)
		return String::create("WINDOWS");
		#elif defined(__APPLE__)
		return String::create("MAC OS");
		#elif defined(__linux__)
		return String::create("LINUX");
		#elif defined(__unix__)
		return String::create("UNIX");
		#else
		return String::create("UNKNOWN");
		#endif
	})

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
		ERef<Block> bRef(new Block());
		ERef<Parser> pRef(new Parser());

		try{
			pRef->parse(bRef.get(),parameter[0]->toString().c_str());
		}catch(Object * e){
			runtime.setExceptionState(e);
		}
		return bRef.detachAndDecrease();
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

	//! [ESF]  number time()
	ESF_DECLARE(globals,"time",0,0,Number::create(time(NULL)))

	//! [ESF]  string toJSON(obj[,formatted=true])
	ESF_DECLARE(globals,"toJSON",1,2,String::create(JSON::toJSON(parameter[0].get(),parameter[1].toBool(true))))


	// --- internals and experimental functions

	//! [ESF]  object _callFunction(fun,obj[,params*])
	ES_FUNCTION_DECLARE(globals,"_callFunction",1,-1, {
		ObjPtr fun(parameter[0]);
		ObjPtr obj(parameter[1].get());
		ParameterValues params=ParameterValues(parameter.count()>2?parameter.count()-2:0);
		for(unsigned int i=2;i<parameter.count();i++){
			params.set(i-2,parameter[i]);
		}
		ObjRef resultRef=runtime.executeFunction(fun.get(),obj.get(),params);
		return resultRef.detachAndDecrease();
	})
}
