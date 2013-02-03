// StdFactories.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "StdFactories.h"

#include "Objects/Collections/Array.h"
#include "Objects/Values/Bool.h"
#include "Objects/Values/Number.h"
#include "Objects/Values/String.h"
#include "Objects/Values/Void.h"

namespace EScript {

Array* create(const std::vector<Object*>&v)	{	return Array::create(v.size(),v.data());	}
Number* create(const double& v)				{	return Number::create(v);	}
Number* create(const float& v)				{	return Number::create(v);	}
Number* create(const int& v)				{	return Number::create(v);	}
Number* create(const uint32_t& v)			{	return Number::create(v);	}
String* create(const StringData & v)		{	return String::create(v);	}
String* create(const std::string & v)		{	return String::create(v);	}
String* create(const char*v)				{	return String::create(v);	}
Bool* create(const bool& v)					{	return Bool::create(v);	}
Void* create(std::nullptr_t)				{	return Void::get();	}

}
