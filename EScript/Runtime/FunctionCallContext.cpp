// FunctionCallContext.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "FunctionCallContext.h"

#include "../Consts.h"
#include "../Objects/Identifier.h"
#include "../Objects/Values/Bool.h"
#include "../Objects/Values/Number.h"
#include "../Objects/Values/String.h"
#include "../Objects/Values/Void.h"
#include <stdexcept>
#include <sstream>

using namespace EScript;

std::stack<FunctionCallContext *> FunctionCallContext::pool;

//! (static) Factory
FunctionCallContext * FunctionCallContext::create(FunctionCallContext * _parent,const EPtr<UserFunction> & userFunction){
	FunctionCallContext * fcc=NULL;
	if(pool.empty()){
		fcc=new FunctionCallContext();
	}else{
		fcc=pool.top();
		pool.pop();
	}
//	assert(userFunction.isNotNull()); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	fcc->init(_parent,userFunction);
	return fcc;
}

//! static
void FunctionCallContext::release(FunctionCallContext *fcc){
	pool.push(fcc);
	fcc->reset();
}

// -------------------------------------------------------------------------

void FunctionCallContext::init(FunctionCallContext * _parent,const EPtr<UserFunction> & _userFunction){
	parent = _parent;
	userFunction = _userFunction;
	instructionCursor = 0;
	
	localVariables.resize(userFunction->getInstructions().getNumLocalVars());
//	std::fill(localVariables.begin(),localVariables.end(),NULL);
}

void FunctionCallContext::reset(){
	caller = NULL;
	parent = NULL;
	userFunction = NULL;
	localVariables.clear();
	while(!valueStack.empty())
		stack_pop();
}

Object * FunctionCallContext::stack_popObject(){
	StackEntry & entry = stack_top();
	Object * obj;
	switch(entry.dataType){
	case StackEntry::VOID:
		obj = NULL;
		break;
	case StackEntry::OBJECT_PTR:{
		obj = entry.value.value_ObjPtr;
		Object::decreaseReference(obj);
		break;
	}
	case StackEntry::BOOL:{
		obj = Bool::create(entry.value.value_bool);
		break;
	}
	case StackEntry::UINT32:{
		obj = Number::create(entry.value.value_uint32);
		break;
	}
	case StackEntry::NUMBER:{
		obj = Number::create(entry.value.value_number);
		break;
	}
	case StackEntry::IDENTIFIER:{
		obj = Identifier::create(StringId(entry.value.value_indentifier));
		break;
	}	
	case StackEntry::STRING_IDX:{
		obj = String::create(getInstructions().getStringConstant(entry.value.value_stringIndex));
		break;
	}
	default:
		obj = NULL;
	}
	valueStack.pop_back();
	return obj ? obj : Void::get();
}
Object * FunctionCallContext::stack_popObjectValue(){
	StackEntry & entry = stack_top();
	
	switch(entry.dataType){
	case StackEntry::VOID:
		valueStack.pop_back();
		return Void::get();
	case StackEntry::OBJECT_PTR:{
		ObjRef obj = entry.value.value_ObjPtr; //! \todo optimize!
		if(obj.isNotNull()){
			Object::decreaseReference(obj.get());
			obj = obj->getRefOrCopy();
		}
		valueStack.pop_back();
		return obj.detachAndDecrease();
	}
	case StackEntry::BOOL:{
		const bool b = entry.value.value_bool;
		valueStack.pop_back();
		return Bool::create(b);
	}
	case StackEntry::UINT32:{
		const bool value = entry.value.value_uint32;
		valueStack.pop_back();
		return Number::create(value);		
	}
	case StackEntry::NUMBER:{
		const double value = entry.value.value_number;
		valueStack.pop_back();
		return Number::create(value);		
	}
	case StackEntry::IDENTIFIER:{
		const StringId value(entry.value.value_indentifier);
		valueStack.pop_back();
		return Identifier::create(value);
	}	
	case StackEntry::STRING_IDX:{
		const uint32_t value = entry.value.value_stringIndex;
		valueStack.pop_back();
		return String::create(getInstructions().getStringConstant(value));
	}
	default:;
	}
	
	return Void::get();
}

void FunctionCallContext::throwError(FunctionCallContext::error_t error)const{
	static const std::string prefix("Internal error: ");
	switch(error){
		case STACK_EMPTY_ERROR:
			throw std::logic_error(prefix+"Empty stack.");		
		case STACK_WRONG_DATA_TYPE:
			throw std::logic_error(prefix+"Wrong data type on stack.");		
		case UNKNOWN_LOCAL_VARIABLE:
			throw std::logic_error(prefix+"Invalid local variable.");
		default:
			throw std::logic_error(prefix+"???");
	}
}

std::string FunctionCallContext::StackEntry::toString()const{
	std::ostringstream out;
	switch(dataType){
	case VOID:{
		out << "Void";
		break;
	}
	case OBJECT_PTR:{
		out << value.value_ObjPtr;
		break;
	}
	case BOOL:{
		out << (value.value_bool ? "true" : "false");
		break;
	}
	case UINT32:{
		out << value.value_uint32;
		break;
	}
	case NUMBER:{
		out << value.value_number;
		break;
	}
	case IDENTIFIER:{
		out << StringId(value.value_indentifier).toString();
		break;
	}
	case STRING_IDX:{
		out << "#" << value.value_stringIndex;
		break;
	}
	default:
		out << "???";
	}
	return out.str();
}
std::string  FunctionCallContext::stack_toDbgString()const{
	std::ostringstream out;
	out<<"[";
	std::vector<StackEntry>::const_iterator it = valueStack.begin();
	if(it!=valueStack.end()){
		out << (*it).toString();
		++it;
	}
	for(;it!=valueStack.end();++it)
		out << ", "<<(*it).toString();
	out << "]";
	return out.str();
}