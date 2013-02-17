// Bool.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
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
		virtual Object * clone()const						{	return create(value);	}
		virtual std::string toString()const					{	return value?"true":"false";	}
		virtual bool toBool()const							{	return value;	}
		virtual double toDouble()const						{	return value?1:0;	}
		virtual bool rt_isEqual(Runtime &,const ObjPtr & o)	{	return value==o.toBool(false);	}
		virtual internalTypeId_t _getInternalTypeId()const	{	return _TypeIds::TYPE_BOOL;	}

	private:
		bool value;
};

}

#endif // BOOL_H
