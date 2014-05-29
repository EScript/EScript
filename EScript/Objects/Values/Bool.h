// Bool.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2012-2013 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef BOOL_H
#define BOOL_H

#include "../Type.h"
#include <string>

namespace EScript {

//! [Bool] ---|> [Object]
class Bool : public Object {
		ES_PROVIDES_TYPE_NAME(Bool)
	public:
		static Type* getTypeObject();
		static void init(EScript::Namespace & globals);
		static Bool * create(bool value);
		static void release(Bool * b);

		// ---

		explicit Bool(bool _value) :
				Object(getTypeObject()),value(_value) {}
		virtual ~Bool(){}

		void setValue(bool b)								{	value = b;	}

		bool & operator*()									{	return value;	}
		bool operator*()const								{	return value;	}

		//! ---|> [Object]
		Object * clone()const override						{	return create(value);	}
		std::string toString()const override					{	return value?"true":"false";	}
		bool toBool()const override							{	return value;	}
		double toDouble()const override						{	return value?1:0;	}
		bool rt_isEqual(Runtime &,const ObjPtr & o) override	{	return value==o.toBool(false);	}
		internalTypeId_t _getInternalTypeId()const override	{	return _TypeIds::TYPE_BOOL;	}

	private:
		bool value;
};

}

#endif // BOOL_H
