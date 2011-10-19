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
class String;

/*! [UserFunction]  ---|> [ExtObject]	*/
class UserFunction : public ExtObject {
		ES_PROVIDES_TYPE_NAME(UserFunction)
	public:
	// -------------------------------------------------------------

	/*! @name Initialization */
	//	@{
	public:
		static Type * getTypeObject();
		static void init(Namespace & globals);
	//	@}

	// -------------------------------------------------------------

	/*! @name Parameter */
	//	@{
	
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

				Parameter* clone()const;
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
	//	@}

	// -------------------------------------------------------------

	/*! @name Main */
	//	@{
	
		UserFunction(parameterList_t * params,Block * block);
		UserFunction(parameterList_t * params,Block * block,const std::vector<ObjRef> & _sConstrExpressions);
		virtual ~UserFunction();

		void setBlock(Block * block);
		Block * getBlock()const								{	return blockRef.get();	}
		parameterList_t * getParamList()const				{	return params;	}
		std::string getFilename()const;
		int getLine()const;

		std::vector<ObjRef> & getSConstructorExpressions() 	{	return sConstrExpressions;	}
		
		void setCodeString(const EPtr<String> & _fileString,size_t _begin,size_t _codeLen);
		std::string getCode()const;
		int getMaxParamCount()const;
		int getMinParamCount()const;

		/// ---|> [Object]
		virtual UserFunction * clone()const;
		virtual std::string toDbgString()const;
		virtual internalTypeId_t _getInternalTypeId()const 	{	return _TypeIds::TYPE_USER_FUNCTION;	}

	private:
		ERef<Block> blockRef;
		parameterList_t * params;
		std::vector<ObjRef> sConstrExpressions;
		
		ERef<String> fileString;
		size_t posInFile,codeLen;

	//	@}		
};
}

#endif // USERFUNCTION_H
