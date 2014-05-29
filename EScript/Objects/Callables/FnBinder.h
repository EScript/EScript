// FnBinder.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2011-2013 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef ES_FN_BINDER_H
#define ES_FN_BINDER_H

#include "../Type.h"
#include <vector>

namespace EScript {

//! [FnBinder] ---|> [Object]
class FnBinder : public Object {
		ES_PROVIDES_TYPE_NAME(FnBinder)
	public:
		static Type* getTypeObject();
		static void init(EScript::Namespace & globals);
		// ----

		static FnBinder * create(ObjPtr object,ObjPtr function);
		static FnBinder * create(ObjPtr object,ObjPtr function,std::vector<ObjRef>&&params);
		static void release(FnBinder * b);
		virtual ~FnBinder()	{ }

		Object * getObject()const				{	return myObjectRef.get();	}
		Object * getFunction()const				{	return functionRef.get();	}
		std::vector<ObjRef> getBoundParameters()const	{	return boundParameters;	}
		void setObject(ObjPtr newObject)		{	myObjectRef = newObject;		}
		void setFunction(ObjPtr newFunction)	{	functionRef = newFunction;	}

		//! ---|> [Object]
		FnBinder * clone() const override;
		bool rt_isEqual(Runtime &rt, const ObjPtr & o) override;
		std::string toDbgString()const override;
		internalTypeId_t _getInternalTypeId()const override	{	return _TypeIds::TYPE_FN_BINDER;	}
	private:
		FnBinder(ObjPtr object,ObjPtr function);

		ObjRef myObjectRef, functionRef;
		std::vector<ObjRef> boundParameters;
};

}
#endif // ES_FN_BINDER_H
