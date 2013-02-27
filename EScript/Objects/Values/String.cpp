// String.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "String.h"
#include "../../Basics.h"
#include "../../StdObjects.h"
#include "../../Utils/StringUtils.h"

#include <sstream>
#include <stack>

namespace EScript{

//! static, internal
StringData String::objToStringData(Object * obj){
	String * strObj = dynamic_cast<String*>(obj);
	return strObj==nullptr ? StringData(obj->toString()) : strObj->sData;
}

/*!
 * Try to cast the given object to the specified type.
 * If the object is not of the appropriate type, a runtime error is thrown.
 */
template<> String * assertType<String>(Runtime & runtime, const ObjPtr & obj) {
	
	if(obj.isNull()||obj->_getInternalTypeId()!=_TypeIds::TYPE_STRING) 
		assertType_throwError(runtime, obj, String::getClassName());
	return static_cast<String*>(obj.get());
}

//---

//! (static)
Type * String::getTypeObject(){
	static Type * typeObject = new Type(Object::getTypeObject()); // ---|> Object
	return typeObject;
}

//! initMembers
void String::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	typeObject->setFlag(Type::FLAG_CALL_BY_VALUE,true);
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] String new String((String)Obj)
	ESF_DECLARE(typeObject,"_constructor",0,1,String::create(parameter[0].toString("")))

	//! [ESMF] String String[(Number)position ]
	ES_MFUNCTION_DECLARE(typeObject,String,"_get",1,1, {
		int pos = parameter[0]->toInt();
		if(static_cast<unsigned int>(pos)>=self->getString().length())
			return nullptr;
		return self->getString().substr(pos,1);
	})


	// ---
	//! [ESMF] Bool String.beginsWith( (String)search )
	ES_MFUNCTION_DECLARE(typeObject,String,"beginsWith",1,1, {
		const std::string & s(self->getString());
		std::string search = parameter[0].toString();
		if(s.length()<search.length())
			return false;
		return s.substr(0,search.length())==search;
	})

	//! [ESMF] Bool String.contains (String)search [,(Number)startIndex] )
	ES_MFUNCTION_DECLARE(typeObject,String,"contains",1,2, {
		const std::string & s(self->getString());
		const std::string search = parameter[0].toString();
		size_t start = s.length();
		if(parameter.count()>1) {
			start = static_cast<size_t>(parameter[1].to<int>(runtime));
			if(start>=s.length())
				start = s.length();
		}
		return s.rfind(search,start)!=std::string::npos;
	})

	//! [ESMF] Bool String.empty()
	ESMF_DECLARE(typeObject,String,"empty",0,0,self->getString().empty())

	//! [ESMF] Bool String.endsWith( (String)search )
	ES_MFUNCTION_DECLARE(typeObject,String,"endsWith",1,1, {
		const std::string & s(self->getString());
		std::string search = parameter[0].toString();
		if(s.length()<search.length()) return false;
		return s.substr(s.length()-search.length(),search.length())==search;
	})

	//! [ESMF] String String.fillUp(length[, string fill=" ")
	ES_MFUNCTION_DECLARE(typeObject,String,"fillUp",1,2,{
		const std::string & s(self->getString());
		std::ostringstream sprinter;
		sprinter<<s;
		const std::string fill = parameter[1].toString(" ");
		if(!fill.empty()){
			const int count = (parameter[0].to<int>(runtime)-s.length())/fill.length();
			for(int i = 0;i<count;++i)
				sprinter<<fill;
		}
		return sprinter.str();
	})

	//! [ESMF] Number|false String.find( (String)search [,(Number)startIndex] )
	ES_MFUNCTION_DECLARE(typeObject,String,"find",1,2, {
		const std::string & s(self->getString());
		std::string search = parameter[0].toString();
		size_t start = 0;
		if(parameter.count()>1) {
			start = static_cast<size_t>(parameter[1].to<int>(runtime));
			if(start>=s.length())
				return false;
		}
		size_t pos = s.find(search,start);
		if(pos==std::string::npos ) {
			return false;
		} else {
			return static_cast<uint32_t>(pos);
		}
	})

	//! [ESMF] Number String.length()
	ESMF_DECLARE(typeObject,String,"length",0,0, static_cast<uint32_t>(self->getString().length()))

	//! [ESMF] String String.ltrim()
	ESMF_DECLARE(typeObject,String,"lTrim",0,0,StringUtils::lTrim(self->getString()))

	//! [ESMF] String String.rTrim()
	ESMF_DECLARE(typeObject,String,"rTrim",0,0,StringUtils::rTrim(self->getString()))

	//! [ESMF] String String.substr( (Number)begin [,(Number)length] )
	ES_MFUNCTION_DECLARE(typeObject,String,"substr",1,2, {
		int start = parameter[0].to<int>(runtime);
		int length = self->getString().length();
		if(start>=length) return create("");
		if(start<0) start = length+start;
		if(start<0) start = 0;
		int count = length-start;
		if(parameter.count()>1) {
			int i = parameter[1].to<int>(runtime);
			if(i<count) {
				if(i<0) {
					count+=i;
				} else {
					count = i;
				}
			}
		}
		return self->getString().substr(start,count);
	})

	//! [ESMF] String String.trim()
	ESMF_DECLARE(typeObject,String,"trim",0,0,StringUtils::trim(self->getString()))


	//! [ESMF] String String.replace((String)search,(String)replace)
	ESMF_DECLARE(typeObject,String,"replace",2,2,
				StringUtils::replaceAll(self->getString(),parameter[0].toString(),parameter[1]->toString(),1))

	typedef std::pair<std::string,std::string> keyValuePair_t;
	//! [ESMF] String.replaceAll( (Map | ((String)search,(String)replace)) [,(Number)max])
	ES_MFUNCTION_DECLARE(typeObject,String,"replaceAll",1,3,{
		const std::string & subject(self->getString());

		//Map * m
		if( Map * m = parameter[0].toType<Map>()) {
			assertParamCount(runtime,parameter.count(),1,2);
			std::vector<keyValuePair_t> rules;
			ERef<Iterator> iRef = m->getIterator();
			while(!iRef->end()) {
				ObjRef key = iRef->key();
				ObjRef value = iRef->value();
				rules.push_back(std::make_pair(key.toString(),value.toString()));
				iRef->next();
			}
			return StringUtils::replaceMultiple(subject,rules,parameter[1].toInt(-1));
		}

		const std::string search(parameter[0].toString());
		const std::string replace(parameter[1]->toString());

		return StringUtils::replaceAll(subject,search,replace,parameter[2].toInt(-1));
	})

	//! [ESMF] Number|false String.rFind( (String)search [,(Number)startIndex] )
	ES_MFUNCTION_DECLARE(typeObject,String,"rFind",1,2, {
		const std::string & s(self->getString());
		std::string search = parameter[0].toString();
		size_t start = s.length();
		if(parameter.count()>1) {
			start = static_cast<size_t>(parameter[1].to<int>(runtime));
			if(start>=s.length())
				start = s.length();
			//std::cout << " #"<<start<< " ";
		}
		size_t pos = s.rfind(search,start);
		if(pos==std::string::npos ) {
			return false;
		} else return static_cast<uint32_t>(pos);
	})




	//! [ESMF] Array String.split((String)search[,(Number)max])
	ES_MFUNCTION_DECLARE(typeObject,String,"split",1,2, {
		std::vector<std::string> result;
		StringUtils::split( self->getString(), parameter[0].toString(), result, parameter[1].to<int>(runtime,-1) );

		Array * a = Array::create();
		for(const auto & str : result) 
			a->pushBack(String::create(str));
		return a;
	})

	//! [ESMF] String String.toLower()
	ESMF_DECLARE(typeObject,String,"toLower",0,0,StringUtils::toLower(self->getString()))

	//! [ESMF] String String.toUpper()
	ESMF_DECLARE(typeObject,String,"toUpper",0,0,StringUtils::toUpper(self->getString()))

	//- Operators

	//! [ESMF] String String+(String)Obj
	ESMF_DECLARE(typeObject,String,"+",1,1,self->getString() + parameter[0].toString())

	//! [ESMF] String String*(Number)Obj
	ES_MFUNCTION_DECLARE(typeObject,String,"*",1,1,{
		std::string s;
		const std::string s2( self->getString() );
		for(int i = parameter[0].to<int>(runtime);i>0;--i)
			s+=s2;
		return s;
	})

	//! [ESMF] self String+=(String)Obj
	ESMF_DECLARE(typeObject,String,"+=",1,1,(self->appendString(parameter[0].toString()),self))

	//! [ESMF] self String*=(Number)Obj
	ES_MFUNCTION_DECLARE(typeObject,String,"*=",1,1,{
		std::string s;
		std::string s2( self->getString() );
		for(int i = parameter[0].to<int>(runtime);i>0;--i)
			s+=s2;
		self->setString(s);
		return  caller;
	})

	//! [ESMF] bool String>(String)Obj
	ESMF_DECLARE(typeObject,String,">",1,1,self->getString() > parameter[0].toString())

	//! [ESMF] bool String>=(String)Obj
	ESMF_DECLARE(typeObject,String,">=",1,1,self->getString() >= parameter[0].toString())

	//! [ESMF] bool String<(String)Obj
	ESMF_DECLARE(typeObject,String,"<",1,1,self->getString() < parameter[0].toString())

	//! [ESMF] bool String<=(String)Obj
	ESMF_DECLARE(typeObject,String,"<=",1,1,self->getString() <= parameter[0].toString())
}

//---
static std::stack<String *> pool;

String * String::create(const StringData & sData){
	#ifdef ES_DEBUG_MEMORY
	return new String(sData);
	#endif
	if(pool.empty()){
		return new String (sData);
	}else{
		String * o = pool.top();
		pool.pop();
		o->setString(sData);
		return o;
	}
}
void String::release(String * o){
	#ifdef ES_DEBUG_MEMORY
	delete o;
	return;
	#endif
	if(o->getType()!=getTypeObject()){
		delete o;
		std::cout << "Found diff StringType\n";
	}else{
	   pool.push(o);
	}
}
//---


//! ---|> [Object]
std::string String::toDbgString()const{
	return std::string("\"")+sData.str()+"\"";
}

//! ---|> [Object]
double String::toDouble()const {
	int to = 0;
	return StringUtils::getNumber(sData.str().c_str(),to, true);
}

//! ---|> [Object]
int String::toInt()const {
	int to = 0;
	return static_cast<int>(StringUtils::getNumber(sData.str().c_str(),to,  true));
}

//! ---|> [Object]
bool String::rt_isEqual(Runtime &, const ObjPtr & o){
	return o.isNull()?false:sData==objToStringData(o.get());
}
}

