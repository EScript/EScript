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

namespace EScript{

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
ObjRef FunctionCallContext::rtValueToObject(RtValue & entry){
	switch(entry.valueType){
	case RtValue::VOID_VALUE:
		return Void::get();
	case RtValue::OBJECT_PTR:{
		ObjRef result(std::move( entry._detachObject() ));
		return result;
	}
	case RtValue::BOOL:{
		return Bool::create(entry._getBool());
	}
	case RtValue::UINT32:{
		return Number::create(entry._getUInt32());
	}
	case RtValue::NUMBER:{
		return Number::create(entry._getNumber());
	}
	case RtValue::IDENTIFIER:{
		return Identifier::create(StringId(entry._getIdentifier()));
	}
	case RtValue::LOCAL_STRING_IDX:{
		return String::create(getInstructionBlock().getStringConstant(entry._getLocalStringIndex()));
	}
	case RtValue::FUNCTION_CALL_CONTEXT:
	case RtValue::UNDEFINED:
	default:;
	}
	return Void::get();
}
ObjRef FunctionCallContext::stack_popObjectValue(){
	RtValue & entry = stack_top();
	ObjRef obj;
	switch(entry.valueType){
	case RtValue::VOID_VALUE:
		obj = Void::get();
		break;
	case RtValue::OBJECT_PTR:{
		obj = entry.getObject()->getRefOrCopy();
		break;
	}
	case RtValue::BOOL:{
		obj = Bool::create(entry._getBool());
		break;
	}
	case RtValue::UINT32:{
		obj = Number::create(entry._getUInt32());
		break;
	}
	case RtValue::NUMBER:{
		obj = Number::create(entry._getNumber());
		break;
	}
	case RtValue::IDENTIFIER:{
		obj = Identifier::create(StringId(entry._getIdentifier()));
		break;
	}
	case RtValue::LOCAL_STRING_IDX:{
		obj = String::create(getInstructionBlock().getStringConstant(entry._getLocalStringIndex()));
		break;
	}
	case RtValue::FUNCTION_CALL_CONTEXT:
	case RtValue::UNDEFINED:{
//		std::cout << "popUndefined";
	}
	default:; // Important: obj remains nullptr!

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

std::string  FunctionCallContext::stack_toDbgString()const{
	std::ostringstream out;
	out<<"[";
	std::vector<RtValue>::const_iterator it = valueStack.begin();
	if(it!=valueStack.end()){
		out << (*it).toDbgString();
		++it;
	}
	for(;it!=valueStack.end();++it)
		out << ", "<<(*it).toDbgString();
	out << "]";
	return out.str();
}
}
