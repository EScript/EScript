// Delegate.h
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2011-2013 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef ES_DELEGATE_H
#define ES_DELEGATE_H

#include "../Type.h"
#include <vector>

namespace EScript {

//! [Delegate] ---|> [Object]
class Delegate : public Object {
		ES_PROVIDES_TYPE_NAME(Delegate)
	public:
		static Type* getTypeObject();
		static void init(EScript::Namespace & globals);
		// ----

		static Delegate * create(ObjPtr object,ObjPtr function);
		static Delegate * create(ObjPtr object,ObjPtr function,std::vector<ObjRef>&&params);
		static void release(Delegate * b);
		virtual ~Delegate()	{ }

		Object * getObject()const				{	return myObjectRef.get();	}
		Object * getFunction()const				{	return functionRef.get();	}
		std::vector<ObjRef> getBoundParameters()const	{	return boundParameters;	}
		void setObject(ObjPtr newObject)		{	myObjectRef = newObject;		}
		void setFunction(ObjPtr newFunction)	{	functionRef = newFunction;	}

		//! ---|> [Object]
		Delegate * clone() const override;
		bool rt_isEqual(Runtime &rt, const ObjPtr & o) override;
		std::string toDbgString()const override;
		internalTypeId_t _getInternalTypeId()const override	{	return _TypeIds::TYPE_DELEGATE;	}
	private:
		Delegate(ObjPtr object,ObjPtr function);

		ObjRef myObjectRef, functionRef;
		std::vector<ObjRef> boundParameters;
};

}
#endif // ES_DELEGATE_H
