// SetAttributeExpr.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef SETATTRIBUTE_H
#define SETATTRIBUTE_H

#include "../Object.h"
#include "../../Utils/Attribute.h"
#include <string>

namespace EScript {
namespace AST {
	
/*! [SetAttributeExpr]  ---|> [Object]  */
class SetAttributeExpr : public Object {
		ES_PROVIDES_TYPE_NAME(SetAttributeExpr)
	public:
		static SetAttributeExpr * createAssignment(Object * obj,StringId attrId,Object * valueExp,int _line=-1);

		SetAttributeExpr(Object * obj,StringId attrId,Object * valueExp,Attribute::flag_t _attrFlags,int _line=-1);
		virtual ~SetAttributeExpr();

		StringId getAttrId()const   					{   return attrId;  }
		ObjPtr getObjectExpression()const				{   return objExpr;   }
		Attribute::flag_t getAttributeProperties()const {   return attrFlags;    }
		ObjPtr getValueExpression()const 		 		{   return valueExpr;    }
		std::string getAttrName()const					{   return attrId.toString();    }
		bool isAssignment()const						{   return assign;    }

		int getLine()const								{	return line;	}

		/// ---|> [Object]
		virtual std::string toString()const;
		virtual internalTypeId_t _getInternalTypeId()const {	return _TypeIds::TYPE_SET_ATTRIBUTE_EXPRESSION; }

	private:
		friend class EScript::Runtime;
		ObjRef objExpr;
		ObjRef valueExpr;
		StringId attrId;
		Attribute::flag_t attrFlags;
		int line;
		bool assign;
};
}
}

#endif // SETATTRIBUTE_H
