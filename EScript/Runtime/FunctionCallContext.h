// FunctionCallContext.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_FUNCTION_CALL_CONTEXT_H
#define ES_FUNCTION_CALL_CONTEXT_H

#include "../Objects/Callables/UserFunction.h"
#include "../Utils/EReferenceCounter.h"
#include "../Utils/ObjRef.h"

#include <stack>

namespace EScript {

/*! [FunctionCallContext]    */
class FunctionCallContext:public EReferenceCounter<FunctionCallContext,FunctionCallContext> {
	private:
		static std::stack<FunctionCallContext *> pool;
	public:
		static FunctionCallContext * create(FunctionCallContext * _parent,const EPtr<UserFunction> & userFunction);
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
				
		ObjRef caller;
		_CountedRef<FunctionCallContext> parent;
		ERef<UserFunction> userFunction;
		size_t instructionCursor;

		FunctionCallContext() : instructionCursor(0) {}
		~FunctionCallContext(){}
		void reset();
		void init(FunctionCallContext * _parent,const EPtr<UserFunction> & userFunction);

	public:
		ObjPtr getCaller()const    						{   return caller; }
		FunctionCallContext * getParent()const			{	return parent.get();	}
		ERef<UserFunction> getUserFunction()const		{	return userFunction;	}
		
		size_t getInstructionCursor()const				{	return instructionCursor;	}
		void setInstructionCursor(const size_t p)		{	instructionCursor=p;	}
		void increaseInstructionCursor()				{	++instructionCursor;	}

		void setCaller(const ObjPtr & _caller)			{	caller = _caller;	}

		const InstructionBlock & getInstructions()const	{	return userFunction->getInstructions();	}
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
	
	//! @name Stack operations
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
		void stack_dup() {	
			if(stack_empty())
				throwError(STACK_EMPTY_ERROR);
			valueStack.push_back(valueStack.back());
		}
		bool stack_empty()const							{	return valueStack.empty();	}
		void stack_pushBool(const bool value)			{	valueStack.insert(valueStack.end(),StackEntry::BOOL)->value.value_bool = value; }
		void stack_pushNumber(const double & value)		{	valueStack.insert(valueStack.end(),StackEntry::NUMBER)->value.value_number = value; }
		void stack_pushUInt32(const uint32_t value)		{	valueStack.insert(valueStack.end(),StackEntry::UINT32)->value.value_uint32 = value; }
		void stack_pushIdentifier(const StringId & strId){	valueStack.insert(valueStack.end(),StackEntry::IDENTIFIER)->value.value_uint32 = strId.getValue(); }
		void stack_pushStringIndex(const uint32_t value){	valueStack.insert(valueStack.end(),StackEntry::STRING_IDX)->value.value_stringIndex = value; }
		void stack_pushObject(Object * obj)	{	
			Object::addReference(obj);
			valueStack.insert(valueStack.end(),StackEntry::OBJECT_PTR)->value.value_ObjPtr = obj; 
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
			bool b;
			if(entry.dataType == StackEntry::OBJECT_PTR){
				b = entry.value.value_ObjPtr == NULL ? false : entry.value.value_ObjPtr->toBool();
				Object::removeReference(entry.value.value_ObjPtr);
			}else if(entry.dataType == StackEntry::BOOL){
				b = entry.value.value_bool;
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
