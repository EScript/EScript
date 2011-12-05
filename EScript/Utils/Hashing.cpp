// Hashing.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Hashing.h"
#include "HashMap.h"

#include <cstddef>
#include <functional>
#include <map>
#include <utility>

using namespace EScript;

//typedef std::map<identifierId,std::string> identifierDB;
typedef HashMap<std::string,identifierId> identifierDB;

/**
 * (static) Returns the identifier database.
 * The database is created on the first call.
 * (When using a normal initializing, some compile order may cause runtime errors
 * if static identifiers are defined in other files that are compiled earlier.)
 */
static identifierDB & getIdentifierDB(){
	static identifierDB _identifier;
	return _identifier;
}
const identifierId EScript::IDENTIFIER_emptyStr=stringToIdentifierId("");
const std::string EScript::ES_UNKNOWN_IDENTIFIER="[?]";

//! (internal)
hashvalue EScript::_hash( const std::string &  s) {
	hashvalue h=0;
	for (size_t i=0;i<s.length();++i)
		h^=(((s.at(i)+h)*1234393)% 0xffffff);
	return h;
}

identifierId EScript::stringToIdentifierId(const std::string & s){
	identifierDB & db=getIdentifierDB();
	identifierId id=_hash(s);
	while(true){
		std::string & oldString = db.get(id);
		if(oldString.empty()){// id not found -> insert it
			oldString=s;
			break;
		}else if(s==oldString){ // same string already inserted
			break;
		}else{
			++id;
		}	
//		std::string * oldString = db.findPtr(id);
//		if(oldString==NULL){// id not found -> insert it
//			db.insert(id,s);
//			break;
//		}else if(s==*oldString){ // same string already inserted
//			break;
//		}else{
//			++id;
//		}
//		
//		identifierDB::iterator lbIt=db.lower_bound(id);
//		if(lbIt==db.end() || db.key_comp()(id, lbIt->first) ){
//			// id not found -> insert it
//			db.insert(lbIt,std::make_pair(id,s));
//			break;
//		}else if( s==lbIt->second){
//			// same string already inserted
//			break;
//		}else {
//			// collision
//			++id;
//		}
	}
	return id;
}

const std::string & EScript::identifierIdToString(identifierId id){
	identifierDB & db=getIdentifierDB();

	std::string * str = db.findPtr(id);
	return str==NULL ? ES_UNKNOWN_IDENTIFIER : *str;
//	
//	identifierDB::const_iterator it=db.find(id);
//	if(it == db.end() )
//		return ES_UNKNOWN_IDENTIFIER;
//	else return *it;
}
