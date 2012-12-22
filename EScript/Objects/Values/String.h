// String.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef STRING_H
#define STRING_H

#include "../Object.h"
#include "../../Utils/StringData.h"
#include <stack>
#include <string>

namespace EScript {

/*! [String] ---|> [Object] */
class String : public Object {
		ES_PROVIDES_TYPE_NAME(String)
	private:
		static std::stack<String *> stringPool;

		explicit String(const StringData & sData,Type * type = nullptr);

		//! internal helper
		static StringData objToStringData(Object * obj);

	public:
		static Type * getTypeObject();
		static void init(EScript::Namespace & globals);

		static String * create(const std::string & s)				{	return create(StringData(s));	}
		static String * create(const std::string & s,Type*type)		{	return create(StringData(s),type);	}
		static String * create(const StringData & sData);
		static String * create(const StringData & sData,Type * type);
		static void release(String * b);

		// ---
		virtual ~String();

		const std::string & getString()const		{	return sData.str();	}
		void setString(const std::string & _s)		{	sData.set(_s);	}
		void setString(const StringData & _sData)	{	sData.set(_sData);	}
		void appendString(const std::string & _s)	{	sData.set(sData.str()+_s);	}
		bool empty()const							{	return sData.empty();	}

		/// ---|> [Object]
		virtual Object * clone()const;
		virtual std::string toString()const;
		virtual double toDouble()const;
		virtual int toInt()const;
		virtual bool toBool()const;
		virtual bool rt_isEqual(Runtime &rt,const ObjPtr o);
		virtual std::string toDbgString()const;
		virtual internalTypeId_t _getInternalTypeId()const	{	return _TypeIds::TYPE_STRING;	}

	private:
		StringData sData;
};
}

#endif // STRING_H
