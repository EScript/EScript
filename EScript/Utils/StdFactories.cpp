// StdFactories.cpp
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2013 Claudius Jähn <ClaudiusJ@live.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#include "StdFactories.h"

#include "../Basics.h"
#include "../StdObjects.h"

namespace EScript {

Array* create(const std::vector<Object*>&v)	{	return Array::create(v.size(),v.data());	}
String* create(const StringData & v)		{	return String::create(v);	}
String* create(const std::string & v)		{	return String::create(v);	}
String* create(const char*v)				{	return String::create(v);	}
Object* create(const ObjRef& obj)			{	return obj.get();	}
Void* create(std::nullptr_t)				{	return Void::get();	}

namespace _Internals{
Bool * createBool(bool v)			{	return Bool::create(v);	}
Number * createNumber(double v)		{	return Number::create(v);	}
Number * createNumber(float v)		{	return Number::create(v);	}
Number * createNumber(int v)		{	return Number::create(v);	}
Number * createNumber(uint32_t v)	{	return Number::create(v);	}
}

}
