// String.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "String.h"
#include "../EScript.h"

#include "../Utils/StringUtils.h"

#include <sstream>

using namespace EScript;
//---

using std::string;

std::stack<String *> String::stringPool;


//---
Type* String::typeObject=NULL;

//! initMembers
void String::init(EScript::Namespace & globals) {
//
	// String ---|> [Object]
	typeObject=new Type(Object::getTypeObject());
	typeObject->setFlag(Type::FLAG_CALL_BY_VALUE,true);

	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] String new String((String)Obj)
	ESF_DECLARE(typeObject,"_constructor",0,1,String::create(parameter[0].toString("")))

	//- Operators

	//! [ESMF] String String+(String)Obj
	ESMF_DECLARE(typeObject,String,"+",1,1,String::create( self->getString() + parameter[0]->toString()))

	//! [ESMF] String String*(Number)Obj
	ES_MFUNCTION_DECLARE(typeObject,String,"*",1,1,{
		string s;
		string s2( self->getString() );
		for (int i=parameter[0].toInt();i>0;--i)
			s+=s2;
		return  String::create(s);
	})

	//! [ESMF] self String+=(String)Obj
	ESMF_DECLARE(typeObject,String,"+=",1,1,(self->appendString(parameter[0].toString()),caller))

	//! [ESMF] self String*=(Number)Obj
	ES_MFUNCTION_DECLARE(typeObject,String,"*=",1,1,{
		string s;
		string s2( self->getString() );
		for (int i=parameter[0]->toInt();i>0;--i)
			s+=s2;
		self->setString(s);
		return  caller;
	})

	//! [ESMF] bool String>(String)Obj
	ESMF_DECLARE(typeObject,String,">",1,1,Bool::create( self->getString() > parameter[0].toString()))

	//! [ESMF] bool String>=(String)Obj
	ESMF_DECLARE(typeObject,String,">=",1,1,Bool::create( self->getString() >= parameter[0].toString()))

	//! [ESMF] bool String<(String)Obj
	ESMF_DECLARE(typeObject,String,"<",1,1,Bool::create( self->getString() < parameter[0].toString()))

	//! [ESMF] bool String<=(String)Obj
	ESMF_DECLARE(typeObject,String,"<=",1,1,Bool::create( self->getString() <= parameter[0].toString()))

	// ---

	//! [ESMF] Bool String.empty()
	ESMF_DECLARE(typeObject,String,"empty",0,0,Bool::create( self->getString().empty()))

	//! [ESMF] Number String.length()
	ESMF_DECLARE(typeObject,String,"length",0,0,Number::create( self->getString().length()))

	//! [ESMF] String String.ltrim()
	ESMF_DECLARE(typeObject,String,"lTrim",0,0,String::create( StringUtils::lTrim(self->getString())))

	//! [ESMF] String String.trim()
	ESMF_DECLARE(typeObject,String,"trim",0,0,String::create( StringUtils::trim(self->getString())))

	//! [ESMF] String String.rTrim()
	ESMF_DECLARE(typeObject,String,"rTrim",0,0,String::create( StringUtils::rTrim(self->getString())))

	//! [ESMF] String String.substr( (Number)begin [,(Number)length] )
	ES_MFUNCTION_DECLARE(typeObject,String,"substr",1,2, {
		int start=parameter[0].toInt();
		int length=self->getString().length();
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
		return  String::create(self->getString().substr(start,count));
	})

	//! [ESMF] String String[(Number)position ]
	ES_MFUNCTION_DECLARE(typeObject,String,"_get",1,1, {
		assertParamCount(runtime,parameter.count(),1,1);
		int pos=parameter[0]->toInt();
		if (static_cast<unsigned int>(pos)>=self->getString().length())
			return NULL;
		return  String::create(self->getString().substr(pos,1));
	})

	//! [ESMF] Number|false String.find( (String)search [,(Number)startIndex] )
	ES_MFUNCTION_DECLARE(typeObject,String,"find",1,2, {
		const string & s(self->getString());
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
	})

	//! [ESMF] Number|false String.rfind( (String)search [,(Number)startIndex] )
	ES_MFUNCTION_DECLARE(typeObject,String,"rfind",1,2, {
		const string & s(self->getString());
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
	})

	//! [ESMF] Bool String.contains (String)search [,(Number)startIndex] )
	ES_MFUNCTION_DECLARE(typeObject,String,"contains",1,2, {
		const string & s(self->getString());
		string search=parameter[0]->toString();
		size_t start=s.length();
		if (parameter.count()>1) {
			start=static_cast<size_t>(parameter[1].toInt());
			if (start>=s.length())
				start=s.length();
		}
		return Bool::create(s.rfind(search,start)!=string::npos);
	})

	//! [ESMF] String String.replace((String)search,(String)replace)
	ESMF_DECLARE(typeObject,String,"replace",2,2,
				String::create(StringUtils::replaceAll(self->getString(),parameter[0]->toString(),parameter[1]->toString(),1)))

	//! [ESMF] String.replaceAll( (Map | ((String)search,(String)replace)) [,(Number)max])
	ES_MFUNCTION_DECLARE(typeObject,String,"replaceAll",1,3,{
		const string & subject(self->getString());

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
	})

	//! [ESMF] Array String.split((String)search[,(Number)max])
	ES_MFUNCTION_DECLARE(typeObject,String,"split",1,2, {
		std::vector<std::string> result;
		StringUtils::split( self->getString(), parameter[0].toString(), result, parameter[1].toInt(-1) );

		Array * a=Array::create();
		for(std::vector<std::string>::const_iterator it=result.begin();it!=result.end();++it)
			a->pushBack( String::create(*it) );
		return a;
	})

	//! [ESMF] String String.fillUp(length[, string fill=" ")
	ES_MFUNCTION_DECLARE(typeObject,String,"fillUp",1,2,{
		const string & s(self->getString());
		std::ostringstream sprinter;
		sprinter<<s;
		string fill=parameter[1].toString(" ");
		int count=(parameter[0].toInt()-s.length())/(fill.length()>0?fill.length():0);
		for(int i=0;i<count;++i)
			sprinter<<fill;
		return String::create(sprinter.str());
	})

	//! [ESMF] Bool String.beginsWith( (String)search )
	ES_MFUNCTION_DECLARE(typeObject,String,"beginsWith",1,1, {
		const string & s(self->getString());
		string search=parameter[0]->toString();
		if(s.length()<search.length())
			return Bool::create(false);
		return Bool::create(s.substr(0,search.length())==search);
	})

	//! [ESMF] Bool String.endsWith( (String)search )
	ES_MFUNCTION_DECLARE(typeObject,String,"endsWith",1,1, {
		const string & s(self->getString());
		string search=parameter[0]->toString();
		if(s.length()<search.length()) return Bool::create(false);
		return Bool::create(s.substr(s.length()-search.length(),search.length())==search);
	})
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

//! (ctor)
String::String(const string & _s,Type * type):
		Object(type?type:typeObject),s(_s) {
	//ctor
}

//! (dtor)
String::~String() {
	//dtor
}

//! ---|> [Object]
Object * String::clone() const {
	return String::create(s,getType());
}

//! ---|> [Object]
string String::toString()const {
	return s;
}

//! ---|> [Object]
std::string String::toDbgString()const{
	return std::string("\"")+s+"\"";
}

//! ---|> [Object]
double String::toDouble()const {
	int to=0;
	return StringUtils::getNumber(s.c_str(),to, true);
}

//! ---|> [Object]
int String::toInt()const {
	int to=0;
	return static_cast<int>(StringUtils::getNumber(s.c_str(),to,  true));
}

//! ---|> [Object]
bool String::toBool()const {
	return true;//s.length()>0;
}

//! ---|> [Object]
bool String::rt_isEqual(Runtime &, const ObjPtr o){
	return o.isNull()?false:s==o.toString();
}
