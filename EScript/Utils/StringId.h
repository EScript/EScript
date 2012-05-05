// StringId.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_StringId_H
#define ES_StringId_H

#include "Hashing.h"
#include <stdint.h>
#include <unordered_map>

namespace EScript {

/*! Numerical representation of a string constant. */
class StringId{
	private:
		uint32_t id;
	public:
		StringId() : id(0) {}
		explicit StringId( uint32_t _id) : id(_id) {}
		/*implicit*/ StringId( const std::string & str) : id(stringToIdentifierId(str)) {}
		StringId( const StringId & other) : id(other.id) {}

		uint32_t getValue()const			{	return id;	}
		const std::string & toString()const	{	return identifierIdToString(id);	}

		StringId & operator=(const StringId & other){
			id = other.id;
			return *this;
		}

		StringId & operator=(const std::string & str){
			id = stringToIdentifierId(str);
			return *this;
		}
		bool empty()const								{	return id==0;	}

		bool operator==(const StringId & other)const	{	return id == other.id;	}
		bool operator!=(const StringId & other)const	{	return id != other.id;	}
		bool operator<(const StringId & other)const		{	return id < other.id;	}
};

}
namespace std{
template <> struct hash<EScript::StringId> {
	size_t operator()(const EScript::StringId & id) const throw() {	return id.getValue();	}
};
}

#endif // ES_StringId_H
