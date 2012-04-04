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

	/*! @name Main */
	//	@{
	protected:
		UserFunction(const UserFunction & other);
	public:
		UserFunction();
		virtual ~UserFunction();

		std::string getFilename()const;

		void setCodeString(const EPtr<String> & _fileString,size_t _begin,size_t _codeLen);
		std::string getCode()const;
		int getMaxParamCount()const							{	return maxParamValueCount;	}
		int getMinParamCount()const							{	return minParamValueCount;	}
		size_t getParamCount()const							{	return paramCount;	}

		void setParameterCounts(size_t paramsCount,int minValues,int maxValues)	{	
			paramCount = paramsCount , minParamValueCount = minValues,maxParamValueCount = maxValues;	
		}
		const InstructionBlock & getInstructions()const 	{	return instructions;	}
		InstructionBlock & getInstructions() 				{	return instructions;	}
	
	
		/// ---|> [Object]
		virtual internalTypeId_t _getInternalTypeId()const 	{	return _TypeIds::TYPE_USER_FUNCTION;	}
		virtual UserFunction * clone()const					{	return new UserFunction(*this);	}

	private:
		ERef<String> fileString;
		size_t posInFile,codeLen;
		size_t paramCount;
		int minParamValueCount;
		int maxParamValueCount;

		InstructionBlock instructions;
	//	@}
};
}

#endif // USERFUNCTION_H
