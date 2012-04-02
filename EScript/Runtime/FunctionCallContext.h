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

#include <stack>

namespace EScript {

/*! [FunctionCallContext]    */
class FunctionCallContext:public EReferenceCounter<FunctionCallContext,FunctionCallContext> {
	private:
		static std::stack<FunctionCallContext *> pool;
	public:
		static FunctionCallContext * create(const EPtr<UserFunction> userFunction,const ObjPtr _caller);
		static void release(FunctionCallContext *rts);

		// ----


	//! @name Main
	// @{
	private:
		enum error_t{
			STACK_EMPTY_ERROR,
			STACK_WRONG_DATA_TYPE,
			UNKNOWN_LOCAL_VARIABLE,
		};
		void throwError(error_t error)const;

//		ObjRef caller;
		_CountedRef<FunctionCallContext> parent;
		ERef<UserFunction> userFunction;
		size_t exceptionHandlerPos;
		size_t instructionCursor;
		ObjRef caller;

		FunctionCallContext() : instructionCursor(0) {}
		~FunctionCallContext(){}
		void reset();
		void init(const EPtr<UserFunction> userFunction,const ObjPtr _caller);
		
		bool constructorCall;
		bool awaitsCaller;
		bool stopExecutionAfterEnding;  //! ... or otherwise, continue with the execution of the parent-context.
	public:
		/*! Marks that the return value of the next returning function call (emerging from this context) should
			be used as this context's calling object. This is the case, if a constructor call has performed its initial steps 
			(e.g. test for parameter types), and now the superconstructor call is called to create the object. */			
		void enableAwaitingCaller()						{	awaitsCaller = true;	}
		void enableStopExecutionAfterEnding()			{	stopExecutionAfterEnding = true;	}
		
		ObjPtr getCaller()const    						{   return caller; }
		ERef<UserFunction> getUserFunction()const		{	return userFunction;	}

		size_t getExceptionHandlerPos()const			{	return exceptionHandlerPos;	}
		size_t getInstructionCursor()const				{	return instructionCursor;	}
		
		//! Set the caller-object; the caller-member as well as the local-'this'-variable
		void initCaller(const ObjPtr _caller);
		bool isAwaitingCaller()const					{	return awaitsCaller;	}
		bool isConstructorCall()const					{	return constructorCall;	}
		bool isExecutionStoppedAfterEnding()const		{	return stopExecutionAfterEnding;	}
		void markAsConstructorCall()					{	constructorCall = true;	}
		void setExceptionHandlerPos(const size_t p)		{	exceptionHandlerPos = p;	}
		void setInstructionCursor(const size_t p)		{	instructionCursor = p;	}
		void increaseInstructionCursor()				{	++instructionCursor;	}

		const InstructionBlock & getInstructions()const	{	return userFunction->getInstructions();	}
		InstructionBlock & getInstructions()			{	return userFunction->getInstructions();	}
//		const Instruction & getNextInstruction()const	{	instructionCursor._accessInstructions(instructionCursor); }
//		bool isInstructionAvailable()const				{	instructionCursor<}
	// @}

	//	-----------------------------

	//! @name Local variables
	// @{
	private:
		std::vector<ObjRef> localVariables;
	public:
		void assignToLocalVariable(const uint32_t index,const ObjPtr value){
			if(index>=localVariables.size())
				throwError(UNKNOWN_LOCAL_VARIABLE);
			localVariables[index] = value;
		}
		ObjPtr getLocalVariable(const uint32_t index)const{
			if(index>=localVariables.size())
				throwError(UNKNOWN_LOCAL_VARIABLE);
			return localVariables[index];
		}
	// @}

	//	-----------------------------

	//! @name Value Stack operations
	// @{
	private:


		struct StackEntry{
			enum dataType_t{
				VOID,
				OBJECT_PTR,
				BOOL,
				UINT32,
				NUMBER,
				IDENTIFIER,
				STRING_IDX, //
//				UINT32_PAIR // \todo coming with c++11
			}dataType;
			union value_t{
				Object * value_ObjPtr;
				bool value_bool;
				uint32_t value_uint32;
				double value_number;
				uint32_t value_indentifier; // \todo c++0x unrestricted union allows StringId
				uint32_t value_stringIndex;
			}value;

			StackEntry(dataType_t _dataType = VOID) : dataType(_dataType) {}
			~StackEntry(){}
			std::string toString()const;
		};
		std::vector<StackEntry> valueStack;

	public:
		void stack_clear() 								{	valueStack.clear();	}
		void stack_dup() {
			StackEntry & entry = stack_top();
			if(entry.dataType == StackEntry::OBJECT_PTR)
				Object::addReference(entry.value.value_ObjPtr);
			valueStack.push_back(entry);
		}
		bool stack_empty()const							{	return valueStack.empty();	}
		void stack_pushBool(const bool value)			{	valueStack.insert(valueStack.end(),StackEntry::BOOL)->value.value_bool = value; }
		void stack_pushFunction(const uint32_t functionIndex){
			UserFunction * uFun = userFunction->getInstructions().getUserFunction(functionIndex);
			Object::addReference(uFun);
			valueStack.insert(valueStack.end(),StackEntry::OBJECT_PTR)->value.value_ObjPtr = uFun;
		}
		void stack_pushNumber(const double & value)		{	valueStack.insert(valueStack.end(),StackEntry::NUMBER)->value.value_number = value; }
		void stack_pushUInt32(const uint32_t value)		{	valueStack.insert(valueStack.end(),StackEntry::UINT32)->value.value_uint32 = value; }
		void stack_pushIdentifier(const StringId & strId){	valueStack.insert(valueStack.end(),StackEntry::IDENTIFIER)->value.value_uint32 = strId.getValue(); }
		void stack_pushStringIndex(const uint32_t value){	valueStack.insert(valueStack.end(),StackEntry::STRING_IDX)->value.value_stringIndex = value; }
		void stack_pushObject(const ObjPtr & obj)	{
			if(obj.isNull()){
				stack_pushVoid();
			}else{
				Object::addReference(obj.get());
				valueStack.insert(valueStack.end(),StackEntry::OBJECT_PTR)->value.value_ObjPtr = obj.get();
			}
		}
		void stack_pushVoid() 							{	valueStack.push_back(StackEntry::VOID);		}

		size_t stack_size()const						{	return valueStack.size();	}
		void stack_pop() {
			StackEntry & entry = stack_top();
			if(entry.dataType == StackEntry::OBJECT_PTR)
				Object::removeReference(entry.value.value_ObjPtr);
			valueStack.pop_back();
		}
		bool stack_popBool(){
			StackEntry & entry = stack_top();
			bool b = false;
			if(entry.dataType == StackEntry::OBJECT_PTR){
				b = entry.value.value_ObjPtr == NULL ? false : entry.value.value_ObjPtr->toBool();
				Object::removeReference(entry.value.value_ObjPtr);
			}else if(entry.dataType == StackEntry::BOOL){
				b = entry.value.value_bool;
			}else if(entry.dataType == StackEntry::VOID){
			}else{
				throwError(STACK_WRONG_DATA_TYPE);
			}
			valueStack.pop_back();
			return b;
		}
		StringId stack_popIdentifier(){
			StackEntry & entry = stack_top();
			if(entry.dataType != StackEntry::IDENTIFIER)
				throwError(STACK_WRONG_DATA_TYPE);
			const StringId id( entry.value.value_indentifier );
			valueStack.pop_back();
			return id;
		}
		uint32_t stack_popUInt32(){
			StackEntry & entry = stack_top();
			if(entry.dataType != StackEntry::UINT32)
				throwError(STACK_WRONG_DATA_TYPE);
			const uint32_t number( entry.value.value_uint32 );
			valueStack.pop_back();
			return number;
		}
		double stack_popNumber(){
			StackEntry & entry = stack_top();
			if(entry.dataType != StackEntry::NUMBER)
				throwError(STACK_WRONG_DATA_TYPE);
			const double number( entry.value.value_number );
			valueStack.pop_back();
			return number;
		}
		uint32_t stack_popStringIndex(){
			StackEntry & entry = stack_top();
			if(entry.dataType != StackEntry::STRING_IDX)
				throwError(STACK_WRONG_DATA_TYPE);
			const uint32_t index( entry.value.value_stringIndex );
			valueStack.pop_back();
			return index;
		}
		Object * stack_popObject();

		//! Works like stack_popObject(), but returns an obj->cloneOrReference() if the contained value is already an Object.
		Object * stack_popObjectValue();

		std::string stack_toDbgString()const;
	private:

		StackEntry & stack_top(){
			if(stack_empty())
				throwError(STACK_EMPTY_ERROR);
			return valueStack.back();
		}
	// @}


};

}

#endif // ES_FUNCTION_CALL_CONTEXT_H
