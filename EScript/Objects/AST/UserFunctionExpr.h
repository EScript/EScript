// UserFunctionExpr.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_USERFUNCTION_EXPR_H
#define ES_USERFUNCTION_EXPR_H

#include "../ExtObject.h"
#include <vector>

namespace EScript {
namespace AST{
class BlockStatement;
}
class String;

/*! [UserFunctionExpr]  ---|> [ExtObject]	*/
class UserFunctionExpr : public ExtObject {
		ES_PROVIDES_TYPE_NAME(UserFunctionExpr)
	public:
	// -------------------------------------------------------------

	/*! @name Parameter */
	//	@{

		/*! [Parameter] */
		class Parameter {
			private:
				StringId name;
				ObjRef defaultValueExpressionRef;
				ObjRef typeRef;
				bool multiParam;
			public:
				explicit Parameter(const StringId & name,Object * defaultValueExpression=NULL,Object * type=NULL);
				std::string toString()const;

				Parameter* clone()const;
				StringId getName()const						{   return name;    }
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

		typedef std::vector<Parameter> parameterList_t;
	//	@}

	// -------------------------------------------------------------

	/*! @name Main */
	//	@{
		UserFunctionExpr(AST::BlockStatement * block);
		UserFunctionExpr(AST::BlockStatement * block,const std::vector<ObjRef> & _sConstrExpressions);
		virtual ~UserFunctionExpr() {}

		void setBlock(AST::BlockStatement * block);
		AST::BlockStatement * getBlock()const				{	return blockRef.get();	}
		const parameterList_t & getParamList()const			{	return params;	}
		parameterList_t & getParamList()					{	return params;	}
		std::string getFilename()const;
		int getLine()const;

		std::vector<ObjRef> & getSConstructorExpressions() 	{	return sConstrExpressions;	}

		void setCodeString(const EPtr<String> & _fileString,size_t _begin,size_t _codeLen);
		std::string getCode()const;
		int getMaxParamCount()const;
		int getMinParamCount()const;
	
		/// ---|> [Object]
		virtual std::string toDbgString()const;
		virtual internalTypeId_t _getInternalTypeId()const 	{	return _TypeIds::TYPE_USER_FUNCTION_EXPRESSION;	}

	private:
		ERef<AST::BlockStatement> blockRef;
		parameterList_t params; // \todo should this really saved as a pointer???
		std::vector<ObjRef> sConstrExpressions;

		ERef<String> fileString;
		size_t posInFile,codeLen;
	//	@}
};
}

#endif // ES_USERFUNCTION_EXPR_H
