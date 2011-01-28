#include "Hashing.h"
#include <iostream>
#include <map>

using namespace EScript;

typedef std::map<identifierId,std::string> identifierDB;

/**
 * (static) Returns the identifier database.
 * The database is created on the first call.
 * (When using a normal initializing, some compile order may cause runtime errors
 * if static identifiers are defined in other files that are compiled earlier.)
 */
static identifierDB & getIdentifierDB(){
    static identifierDB _identifier=identifierDB();
    return _identifier;
}
const identifierId EScript::IDENTIFIER_emptyStr=stringToIdentifierId("");
const std::string EScript::ES_UNKNOWN_IDENTIFIER="[?]";
/**
 *
 */
hashvalue EScript::_hash(const char * s) {
    hashvalue h=0;
    for (; *s;s++) {
        h^=((((*s)+h)*1234393)% 0xffffff) ;
    }//
    return h;
}

/**
 *
 */
hashvalue EScript::_hash( const std::string &  s) {
    hashvalue h=0;
    for (size_t i=0;i<s.length();++i)
        h^=(((s.at(i)+h)*1234393)% 0xffffff);
    return h;
}

/**
 *
 */
identifierId EScript::stringToIdentifierId(const std::string & s){
    identifierDB & db=getIdentifierDB();
    identifierId id=_hash(s);
    while(true){
    	identifierDB::iterator lbIt=db.lower_bound(id);
		if(lbIt==db.end() || db.key_comp()(id, lbIt->first) ){
			// id not found -> insert it
			db.insert(lbIt,std::make_pair(id,s));
			break;
		}else if( s==lbIt->second){
			// same string already inserted
			break;
		}else {
			// collision
			++id;
		}
    }
    return id;
}

/**
 *
 */
const std::string & EScript::identifierIdToString(identifierId id){
     identifierDB & db=getIdentifierDB();

    identifierDB::const_iterator it=db.find(id);
    if(it == db.end() )
        return ES_UNKNOWN_IDENTIFIER;
    else return (*it).second;
}
