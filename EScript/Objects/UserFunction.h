// UserFunction.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef USERFUNCTION_H
#define USERFUNCTION_H

#include "ExtObject.h"
#include <vector>

namespace EScript {

class Block;

/*! [UserFunction]  ---|> [ExtObject]	*/
class UserFunction : public ExtObject {
		ES_PROVIDES_TYPE_NAME(UserFunction)
	public:

		/*! [Parameter] */
		class Parameter {
			private:
				identifierId name;
				ObjRef defaultValueExpressionRef;
				ObjRef typeRef;
				bool multiParam;
			public:
				Parameter(identifierId  name,Object * defaultValueExpression=NULL,Object * type=NULL);
				~Parameter();
				std::string toString()const;

				identifierId getName()const					{   return name;    }
				Object * getType()const						{   return typeRef.get();   }

				void setMultiParam(bool b)					{   multiParam=b;   }
				bool isMultiParam()const					{   return multiParam;  }

				Object * getDefaultValueExpression()const {
					return defaultValueExpressionRef.get();
				}
				void setDefaultValueExpression(Object * newDefaultExpression) {
					defaultValueExpressionRef=newDefaultExpression;
				}

		};

		typedef std::vector<Parameter *> parameterList_t;

		UserFunction(parameterList_t * params,Block * block);
		UserFunction(parameterList_t * params,Block * block,const std::vector<ObjRef> & _sConstrExpressions);
		virtual ~UserFunction();

		void setBlock(Block * block);
		Block * getBlock()const								{	return blockRef.get();	}
		parameterList_t * getParamList()const				{	return params;	}
		std::string getFilename()const;
		int getLine()const;

		std::vector<ObjRef> & getSConstructorExpressions() 	{	return sConstrExpressions;	}

		/// ---|> [Object]
		virtual std::string toString()const;
		virtual std::string toDbgString()const;
		virtual internalTypeId_t _getInternalTypeId()const 	{	return _TypeIds::TYPE_USER_FUNCTION;	}

	private:
		ERef<Block> blockRef;
		parameterList_t * params;
		std::vector<ObjRef> sConstrExpressions;
};
}

#endif // USERFUNCTION_H
