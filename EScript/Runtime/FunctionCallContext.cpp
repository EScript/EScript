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
FunctionCallContext * FunctionCallContext::create(const EPtr<UserFunction> userFunction,const ObjPtr _caller){
	FunctionCallContext * fcc = nullptr;
	if(pool.empty()){
		fcc = new FunctionCallContext;
	}else{
		fcc = pool.top();
		pool.pop();
	}
//	assert(userFunction.isNotNull()); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	fcc->init(userFunction,_caller);
	return fcc;
}

//! static
void FunctionCallContext::release(FunctionCallContext *fcc){
	pool.push(fcc);
	fcc->reset();
}

// -------------------------------------------------------------------------

std::string FunctionCallContext::getLocalVariablesAsString(const bool includeUndefined)const{
	const std::vector<StringId> & vars = getInstructionBlock().getLocalVariables();
	std::ostringstream os;
	for(size_t i = 0;i<vars.size();++i ){
		ObjPtr value = getLocalVariable(i);
		if(value.isNull() && !includeUndefined )
			continue;
//		os << '$' << vars[i].toString() << '=' << (value.isNotNull() ? value->toDbgString() : "undefined" )<< '\t';
		os << '$' << vars[i].toString() << '=' << value.toString("undefined") << '\t';
	}
	return os.str();

}

void FunctionCallContext::init(const EPtr<UserFunction> _userFunction,const ObjPtr _caller){
	caller = _caller;
	userFunction = _userFunction;
	instructionCursor = getInstructions().begin();
	constructorCall = false;
	providesCallerAsResult = false;
	stopExecutionAfterEnding = false;
	exceptionHandlerPos = Instruction::INVALID_JUMP_ADDRESS;

	localVariables.resize(getInstructionBlock().getNumLocalVars());

	localVariables[Consts::LOCAL_VAR_INDEX_this] = caller; // ?????????????????
	localVariables[Consts::LOCAL_VAR_INDEX_thisFn] = userFunction.get();
}
void FunctionCallContext::initCaller(const ObjPtr _caller){
	caller = _caller;
	localVariables[Consts::LOCAL_VAR_INDEX_this] = caller;
}


void FunctionCallContext::reset(){
	caller = nullptr;
	userFunction = nullptr;
	localVariables.clear();
	while(!valueStack.empty())
		stack_pop();
}
void FunctionCallContext::stack_clear(){
	while(!valueStack.empty()){
		stack_pop();
	}
}
ObjRef FunctionCallContext::stack_popObject(){
	StackEntry & entry = stack_top();
	ObjRef obj;
	switch(entry.valueType){
	case StackEntry::VOID:
		obj = Void::get();
		break;
	case StackEntry::OBJECT_PTR:{ // move object
		obj._set( entry.value.value_obj );
		entry.valueType = StackEntry::UNDEFINED;
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
	case StackEntry::LOCAL_STRING_IDX:{
		obj = String::create(getInstructionBlock().getStringConstant(entry.value.value_localStringIndex));
		break;
	}
	case StackEntry::UNDEFINED:{
//		std::cout << "popUndefined";
	}
	default:
		obj = Void::get();
	}
	valueStack.pop_back();
	return obj;
}
ObjRef FunctionCallContext::stack_popObjectValue(){
	StackEntry & entry = stack_top();
	ObjRef obj;
	switch(entry.valueType){
	case StackEntry::VOID:
		obj = Void::get();
		break;
	case StackEntry::OBJECT_PTR:{
		obj = entry.getObject()->getRefOrCopy();
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
	case StackEntry::LOCAL_STRING_IDX:{
		obj = String::create(getInstructionBlock().getStringConstant(entry.value.value_localStringIndex));
		break;
	}
	case StackEntry::UNDEFINED:{
//		std::cout << "popUndefined";
	}
	default:;
//		obj = Void::get();
	}
	valueStack.pop_back();
	return obj;
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

//std::string FunctionCallContext::StackEntry::toString()const{
//	std::ostringstream out;
//	switch(dataType){
//	case VOID:{
//		out << "Void";
//		break;
//	}
//	case OBJECT_PTR:{
//		out << value.value_ObjPtr;
//		break;
//	}
//	case BOOL:{
//		out << (value.value_bool ? "true" : "false");
//		break;
//	}
//	case UINT32:{
//		out << value.value_uint32;
//		break;
//	}
//	case NUMBER:{
//		out << value.value_number;
//		break;
//	}
//	case IDENTIFIER:{
//		out << StringId(value.value_indentifier).toString();
//		break;
//	}
//	case STRING_IDX:{
//		out << "#" << value.value_stringIndex;
//		break;
//	}
//	case UNDEFINED:{
//		out << "UNDEFINED";
//		break;
//	}
//	default:
//		out << "???";
//	}
//	return out.str();
//}
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
