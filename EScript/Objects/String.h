// String.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef STRING_H
#define STRING_H

#include "Object.h"
#include <stack>
#include <string>

namespace EScript {

/*! [String] ---|> [Object] */
class String : public Object {
		ES_PROVIDES_TYPE_NAME(String)
	private:
		static std::stack<String *> stringPool;
	public:
		static Type* typeObject;
		static void init(EScript::Namespace & globals);

		static String * create(const std::string & s);
		static String * create(const std::string & s,Type*type);
		static void release(String * b);

		// ---
		String(const std::string & s,Type * type=NULL);
		virtual ~String();

		const std::string & getString()const		{	return s;	}
		void setString(const std::string & _s)		{	s=_s;	}
		void appendString(const std::string & _s)	{	s+=_s;	}
		bool empty()const							{	return s.empty();	}

		/// ---|> [Object]
		virtual Object * clone()const;
		virtual std::string toString()const;
		virtual double toDouble()const;
		virtual int toInt()const;
		virtual bool toBool()const;
		virtual bool rt_isEqual(Runtime &rt,const ObjPtr o);
		virtual std::string toDbgString()const;
		virtual internalTypeId_t _getInternalTypeId()const 	{	return _TypeIds::TYPE_STRING;	}

	private:
		std::string s;

};
}

#endif // STRING_H
