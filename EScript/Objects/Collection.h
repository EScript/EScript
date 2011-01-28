#ifndef COLLECTION_H
#define COLLECTION_H

#include "ExtObject.h"

namespace EScript {
class Iterator;

/*! (abstract) [Collection] ---|> [ExtObject] ---|> [Object]  */
class Collection : public ExtObject {
        ES_PROVIDES_TYPE_NAME(Collection)
    public:
        static Type * getTypeObject();
        static void init(EScript::Namespace & globals);
        // ---
        Collection(Type * type=NULL);
        virtual ~Collection();

        /// ---o
        virtual Object * getValue(ObjPtr key);
        /// ---o
        virtual void setValue(ObjPtr key,ObjPtr value);
        /// ---o
        virtual void clear();
        /// ---o
        virtual size_t count()const;
        /// ---o
        virtual Iterator * getIterator();

        /// ---o
        virtual Object * rt_findValue(Runtime & runtime,ObjPtr value);
        virtual bool rt_contains(Runtime & runtime,ObjPtr value);
        virtual Object * rt_reduce(Runtime & runtime,ObjPtr function,ObjPtr initialValue);
        virtual Object * rt_map(Runtime & runtime,ObjPtr function);
        virtual Object * rt_extract(Runtime & runtime,identifierId functionId,bool decision=true);


        /// ---|> Object
        bool rt_isEqual(Runtime &runtime,const ObjPtr other);
};
}

#endif // COLLECTION_H
