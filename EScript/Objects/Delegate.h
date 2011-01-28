#ifndef ES_DELEGATE_H
#define ES_DELEGATE_H

#include "../Object.h"
#include <vector>

namespace EScript {

/*! [Delegate] ---|> [Object]   */
class Delegate : public Object {
        ES_PROVIDES_TYPE_NAME(Delegate)
    public:
        static Type* typeObject;
        static void init(EScript::Namespace & globals);
        // ----

        Delegate(ObjPtr object,ObjPtr function);
        virtual ~Delegate();

        inline Object * getObject()const		{	return myObjectRef.get();	}
        inline Object * getFunction()const		{	return functionRef.get();	}
        void setObject(ObjPtr newObject)		{	myObjectRef=newObject;		}
        void setFunction(ObjPtr newFunction)	{	functionRef=newFunction;	}

        /// ---|> [Object]
        virtual Delegate * clone() const;
		virtual bool rt_isEqual(Runtime &rt, const ObjPtr o);

    private:
        ObjRef myObjectRef;
        ObjRef functionRef;
};

}
#endif // ES_DELEGATE_H
