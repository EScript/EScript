// Bool.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef BOOL_H
#define BOOL_H

#include "../Object.h"
#include <string>

namespace EScript {

/*! [Bool] ---|> [Object]   */
class Bool : public Object {
		ES_PROVIDES_TYPE_NAME(Bool)
	public:
		static Type * getTypeObject();

		static void init(EScript::Namespace & globals);
		static Bool * create(bool value);
		static void release(Bool * b);

		// ---

		explicit Bool(bool value,Type * type=NULL);
		virtual ~Bool();

		void setValue(bool b)				{	value=b;	}

		/// ---|> [Object]
		virtual Object * clone()const;
		virtual std::string toString()const;
		virtual bool toBool()const;
		virtual double toDouble()const;
		virtual bool rt_isEqual(Runtime & rt,const ObjPtr o);
		virtual internalTypeId_t _getInternalTypeId()const 	{	return _TypeIds::TYPE_BOOL;	}

		//! ---|> Object
		void _asmOut(std::ostream & out);
		
	protected:
		bool value;
};
}

#endif // BOOL_H
