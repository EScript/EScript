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
		_Internals::assertType_throwError(runtime, obj, String::getClassName());
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
	ES_CTOR(typeObject,0,1,String::create(parameter[0].toString("")))

	//! [ESMF] String String[(Number)position ]
	ES_MFUNCTION(typeObject,const String,"_get",1,1, {
		const std::string s = thisObj->sData.getSubStr( parameter[0].to<uint32_t>(rt), 1 );
		if(s.empty())
			return nullptr;
		return s;
	})


	// ---
	//! [ESMF] Bool String.beginsWith( (String)search ) // \todo starting pos!!!!!!!!!!!!!!!!!!!!!!!!
	ES_MFUNCTION(typeObject,const String,"beginsWith",1,1, {
		const std::string search = parameter[0].toString();
		return thisObj->getString().compare(0,search.length(),search)==0;
	})

	//! [ESMF] Bool String.contains (String)search [,(Number)startIndex] )
	ES_MFUN(typeObject,const String,"contains",1,2, 
				thisObj->getString().find(
										parameter[0].toString(),
										thisObj->sData.codePointToBytePos( parameter[1].toUInt(0) )) != std::string::npos )

	//! [ESMF] Bool String.empty()
	ES_MFUN(typeObject,const String,"empty",0,0,thisObj->getString().empty())

	//! [ESMF] Bool String.endsWith( (String)search )
	ES_MFUNCTION(typeObject,const String,"endsWith",1,1, {
		const std::string search = parameter[0].toString();
		return thisObj->getString().compare(thisObj->getString().length()-search.length(),search.length(),search)==0;
	})

	//! [ESMF] String String.fillUp(length[, string fill=" ")
	ES_MFUNCTION(typeObject,const String,"fillUp",1,2,{
		size_t length = thisObj->length();
		const size_t targetLength = parameter[0].to<uint32_t>(rt);
		if(targetLength<=length)
			return thisEObj;
		
		const std::string fillStr = parameter[1].toString(" ");
		const size_t fillLength = StringData(fillStr).getNumCodepoints();
		if(fillLength==0)
			return thisEObj;
		
		std::ostringstream sprinter;
		sprinter<<thisObj->getString();
		while( length<targetLength ){
			sprinter<<fillStr;
			length += fillLength;
		}
		return sprinter.str();
	})

	//! [ESMF] Number|false String.find( (String)search [,(Number)startIndex] )
	ES_MFUNCTION(typeObject,String,"find",1,2, {
		const size_t pos = thisObj->sData.find(parameter[0].toString(),parameter[1].toUInt(0));
		if(pos==std::string::npos ) {
			return false;
		} else {
			return static_cast<uint32_t>(pos);
		}
	})

	//! [ESMF] Number String.length()
	ES_MFUN(typeObject,String,"length",0,0, static_cast<uint32_t>(thisObj->sData.getNumCodepoints()))

	//! [ESMF] String String.ltrim()
	ES_MFUN(typeObject,String,"lTrim",0,0,StringUtils::lTrim(thisObj->getString()))

	//! [ESMF] String String.rTrim()
	ES_MFUN(typeObject,String,"rTrim",0,0,StringUtils::rTrim(thisObj->getString()))

	//! [ESMF] String String.substr( (Number)begin [,(Number)length] ) // UNICODE_TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	ES_MFUNCTION(typeObject,String,"substr",1,2, {
		int start = parameter[0].to<int>(rt);
		const size_t length = thisObj->sData.getNumCodepoints();
		if(start>=length) return create("");
		if(start<0) start = length+start;
		int count = length-start;
		if(parameter.count()>1) {
			int i = parameter[1].to<int>(rt);
			if(i<count) {
				if(i<0) {
					count+=i;
				} else {
					count = i;
				}
			}
		}
		return thisObj->sData.getSubStr( static_cast<size_t>(start), static_cast<size_t>(count) );
	})

	//! [ESMF] String String.trim()
	ES_MFUN(typeObject,String,"trim",0,0,StringUtils::trim(thisObj->getString()))  // UNICODE_TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	//! [ESMF] String String.replace((String)search,(String)replace)  // UNICODE_TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	ES_MFUN(typeObject,String,"replace",2,2,
				StringUtils::replaceAll(thisObj->getString(),parameter[0].toString(),parameter[1]->toString(),1))

	typedef std::pair<std::string,std::string> keyValuePair_t;
	//! [ESMF] String.replaceAll( (Map | ((String)search,(String)replace)) [,(Number)max])  // UNICODE_TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	ES_MFUNCTION(typeObject,String,"replaceAll",1,3,{
		const std::string & subject(thisObj->getString());

		//Map * m
		if( Map * m = parameter[0].toType<Map>()) {
			assertParamCount(rt,parameter.count(),1,2);
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

	//! [ESMF] Number|false String.rFind( (String)search [,(Number)startIndex] )// UNICODE_TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	ES_MFUNCTION(typeObject,String,"rFind",1,2, {
		const std::string & s(thisObj->getString());
		std::string search = parameter[0].toString();
		size_t start = s.length();
		if(parameter.count()>1) {
			start = static_cast<size_t>(parameter[1].to<int>(rt));
			if(start>=s.length())
				start = s.length();
			//std::cout << " #"<<start<< " ";
		}
		size_t pos = s.rfind(search,start);
		if(pos==std::string::npos ) {
			return false;
		} else return static_cast<uint32_t>(pos);
	})




	//! [ESMF] Array String.split((String)search[,(Number)max])  // UNICODE_TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	ES_MFUNCTION(typeObject,String,"split",1,2, {
		std::vector<std::string> result;
		StringUtils::split( thisObj->getString(), parameter[0].toString(), result, parameter[1].to<int>(rt,-1) );
		return Array::create(result);
	})

	//! [ESMF] String String.toLower()  // UNICODE_TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	ES_MFUN(typeObject,String,"toLower",0,0,StringUtils::toLower(thisObj->getString()))

	//! [ESMF] String String.toUpper()  // UNICODE_TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	ES_MFUN(typeObject,String,"toUpper",0,0,StringUtils::toUpper(thisObj->getString()))

	//- Operators

	//! [ESMF] String String+(String)Obj
	ES_MFUN(typeObject,String,"+",1,1,thisObj->getString() + parameter[0].toString())

	//! [ESMF] String String*(Number)Obj
	ES_MFUNCTION(typeObject,String,"*",1,1,{
		std::string s;
		const std::string s2( thisObj->getString() );
		for(int i = parameter[0].to<int>(rt);i>0;--i)
			s+=s2;
		return s;
	})

	//! [ESMF] thisObj String+=(String)Obj
	ES_MFUN(typeObject,String,"+=",1,1,(thisObj->appendString(parameter[0].toString()),thisEObj))

	//! [ESMF] thisObj String*=(Number)Obj
	ES_MFUNCTION(typeObject,String,"*=",1,1,{
		std::string s;
		std::string s2( thisObj->getString() );
		for(int i = parameter[0].to<int>(rt);i>0;--i)
			s+=s2;
		thisObj->setString(s);
		return  thisEObj;
	})

	//! [ESMF] bool String>(String)Obj
	ES_MFUN(typeObject,String,">",1,1,thisObj->getString() > parameter[0].toString())

	//! [ESMF] bool String>=(String)Obj
	ES_MFUN(typeObject,String,">=",1,1,thisObj->getString() >= parameter[0].toString())

	//! [ESMF] bool String<(String)Obj
	ES_MFUN(typeObject,String,"<",1,1,thisObj->getString() < parameter[0].toString())

	//! [ESMF] bool String<=(String)Obj
	ES_MFUN(typeObject,String,"<=",1,1,thisObj->getString() <= parameter[0].toString())
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
double String::toDouble() const {
	std::size_t to = 0;
	return StringUtils::readNumber(sData.str().c_str(), to, true);
}

//! ---|> [Object]
int String::toInt() const {
	std::size_t to = 0;
	return static_cast<int>(StringUtils::readNumber(sData.str().c_str(), to, true));
}

//! ---|> [Object]
bool String::rt_isEqual(Runtime &, const ObjPtr & o){
	return o.isNull()?false:sData==objToStringData(o.get());
}
}

