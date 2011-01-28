// GetAttribute.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef GETATTRIBUTE_H
#define GETATTRIBUTE_H

#include "../Object.h"

namespace EScript {

/*! [GetAttribute]  ---|> [Object]  */
class GetAttribute : public Object {
		ES_PROVIDES_TYPE_NAME(GetAttribute)
	public:
		GetAttribute(Object * obj,identifierId attrId);
		GetAttribute(Object * obj,const std::string & attrName);
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
};
}

#endif // GETATTRIBUTE_H
