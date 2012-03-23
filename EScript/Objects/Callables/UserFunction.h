// UserFunction.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef USERFUNCTION_H
#define USERFUNCTION_H

#include "../ExtObject.h"
#include "../../Instructions/InstructionBlock.h"
#include <vector>

namespace EScript {
namespace AST{
class BlockStatement;
}
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

		/*! [Parameter] */  // \todo old!
		class Parameter {
			private:
				StringId name;
				ObjRef defaultValueExpressionRef;
				ObjRef typeRef;
				bool multiParam;
			public:
				explicit Parameter(const StringId & name,Object * defaultValueExpression=NULL,Object * type=NULL);
				~Parameter();
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

		typedef std::vector<Parameter *> parameterList_t;
	//	@}

	// -------------------------------------------------------------

	/*! @name Main */
	//	@{
		UserFunction(parameterList_t * params,AST::BlockStatement * block);
		UserFunction(parameterList_t * params,AST::BlockStatement * block,const std::vector<ObjRef> & _sConstrExpressions);
		virtual ~UserFunction();

		void setBlock(AST::BlockStatement * block); // \todo old!
		AST::BlockStatement * getBlock()const				{	return blockRef.get();	}  // \todo old!
		parameterList_t * getParamList()const				{	return params;	}  // \todo old!
		std::string getFilename()const;
		int getLine()const;

		std::vector<ObjRef> & getSConstructorExpressions() 	{	return sConstrExpressions;	}  // \todo old!

		void setCodeString(const EPtr<String> & _fileString,size_t _begin,size_t _codeLen);
		std::string getCode()const;
		int getMaxParamCount()const;
		int getMinParamCount()const;

		const InstructionBlock & getInstructions()const 	{	return instructions;	}
		InstructionBlock & getInstructions() 				{	return instructions;	}
		void emplaceInstructions( InstructionBlock & source){	instructions.emplace(source);	}
	
	
		/// ---|> [Object]
		virtual UserFunction * clone()const;
		virtual std::string toDbgString()const;
		virtual internalTypeId_t _getInternalTypeId()const 	{	return _TypeIds::TYPE_USER_FUNCTION;	}

	private:
		void initInstructions();
		ERef<AST::BlockStatement> blockRef;  // \todo old!
		parameterList_t * params;  // \todo old!
		std::vector<ObjRef> sConstrExpressions;  // \todo old!

		ERef<String> fileString;
		size_t posInFile,codeLen;

		InstructionBlock instructions;
	//	@}
};
}

#endif // USERFUNCTION_H
