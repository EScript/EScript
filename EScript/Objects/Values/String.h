// String.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef STRING_H
#define STRING_H

#include "../Type.h"
#include "../../Utils/StringData.h"
#include <string>

namespace EScript {

//! [String] ---|> [Object]
class String : public Object {
		ES_PROVIDES_TYPE_NAME(String)
	private:
		explicit String(const StringData & _sData) : Object(getTypeObject()),sData(_sData) {}

		//! internal helper
		static StringData objToStringData(Object * obj);

	public:
		static Type* getTypeObject();
		static void init(EScript::Namespace & globals);

		static String * create(const std::string & s)		{	return create(StringData(s));	}
		static String * create(const StringData & sData);
		static void release(String * b);

		// ---
		virtual ~String()							{}

		StringData & operator*()					{	return sData;	}
		const std::string & operator*()const		{	return sData.str();	}

		void appendString(const std::string & _s)	{	sData.set(sData.str()+_s);	}
		bool empty()const							{	return sData.empty();	}
		size_t length()const						{	return sData.getNumCodepoints();	}
		size_t getDataSize()const					{	return sData.getDataSize();	}

		const std::string & getString()const		{	return sData.str();	}
		void setString(const std::string & _s)		{	sData.set(_s);	}
		void setString(const StringData & _sData)	{	sData.set(_sData);	}

		//! ---|> [Object]
		virtual Object * clone()const				{	return create(sData);	}
		virtual std::string toString()const			{	return getString();	}
		virtual double toDouble()const;
		virtual int toInt()const;
		virtual bool rt_isEqual(Runtime &rt,const ObjPtr & o);
		virtual std::string toDbgString()const;
		virtual internalTypeId_t _getInternalTypeId()const	{	return _TypeIds::TYPE_STRING;	}

	private:
		StringData sData;
};

}

#endif // STRING_H
