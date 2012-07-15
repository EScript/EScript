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

		ObjPtr getCaller()const    						{   return caller; }
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
		std::string getLocalVariablesAsString(const bool includeUndefined)const;
	// @}

	//	-----------------------------

	//! @name Value Stack operations
	//! 	\todo use StackEntry to manage obj ownership and return References
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
				STRING_IDX,
				UNDEFINED
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
		void stack_clear();
		void stack_dup() {
			StackEntry & entry = stack_top();
			if(entry.dataType == StackEntry::OBJECT_PTR)
				Object::addReference(entry.value.value_ObjPtr);
			valueStack.push_back(entry);
		}
		bool stack_empty()const							{	return valueStack.empty();	}
		void stack_pushBool(const bool value)			{	valueStack.insert(valueStack.end(),StackEntry::BOOL)->value.value_bool = value; }
		void stack_pushUndefined()						{	valueStack.push_back(StackEntry::UNDEFINED);	 }
		void stack_pushFunction(const uint32_t functionIndex){
			UserFunction * uFun = userFunction->getInstructionBlock().getUserFunction(functionIndex);
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
			}else if(entry.dataType == StackEntry::VOID || entry.dataType == StackEntry::UNDEFINED){
			}else{ // Number || Identifier || STRING_IDX
				b = true;
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
