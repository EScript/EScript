// StringId.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_StringId_H
#define ES_StringId_H

#include "Hashing.h"
#include <cstdint>
#include <unordered_map>

namespace EScript {

//! Numerical representation of a string constant.
class StringId{
	private:
		uint32_t id;
	public:
		static const std::string & toString(const uint32_t value)	{ return identifierIdToString(value);	}
		
		StringId() : id(0) {}
		explicit StringId(uint32_t _id) : id(_id) {}
		/*implicit*/ StringId(const std::string & str) : id(stringToIdentifierId(str)) {}
		StringId(const StringId &) = default;
		StringId(StringId &&) = default;

		uint32_t getValue()const			{	return id;	}
		const std::string & toString()const	{	return identifierIdToString(id);	}

		StringId & operator=(const StringId &) = default;
		StringId & operator=(StringId &&) = default;
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
	size_t operator()(const EScript::StringId & id) const noexcept {	return id.getValue();	}
};
}

#endif // ES_StringId_H
