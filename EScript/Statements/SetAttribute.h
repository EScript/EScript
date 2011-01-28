#ifndef SETATTRIBUTE_H
#define SETATTRIBUTE_H

#include "../Object.h"

namespace EScript {

/*! [SetAttribute]  ---|> [Object]  */
class SetAttribute : public Object {
        ES_PROVIDES_TYPE_NAME(SetAttribute)
    public:
		enum assignType_t{
			ASSIGN = 0, SET_OBJ_ATTRIBUTE = 1,SET_TYPE_ATTRIBUTE = 2
		};

        SetAttribute(Object * obj,identifierId attrId,Object * valueExp,assignType_t _assignType);
        virtual ~SetAttribute();

        identifierId getId()const   {   return attrId;  }

        /// ---|> [Object]
        virtual std::string toString()const;
        virtual Object * execute(Runtime & rt);

    private:
        ObjRef objRef;
        ObjRef valueExpRef;
        identifierId attrId;
        assignType_t assignType;
};
}

#endif // SETATTRIBUTE_H
