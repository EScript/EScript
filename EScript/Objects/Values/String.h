// String.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2012-2013 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
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

		const StringData & _getStringData()const	{	return sData;	}
		StringData & _getStringData()				{	return sData;	}
		const std::string & getString()const		{	return sData.str();	}
		void setString(const std::string & _s)		{	sData.set(_s);	}
		void setString(const StringData & _sData)	{	sData.set(_sData);	}

		//! ---|> [Object]
		Object * clone()const override				{	return create(sData);	}
		std::string toString()const override		{	return getString();	}
		double toDouble()const override;
		int toInt()const override;
		bool rt_isEqual(Runtime &rt,const ObjPtr & o) override;
		std::string toDbgString()const override;
		internalTypeId_t _getInternalTypeId()const override	{	return _TypeIds::TYPE_STRING;	}

	private:
		StringData sData;
};

}

#endif // STRING_H
