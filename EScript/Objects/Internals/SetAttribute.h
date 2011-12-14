// SetAttribute.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef SETATTRIBUTE_H
#define SETATTRIBUTE_H

#include "../Object.h"
#include <string>

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

		identifierId getAttrId()const   {   return attrId;  }
		Object * getObjectExpression()  {   return objExpr.get();    }
		assignType_t getAssignType()  	{   return assignType;    }
		Object * getValueExpression()  	{   return valueExpr.get();    }
		std::string getAttrName()const	{   return identifierIdToString(attrId);    }

		int getLine()const				{	return line;	}

		/// ---|> [Object]
		virtual std::string toString()const;
		virtual internalTypeId_t _getInternalTypeId()const {	return _TypeIds::TYPE_SET_ATTRIBUTE; }

	private:
		friend class Runtime;
		ObjRef objExpr;
		ObjRef valueExpr;
		identifierId attrId;
		assignType_t assignType;
		int line;
};
}

#endif // SETATTRIBUTE_H
