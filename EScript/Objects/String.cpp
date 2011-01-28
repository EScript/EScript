#include "String.h"
#include "../EScript.h"

#include "../Utils/StringUtils.h"

#include <sstream>

using namespace EScript;
//---

using std::string;

std::stack<String *> String::stringPool;

/*!	[ESMF] String new String((String)Obj)	*/
ES_FUNCTION(esmf_String_constructor) {
    assertParamCount(runtime,parameter,0,1);
    return  String::create(parameter[0].toString(""));
}

//- Operators

/*!	[ESMF] String String+(String)Obj	*/
ESF(esmf_String_plus,1,1,String::create( caller->toString() + parameter[0]->toString()))

/*!	[ESMF] String String*(Number)Obj	*/
ES_FUNCTION(esmf_String_mul) {
    assertParamCount(runtime,parameter,1,1);
    string s="";
    string s2=caller->toString();
    for (int i=parameter[0].toInt();i>0;i--)
        s+=s2;
    return  String::create(s);
}

/*!	[ESMF] self String+=(String)Obj	*/
ESF(esmf_String_plusAssign,1,1,(assertType<String>(runtime,caller)->appendString(parameter[0].toString()),caller))

/*!	[ESMF] self String*=(Number)Obj	*/
ES_FUNCTION(esmf_String_mulAssign) {
    assertParamCount(runtime,parameter,1,1);
    String * strObj=assertType<String>(runtime,caller);
    string s="";
    string s2=caller->toString();
    for (int i=parameter[0]->toInt();i>0;i--)
        s+=s2;
    strObj->setString(s);
    return  caller;
}

/*!	[ESMF] bool String>(String)Obj	*/
ESF(esmf_String_greater,1,1,Bool::create( caller->toString() > parameter[0].toString()))

/*!	[ESMF] bool String>=(String)Obj	*/
ESF(esmf_String_greaterEqual,1,1,Bool::create( caller->toString() >= parameter[0].toString()))

/*!	[ESMF] bool String<(String)Obj	*/
ESF(esmf_String_less,1,1,Bool::create( caller->toString() < parameter[0].toString()))

/*!	[ESMF] bool String<=(String)Obj	*/
ESF(esmf_String_lessEqual,1,1,Bool::create( caller->toString() <= parameter[0].toString()))

// ---

/*!	[ESMF] Number String.length()	*/
ESF(esmf_String_length,0,0,Number::create( caller->toString().length()))

/*!	[ESMF] String String.trim()	*/
ESF(esmf_String_trim,0,0,String::create( StringUtils::trim(caller->toString())))

/*!	[ESMF] String String.substr( (Number)begin [,(Number)length] )	*/
ES_FUNCTION(esmf_String_substr) {
    assertParamCount(runtime,parameter,1,2);
    int start=parameter[0].toInt();
    int length=caller->toString().length();
    if (start>=length) return String::create("");
    if (start<0) start=length+start;
    if (start<0) start=0;
    int count=length-start;
    if (parameter.count()>1) {
        int i=parameter[1].toInt();
        if (i<count) {
            if (i<0) {
                count+=i;
            } else {
                count=i;
            }
        }
    }
    return  String::create(caller->toString().substr(start,count));
}

/*!	[ESMF] String String.get( (Number)position )	*/
ES_FUNCTION(esmf_String_get) {
    assertParamCount(runtime,parameter.count(),1,1);
    int pos=parameter[0]->toInt();
    if (static_cast<unsigned int>(pos)>=caller->toString().length()) return NULL; // throw error?
    return  String::create(caller->toString().substr(pos,1));
}

/*!	[ESMF] Number|false String.find( (String)search [,(Number)startIndex] )	*/
ES_FUNCTION(esmf_String_find) {
    assertParamCount(runtime,parameter,1,2);
    string s=caller->toString();
    string search=parameter[0].toString();
    size_t start=0;
    if (parameter.count()>1) {
        start=static_cast<size_t>(parameter[1].toInt());
        if (start>=s.length())
            return Bool::create(false);
    }
    size_t pos=s.find(search,start);
    if (pos==string::npos ) {
        return Bool::create(false);
    } else return Number::create(pos);
}

/*!	[ESMF] Number|false String.rfind( (String)search [,(Number)startIndex] )	*/
ES_FUNCTION(esmf_String_rfind) {
    assertParamCount(runtime,parameter.count(),1,2);
    string s=caller->toString();
    string search=parameter[0].toString();
    size_t start=s.length();
    if (parameter.count()>1) {
        start=static_cast<size_t>(parameter[1].toInt());
        if (start>=s.length())
            start=s.length();
        //std::cout << " #"<<start<< " ";
    }
    size_t pos=s.rfind(search,start);
    if (pos==string::npos ) {
        return Bool::create(false);
    } else return Number::create(pos);
}

/*!	[ESMF] Bool String.contains (String)search [,(Number)startIndex] )	*/
ES_FUNCTION(esmf_String_contains) {
    assertParamCount(runtime,parameter.count(),1,2);
    string s=caller->toString();
    string search=parameter[0]->toString();
    size_t start=s.length();
    if (parameter.count()>1) {
        start=static_cast<size_t>(parameter[1].toInt());
        if (start>=s.length())
            start=s.length();
    }

    return Bool::create(s.rfind(search,start)!=string::npos);
}

/*!	[ESMF] String String.replace((String)search,(String)replace)	*/
ES_FUNCTION(esmf_String_replace) {
    assertParamCount(runtime,parameter.count(),2,2);
    string subject=caller->toString();

    string search=parameter[0]->toString();
    string replace=parameter[1]->toString();
    return String::create(StringUtils::replaceAll(subject,search,replace,1));
}

/*!	[ESMF] String.replaceAll( (Map | ((String)search,(String)replace)) [,(Number)max])	*/
ES_FUNCTION(esmf_String_replaceAll) {
    assertParamCount(runtime,parameter.count(),1,3);
    string subject=caller->toString();

    //Map * m
    if ( Map * m=parameter[0].toType<Map>()) {
        assertParamCount(runtime,parameter.count(),1,2);
        int i=m->count();
        string * searches =new string[i];
        string * replaces =new string[i];

        ERef<Iterator> iRef=m->getIterator();
        int i2=0;
        while (!iRef->end()) {
            ObjRef key=iRef->key();
            ObjRef value=iRef->value();
            searches[i2]=key.toString();
            replaces[i2]=value.toString();
            i2++;
            iRef->next();
        }

        String * s=String::create(StringUtils::replaceMultiple(subject,i,searches,replaces,parameter[1].toInt(-1)));
        delete [] searches;
        delete [] replaces;
        return s;
    }

    string search=parameter[0]->toString();
    string replace=parameter[1]->toString();

    return String::create(StringUtils::replaceAll(subject,search,replace,parameter[2].toInt(-1)));
}

/*!	[ESMF] Array String.split((String)search[,(Number)max])	*/
ES_FUNCTION(esmf_String_split) {
    assertParamCount(runtime,parameter.count(),1,2);
    
    std::vector<std::string> result;
    StringUtils::split( caller->toString(), parameter[0].toString(), result, parameter[1].toInt(-1) );
		
    Array * a=Array::create();
    for(std::vector<std::string>::const_iterator it=result.begin();it!=result.end();++it){
		a->pushBack( String::create(*it) );
    }
		
    return a;
}

/*!	[ESMF] String String.fillUp(length[, string fill=" ")	*/
ES_FUNCTION(esmf_String_fillUp) {
    assertParamCount(runtime,parameter.count(),1,2);
    string s=caller->toString();
    std::ostringstream sprinter;
    sprinter<<s;
    string fill=parameter[1].toString(" ");
    int count=(parameter[0].toInt()-s.length())/(fill.length()>0?fill.length():0);
    for(int i=0;i<count;++i)
        sprinter<<fill;
    return String::create(sprinter.str());
}

/*!	[ESMF] Bool String.beginsWith( (String)search )	*/
ES_FUNCTION(esmf_String_beginsWith) {
    assertParamCount(runtime,parameter.count(),1,1);
    string s=caller->toString();
    string search=parameter[0]->toString();
    if(s.length()<search.length()) return Bool::create(false);
    return Bool::create(s.substr(0,search.length())==search);
}

/*!	[ESMF] Bool String.endsWith( (String)search )	*/
ES_FUNCTION(esmf_String_endsWith) {
    assertParamCount(runtime,parameter.count(),1,1);
    string s=caller->toString();
    string search=parameter[0]->toString();
    if(s.length()<search.length()) return Bool::create(false);
    return Bool::create(s.substr(s.length()-search.length(),search.length())==search);
}
//---
Type* String::typeObject=NULL;

/*!	initMembers	*/
void String::init(EScript::Namespace & globals) {
//
    // String ---|> [Object]
    typeObject=new Type(Object::getTypeObject());
	typeObject->setFlag(Type::FLAG_CALL_BY_VALUE,true);

    declareConstant(&globals,getClassName(),typeObject);
//
    declareFunction(typeObject,"_constructor",esmf_String_constructor);
    declareFunction(typeObject,"+",esmf_String_plus);
    declareFunction(typeObject,"*",esmf_String_mul);
    declareFunction(typeObject,"+=",esmf_String_plusAssign);
    declareFunction(typeObject,"*=",esmf_String_mulAssign);
//
    declareFunction(typeObject,">",esmf_String_greater);
    declareFunction(typeObject,">=",esmf_String_greaterEqual);
    declareFunction(typeObject,"<",esmf_String_less);
    declareFunction(typeObject,"<=",esmf_String_lessEqual);
//    declareFunction(typeObject,"===",esmf_String_identical);
//    declareFunction(typeObject,"!==",esmf_String_notIdentical);

    declareFunction(typeObject,"length",esmf_String_length);
    declareFunction(typeObject,"trim",esmf_String_trim);
    declareFunction(typeObject,"substr",esmf_String_substr);
    declareFunction(typeObject,"_get",esmf_String_get);
    declareFunction(typeObject,"find",esmf_String_find);
    declareFunction(typeObject,"contains",esmf_String_contains);
    declareFunction(typeObject,"rfind",esmf_String_rfind);
    declareFunction(typeObject,"replace",esmf_String_replace);
    declareFunction(typeObject,"replaceAll",esmf_String_replaceAll);
    declareFunction(typeObject,"split",esmf_String_split);
    declareFunction(typeObject,"fillUp",esmf_String_fillUp);

    declareFunction(typeObject,"beginsWith",esmf_String_beginsWith);
    declareFunction(typeObject,"endsWith",esmf_String_endsWith);
	//FLAG_CALL_BY_VALUE
}

//---

String * String::create(const string & s){
    #ifdef ES_DEBUG_MEMORY
    return new String(s);
    #endif
    if(stringPool.empty()){
        return new String (s);
    }else{
        String * o=stringPool.top();
        stringPool.pop();
        o->setString(s);
        return o;
    }
}
String * String::create(const string & s,Type * type){
    #ifdef ES_DEBUG_MEMORY
    return new String(s,type);
    #endif
    if(type==typeObject){
        return create(s);
    }else{
        return new String(s,type);
    }
}
void String::release(String * o){
    #ifdef ES_DEBUG_MEMORY
    delete o;
    return;
    #endif
    if(o->getType()!=typeObject){
        delete o;
        std::cout << "Found diff StringType\n";
    }else{
       stringPool.push(o);
    }
}
//---

/*!	(ctor)	*/
String::String(const string & _s,Type * type):
        Object(type?type:typeObject),s(_s) {
    //ctor
}

/*!	(dtor)	*/
String::~String() {
    //dtor
}

/*!	---|> [Object]	*/
Object * String::clone() const {
    return String::create(s,getType());
}

/*!	---|> [Object]	*/
string String::toString()const {
    return s;
}

/*!	---|> [Object]	*/
std::string String::toDbgString()const{
	return std::string("\"")+s+"\"";
}

/*!	---|> [Object]	*/
double String::toDouble()const {
    int to=0;
    return StringUtils::getNumber(s.c_str(),to, true);
}

/*!	---|> [Object]	*/
int String::toInt()const {
    int to=0;
    return static_cast<int>(StringUtils::getNumber(s.c_str(),to,  true));
}

/*!	---|> [Object]	*/
bool String::toBool()const {
    return true;//s.length()>0;
}

/*!	---|> [Object]	*/
bool String::rt_isEqual(Runtime &, const ObjPtr o){
    return o.isNull()?false:s==o.toString();
}
