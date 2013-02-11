// Delegate.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_DELEGATE_H
#define ES_DELEGATE_H

#include "../Type.h"

namespace EScript {

//! [Delegate] ---|> [Object]
class Delegate : public Object {
		ES_PROVIDES_TYPE_NAME(Delegate)
	public:
		static Type* getTypeObject();
		static void init(EScript::Namespace & globals);
		// ----

		static Delegate * create(ObjPtr object,ObjPtr function);
		static void release(Delegate * b);
		virtual ~Delegate()	{ }

		Object * getObject()const				{	return myObjectRef.get();	}
		Object * getFunction()const				{	return functionRef.get();	}
		void setObject(ObjPtr newObject)		{	myObjectRef = newObject;		}
		void setFunction(ObjPtr newFunction)	{	functionRef = newFunction;	}

		//! ---|> [Object]
		virtual Delegate * clone() const;
		virtual bool rt_isEqual(Runtime &rt, const ObjPtr & o);
		virtual std::string toDbgString()const;
		virtual internalTypeId_t _getInternalTypeId()const	{	return _TypeIds::TYPE_DELEGATE;	}
	private:
		Delegate(ObjPtr object,ObjPtr function);

		ObjRef myObjectRef;
		ObjRef functionRef;
};

}
#endif // ES_DELEGATE_H
