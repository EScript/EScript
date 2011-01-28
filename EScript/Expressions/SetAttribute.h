// SetAttribute.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
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

		SetAttribute(Object * obj,identifierId attrId,Object * valueExp,assignType_t _assignType,int _line=-1);
		virtual ~SetAttribute();

		identifierId getId()const   {   return attrId;  }

		int getLine()const			{	return line;	}

		/// ---|> [Object]
		virtual std::string toString()const;
		virtual Object * execute(Runtime & rt);

	private:
		ObjRef objRef;
		ObjRef valueExpRef;
		identifierId attrId;
		assignType_t assignType;
		int line;
};
}

#endif // SETATTRIBUTE_H
