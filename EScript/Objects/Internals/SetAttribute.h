// SetAttribute.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef SETATTRIBUTE_H
#define SETATTRIBUTE_H

#include "../Object.h"
#include "../../Utils/Attribute.h"
#include <string>

namespace EScript {

/*! [SetAttribute]  ---|> [Object]  */
class SetAttribute : public Object {
		ES_PROVIDES_TYPE_NAME(SetAttribute)
	public:
		static SetAttribute * createAssignment(Object * obj,identifierId attrId,Object * valueExp,int _line=-1);
		
		SetAttribute(Object * obj,identifierId attrId,Object * valueExp,Attribute::flag_t _attrFlags,int _line=-1);
		virtual ~SetAttribute();

		identifierId getAttrId()const   	{   return attrId;  }
		Object * getObjectExpression()  	{   return objExpr.get();    }
		Attribute::flag_t getAttributeFlags()  	{   return attrFlags;    }
		Object * getValueExpression()  		{   return valueExpr.get();    }
		std::string getAttrName()const		{   return identifierIdToString(attrId);    }

		int getLine()const					{	return line;	}

		/// ---|> [Object]
		virtual std::string toString()const;
		virtual internalTypeId_t _getInternalTypeId()const {	return _TypeIds::TYPE_SET_ATTRIBUTE; }

	private:
		friend class Runtime;
		ObjRef objExpr;
		ObjRef valueExpr;
		identifierId attrId;
		Attribute::flag_t attrFlags;
		int line;
		bool assign;
};
}

#endif // SETATTRIBUTE_H
