// UserFunction.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef USERFUNCTION_H
#define USERFUNCTION_H

#include "../ExtObject.h"
#include "../../Instructions/InstructionBlock.h"
#include "../../Utils/CodeFragment.h"
#include <vector>

namespace EScript {

//! Container for static variables shared among several UserFunctions.
class StaticData : public EReferenceCounter<StaticData> {
		std::vector<StringId> staticVariableNames; // currently unused!
		std::vector<ObjRef> staticVariableValues;

	public:
		uint32_t declareStaticVariable(const StringId & name){
			staticVariableNames.emplace_back(name);
			staticVariableValues.emplace_back(nullptr);
			return static_cast<uint32_t>(staticVariableNames.size()-1);
		}
		const std::vector<ObjRef>& getStaticVariableValues()const{	return	staticVariableValues;	}
		bool updateStaticVariable(uint32_t index,Object*value){
			if(index>=staticVariableValues.size())
				return false;
			staticVariableValues[index] = value;
			return true;
		}
};

//! [UserFunction]  ---|> [ExtObject]
class UserFunction : public ExtObject {
		ES_PROVIDES_TYPE_NAME(UserFunction)
	public:
	// -------------------------------------------------------------

	//! @name Initialization
	//	@{
	public:
		static Type* getTypeObject();
		static void init(Namespace & globals);
	//	@}

	// -------------------------------------------------------------

	//! @name Main
	//	@{
	protected:
		UserFunction(const UserFunction & other);
	public:
		UserFunction();
		virtual ~UserFunction()	{ }

		const CodeFragment & getCode()const					{	return codeFragment;	}
		void setCode(const CodeFragment & c)				{	codeFragment = c;	}

		int getMaxParamCount()const							{	return maxParamValueCount;	}
		int getMinParamCount()const							{	return minParamValueCount;	}
		size_t getParamCount()const							{	return paramCount;	}

		void setParameterCounts(size_t paramsCount,int minValues,int maxValues)	{
			paramCount = paramsCount , minParamValueCount = minValues,maxParamValueCount = maxValues;
		}
		const InstructionBlock & getInstructionBlock()const	{	return instructions;	}
		InstructionBlock & getInstructionBlock()			{	return instructions;	}
		int getLine()const									{	return line;	}
		void setLine(const int l)							{	line = l;	}

		//! if multiParam >= paramCount, the additional parameter values are to be ignored. e.g. fn(a,...)
		int getMultiParam()const							{	return multiParam;	}
		void setMultiParam(int i)							{	multiParam = i;	}

		StaticData* getStaticData()const					{	return staticData.get();	}
		void setStaticData(_CountedRef<StaticData> && d)	{	staticData = d;	}

		//! ---|> [Object]
		virtual internalTypeId_t _getInternalTypeId()const	{	return _TypeIds::TYPE_USER_FUNCTION;	}
		virtual UserFunction * clone()const					{	return new UserFunction(*this);	}
		virtual std::string toDbgString()const;
	private:
		CodeFragment codeFragment;
		int line;
		size_t paramCount;
		int minParamValueCount, maxParamValueCount, multiParam;

		InstructionBlock instructions;
		_CountedRef<StaticData> staticData;

	//	@}
};
}

#endif // USERFUNCTION_H
