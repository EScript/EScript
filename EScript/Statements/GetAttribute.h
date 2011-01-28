#ifndef GETATTRIBUTE_H
#define GETATTRIBUTE_H

#include "../Object.h"

namespace EScript {

/*! [GetAttribute]  ---|> [Object]  */
class GetAttribute : public Object {
        ES_PROVIDES_TYPE_NAME(GetAttribute)
    public:
        GetAttribute(Object * obj,identifierId attrId,bool global=false);
        GetAttribute(Object * obj,const std::string & attrName,bool global=false);
        virtual ~GetAttribute();

        identifierId getId()const       {   return attrId;  }
        std::string getAttrName()const  {   return identifierIdToString(attrId);    }
        Object * getObject()            {   return objRef.get();    }

        /// ---|> [Object]
        virtual std::string toString()const;
        virtual Object * execute(Runtime & rt);

    private:
        ObjRef objRef;
        identifierId attrId;
        bool global;
};
}

#endif // GETATTRIBUTE_H
