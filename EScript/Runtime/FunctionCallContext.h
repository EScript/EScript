// FunctionCallContext.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_FUNCTION_CALL_CONTEXT_H
#define ES_FUNCTION_CALL_CONTEXT_H

#include "../Objects/Callables/UserFunction.h"
#include "../Utils/EReferenceCounter.h"
#include "../Utils/ObjRef.h"
#include "../Consts.h"
#include "RtValue.h"

#include <stack>

namespace EScript {

//! [FunctionCallContext]
class FunctionCallContext:public EReferenceCounter<FunctionCallContext,FunctionCallContext> {
	private:
		static std::stack<FunctionCallContext *> pool;
	public:
		static FunctionCallContext * create(const EPtr<UserFunction> userFunction,const ObjPtr _caller);
		static void release(FunctionCallContext *rts);

		// ----


	//! @name Main
	// @{
	public:
		typedef std::vector<Instruction>::const_iterator instructionCursor_t;
	private:
		enum error_t{
			STACK_EMPTY_ERROR,
			STACK_WRONG_DATA_TYPE,
			UNKNOWN_LOCAL_VARIABLE
		};
		void throwError(error_t error)const;

		ERef<UserFunction> userFunction;
		size_t exceptionHandlerPos;
		instructionCursor_t instructionCursor;
		ObjRef caller;

		FunctionCallContext()  {}
		~FunctionCallContext(){}
		void reset();
		void init(const EPtr<UserFunction> userFunction,const ObjPtr _caller);

		bool constructorCall;
		bool providesCallerAsResult;
		bool stopExecutionAfterEnding;  //! ... or otherwise, continue with the execution of the parent-context.
	public:
		/*! Marks that the return value of the context should be used as the calling context's calling object.
			This is the case, if the context belongs to a superconstructor call. */
		void markAsProvidingCallerAsResult()			{	providesCallerAsResult = true;	}
		void enableStopExecutionAfterEnding()			{	stopExecutionAfterEnding = true;	}

		ObjPtr getCaller()const							{	return caller; }
		int getCurrentLine()const{
			return instructionCursor == getInstructions().end() ? -1 : instructionCursor->getLine();
		}
		size_t getExceptionHandlerPos()const			{	return exceptionHandlerPos;	}
		const InstructionBlock & getInstructionBlock()const		{	return userFunction->getInstructionBlock();	}
		const instructionCursor_t & getInstructionCursor()const	{	return instructionCursor;	}
		const std::vector<Instruction> & getInstructions()const	{	return userFunction->getInstructionBlock().getInstructions();	}
		EPtr<UserFunction> getUserFunction()const		{	return userFunction;	}

		void increaseInstructionCursor()				{	++instructionCursor;	}

		//! Set the caller-object; the caller-member as well as the local-'this'-variable
		void initCaller(const ObjPtr _caller);
		bool isConstructorCall()const					{	return constructorCall;	}
		bool isExecutionStoppedAfterEnding()const		{	return stopExecutionAfterEnding;	}
		bool isProvidingCallerAsResult()const			{	return providesCallerAsResult;	}
		void markAsConstructorCall()					{	constructorCall = true;	}
		void setExceptionHandlerPos(const size_t p)		{	exceptionHandlerPos = p;	}
		void setInstructionCursor(const size_t p){
			const std::vector<Instruction> & instructions = getInstructions();
			instructionCursor = (p>=instructions.size() ? instructions.end() : instructions.begin()+p);
		}
	// @}

	//	-----------------------------

	//! @name Local variables
	// @{
	private:
		std::vector<ObjRef> localVariables;
	public:
		void assignToLocalVariable(const uint32_t index, ObjRef && value){
			if(index>=localVariables.size())
				throwError(UNKNOWN_LOCAL_VARIABLE);
			localVariables[index] = std::move(value);
		}
		void assignToLocalVariable(const uint32_t index, const ObjRef & value){
			if(index>=localVariables.size())
				throwError(UNKNOWN_LOCAL_VARIABLE);
			localVariables[index] = value;
		}
		Object * getLocalVariable(const uint32_t index)const{
			if(index>=localVariables.size())
				throwError(UNKNOWN_LOCAL_VARIABLE);
			return localVariables[index].get();
		}
		std::string getLocalVariablesAsString(const bool includeUndefined)const;
		void resetLocalVariable(const uint32_t index){
			if(index>=localVariables.size())
				throwError(UNKNOWN_LOCAL_VARIABLE);
			localVariables[index] = nullptr;
		}
		StringId getLocalVariableName(const uint32_t index)const{
			return getInstructionBlock().getLocalVariables().at(index);
		}
	// @}

	//	-----------------------------

	//! @name RtValue Stack operations
	// @{
	private:
		std::vector<RtValue> valueStack;

	public:
		void stack_clear();
		void stack_dup()								{		valueStack.emplace_back(stack_top());	}
		bool stack_empty()const							{	return valueStack.empty();	}
		void stack_pushBool(const bool value)			{	valueStack.emplace_back(value); }
		void stack_pushUndefined()						{	valueStack.emplace_back(RtValue());	 }
		void stack_pushFunction(const uint32_t functionIndex){
			valueStack.emplace_back(userFunction->getInstructionBlock().getUserFunction(functionIndex));
		}
		void stack_pushNumber(const double & value)		{	valueStack.emplace_back(value); }
		void stack_pushUInt32(const uint32_t value)		{	valueStack.emplace_back(value); }
		void stack_pushIdentifier(const StringId & strId){	valueStack.emplace_back(strId); }
		void stack_pushStringIndex(const uint32_t value){	valueStack.emplace_back(RtValue::createLocalStringIndex(value)); }
		void stack_pushObject(const ObjPtr & obj)		{	valueStack.emplace_back(obj.get());	}
		void stack_pushValue(RtValue && value)			{	valueStack.emplace_back(std::move(value));	}
		void stack_pushVoid()							{	valueStack.emplace_back(nullptr);		}

		size_t stack_size()const						{	return valueStack.size();	}
		void stack_pop()								{	valueStack.pop_back();	}
		bool stack_popBool(){
			const bool b = stack_top().toBool();
			valueStack.pop_back();
			return b;
		}
		StringId stack_popIdentifier(){
			RtValue & entry = stack_top();
			if(!entry.isIdentifier())
				throwError(STACK_WRONG_DATA_TYPE);
			const StringId id( entry._getIdentifier() );
			valueStack.pop_back();
			return id;
		}
		uint32_t stack_popUInt32(){
			RtValue & entry = stack_top();
			if(!entry.isUint32())
				throwError(STACK_WRONG_DATA_TYPE);
			const uint32_t number( entry._getUInt32() );
			valueStack.pop_back();
			return number;
		}
		double stack_popNumber(){
			RtValue & entry = stack_top();
			if(!entry.isNumber())
				throwError(STACK_WRONG_DATA_TYPE);
			const double number( entry._getNumber() );
			valueStack.pop_back();
			return number;
		}
		uint32_t stack_popStringIndex(){
			RtValue & entry = stack_top();
			if(!entry.isLocalString())
				throwError(STACK_WRONG_DATA_TYPE);
			const uint32_t index( entry._getLocalStringIndex() );
			valueStack.pop_back();
			return index;
		}
		ObjRef stack_popObject(){
			ObjRef obj;
			RtValue & top = stack_top();
			if(top.isObject()){ // fast path
				obj._set( top._detachObject() );
			}else{
				obj =  std::move(rtValueToObject(top));
			}
			valueStack.pop_back();
			return obj;
		}
		ObjRef rtValueToObject(RtValue & value);

		/*! Works almost like stack_popObject(), but:
			- returns an obj->cloneOrReference() if the contained value is already an Object.
			- returns nullptr (and not Void) iff the value is undefined.	This is necessary to detect undefined parameters. */
		ObjRef stack_popObjectValue();
		
		RtValue stack_popValue(){
			const RtValue v = stack_top();
			valueStack.pop_back();
			return v;
		}

		std::string stack_toDbgString()const;
	private:

		RtValue & stack_top(){
			if(stack_empty())
				throwError(STACK_EMPTY_ERROR);
			return valueStack.back();
		}
	// @}


};

}

#endif // ES_FUNCTION_CALL_CONTEXT_H
