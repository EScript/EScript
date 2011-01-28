#include "StdLib.h"

//#include "../Utils/LoadLib.h"
#include "../EScript/EScript.h"
#include "../EScript/Parser/Parser.h"
#include "../EScript/Utils/FileUtils.h"
#include "ext/JSON.h"

#include <sstream>
#include <stdlib.h>
#include <ctime>
using namespace EScript;

/*!	[ESF] void out(...)	*/
ES_FUNCTION(esf_out) {
    for(ParameterValues::const_iterator it=parameter.begin();it!=parameter.end();++it)
        std::cout << (*it).toString();
    std::cout.flush();
    return NULL;
}


/*!	[ESF] void assert( expression[,text])	*/
ES_FUNCTION(esf_assert) {
    assertParamCount(runtime,parameter.count(),1,2);
    if(!parameter[0]->toBool()){
        runtime.error(parameter.count()>1?parameter[1]->toString():"Assert failed.");
    }
    return NULL;
}

/*!	(internal)	*/
static void print_r(Object * o,int level) {
    if (!o) return;
    if (level>7) {
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
                print_r(valueRef.get(),level+1);

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
                print_r(valueRef.get(),level+1);

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

/*!	[ESF] void print_r(...)	*/
ES_FUNCTION(esf_print_r) {
    std::cout << "\n";
    for(ParameterValues::const_iterator it=parameter.begin();it!=parameter.end();++it) {
        if (!(*it).isNull())
            print_r((*it).get(),1);
    }
    return NULL;
}

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
    if(runtime.getState() == Runtime::STATE_RETURN){
            resultRef=runtime.getResult();
            runtime.resetState();
            return resultRef.detachAndDecrease();
    }
    return NULL;
}

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
/**
 * [ESF] mixed load(string filename)
 */
ESF(esf_load,1,1,StdLib::load(runtime,parameter[0].toString()))

/**
 * [ESF] mixed loadOnce(string filename)
 */
ESF(esf_loadOnce,1,1,StdLib::loadOnce(runtime,parameter[0].toString()))

/*!	[ESF]  number clock()	*/
ESF(esf_clock,0,0,Number::create( static_cast<double>(clock())/CLOCKS_PER_SEC))

/*!	[ESF]  string chr(number)	*/
ES_FUNCTION(esf_chr) {
    assertParamCount(runtime,parameter.count(),1,1);
    std::ostringstream s;
    s<< static_cast<char>(parameter[0]->toInt());

    return String::create(s.str());
}
/**
 * [ESF]  number system(command)
 * \todo create possibility to disable this
 */
ESF(esf_system,1,1,Number::create(system(parameter[0]->toString().c_str())))

/**
 * [ESF]  Block parse(string)
 * \todo create possibility to disable this
 */
ES_FUNCTION(esf_parse) {
    assertParamCount(runtime,parameter.count(),1,1);
    ERef<Block> bRef=new Block();
    ERef<Parser> pRef=new Parser();

    try{
        pRef->parse(bRef.get(),parameter[0]->toString().c_str());
    }catch(Object * e){
        runtime.setExceptionState(e);
    }

    return bRef.detachAndDecrease();
}

/*!	[ESF]  number time()	*/
ESF(esf_time,0,0,Number::create(time(NULL)))

/**
 * [ESF]  Map getDate([time])
 * like http://de3.php.net/manual/de/function.getdate.php
 */
ES_FUNCTION(esf_getDate) {
    assertParamCount(runtime,parameter.count(),0,1);
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
}

/*!	[ESF]  object _callFunction(fun,obj[,params])	*/
ES_FUNCTION(esf_callFunction) {
    assertParamCount(runtime,parameter.count(),1);

    ObjPtr fun(parameter[0]);
    ObjPtr obj(parameter[1].get());
    ParameterValues params=ParameterValues(parameter.count()>2?parameter.count()-2:0);
    for(unsigned int i=2;i<parameter.count();i++){
        params.set(i-2,parameter[i]);
    }
    ObjRef resultRef=runtime.executeFunction(fun.get(),obj.get(),params);
    return resultRef.detachAndDecrease();
}

/*!	[ESF]  string _toJSON(obj[,formatted=true])	*/
ESF(esf_toJSON,1,2,String::create(JSON::toJSON(parameter[0].get(),parameter[1].toBool(true))))

/*!	[ESF]  obj _parseJSON(string)	*/
ESF(esf_parseJSON,1,1,JSON::parseJSON(parameter[0].toString()))

/*!	[ESF]  string getOS()	*/
ES_FUNCTION(esf_getOS){
    assertParamCount(runtime, parameter.count(), 0, 0);
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
}

/*! [ESF] Runtime getRuntime( ) */
ESF(esf_getRuntime,0,0, &runtime)

/*!	init  (globals)	*/
void StdLib::init(EScript::Namespace * globals) {
    //  Object * lib=new Object();
    declareFunction(globals,"assert",esf_assert);
    declareFunction(globals,"out",esf_out);
    declareFunction(globals,"print_r",esf_print_r);
    declareFunction(globals,"load",esf_load);
    declareFunction(globals,"loadOnce",esf_loadOnce);
    declareFunction(globals,"clock",esf_clock);
    declareFunction(globals,"chr",esf_chr);
    declareFunction(globals,"system",esf_system);
    declareFunction(globals,"parse",esf_parse);
    declareFunction(globals,"time",esf_time);
    declareFunction(globals,"getDate",esf_getDate);
    declareFunction(globals,"_callFunction",esf_callFunction); /// @deprecated
    declareFunction(globals,"_toJSON",esf_toJSON); /// @deprecated
    declareFunction(globals,"toJSON",esf_toJSON);
    declareFunction(globals,"_parseJSON",esf_parseJSON);/// @deprecated
    declareFunction(globals,"parseJSON",esf_parseJSON);
    declareFunction(globals,"getOS",esf_getOS);
    declareFunction(globals,"getRuntime",esf_getRuntime);

}

