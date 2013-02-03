// Instruction.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Instruction.h"
#include "InstructionBlock.h"
#include <iostream>
#include <sstream>

namespace EScript {

//! (static)
Instruction Instruction::createAssignAttribute(const StringId & varName){
	Instruction i(I_ASSIGN_ATTRIBUTE);
	i.setValue_Identifier(varName);
	return i;
}

//! (static)
Instruction Instruction::createAssignLocal(const uint32_t localVarIdx){
	Instruction i(I_ASSIGN_LOCAL);
	i.setValue_uint32(localVarIdx);
	return i;
}

//! (static)
Instruction Instruction::createAssignVariable(const StringId & varName){
	Instruction i(I_ASSIGN_VARIABLE);
	i.setValue_Identifier(varName);
	return i;
}


//! (static)
Instruction Instruction::createCall(const uint32_t numParams){
	Instruction i(I_CALL);
	i.setValue_uint32(numParams);
	return i;
}

//! (static)
Instruction Instruction::createCheckType(const uint32_t localVarIdx){
	Instruction i(I_CHECK_TYPE);
	i.setValue_uint32(localVarIdx);
	return i;
}

//! (static)
Instruction Instruction::createCreateInstance(const uint32_t numParams){
	Instruction i(I_CREATE_INSTANCE);
	i.setValue_uint32(numParams);
	return i;
}

//! (static)
Instruction Instruction::createFindVariable(const StringId & id){
	Instruction i(I_FIND_VARIABLE);
	i.setValue_Identifier(id);
	return i;
}

//! (static)
Instruction Instruction::createGetAttribute(const StringId & id){
	Instruction i(I_GET_ATTRIBUTE);
	i.setValue_Identifier(id);
	return i;
}

//! (static)
Instruction Instruction::createGetLocalVariable(const uint32_t localVarIdx){
	Instruction i(I_GET_LOCAL_VARIABLE);
	i.setValue_uint32(localVarIdx);
	return i;
}

//! (static)
Instruction Instruction::createGetVariable(const StringId & id){
	Instruction i(I_GET_VARIABLE);
	i.setValue_Identifier(id);
	return i;
}

//! (static)
Instruction Instruction::createInitCaller(const uint32_t numSuperParams){
	Instruction i(I_INIT_CALLER);
	i.setValue_uint32(numSuperParams);
	return i;
}

//! (static)
Instruction Instruction::createJmp(const uint32_t markerId){
	Instruction i(I_JMP);
	i.setValue_uint32(markerId);
	return i;
}

//! (static)
Instruction Instruction::createJmpIfSet(const uint32_t markerId){
	Instruction i(I_JMP_IF_SET);
	i.setValue_uint32(markerId);
	return i;
}
//! (static)
Instruction Instruction::createJmpOnTrue(const uint32_t markerId){
	Instruction i(I_JMP_ON_TRUE);
	i.setValue_uint32(markerId);
	return i;
}

//! (static)
Instruction Instruction::createJmpOnFalse(const uint32_t markerId){
	Instruction i(I_JMP_ON_FALSE);
	i.setValue_uint32(markerId);
	return i;
}


//! (static)
Instruction Instruction::createPushBool(const bool value){
	Instruction i(I_PUSH_BOOL);
	i.setValue_Bool(value);
	return i;
}

//! (static)
Instruction Instruction::createPushId(const StringId & id){
	Instruction i(I_PUSH_ID);
	i.setValue_Identifier(id);
	return i;
}

//! (static)
Instruction Instruction::createPushFunction(const uint32_t functionIndex){
	Instruction i(I_PUSH_FUNCTION);
	i.setValue_uint32(functionIndex);
	return i;
}

//! (static)
Instruction Instruction::createPushNumber(const double value){
	Instruction i(I_PUSH_NUMBER);
	i.setValue_Number(value);
	return i;
}

//! (static)
Instruction Instruction::createPushString(const uint32_t stringIndex){
	Instruction i(I_PUSH_STRING);
	i.setValue_uint32(stringIndex);
	return i;
}

//! (static)
Instruction Instruction::createPushUInt(const uint32_t value){
	Instruction i(I_PUSH_UINT);
	i.setValue_uint32(value);
	return i;
}

//! (static)
Instruction Instruction::createResetLocalVariable(const uint32_t localVarIdx){
	Instruction i(I_RESET_LOCAL_VARIABLE);
	i.setValue_uint32(localVarIdx);
	return i;
}
//! (static)
Instruction Instruction::createSetAttribute(const StringId & id){
	Instruction i(I_SET_ATTRIBUTE);
	i.setValue_Identifier(id);
	return i;
}
//! (static)
Instruction Instruction::createSetExceptionHandler(const uint32_t markerId){
	Instruction i(I_SET_EXCEPTION_HANDLER);
	i.setValue_uint32(markerId);
	return i;
}
//! (static)
Instruction Instruction::createSetMarker(const uint32_t markerId){
	Instruction i(I_SET_MARKER);
	i.setValue_uint32(markerId);
	return i;
}

//! (static)
Instruction Instruction::createSysCall(const uint32_t fnIdx, const uint32_t numParams){
	Instruction i(I_SYS_CALL);
	i.setValue_uint32Pair(fnIdx,numParams);
	return i;
}

std::string Instruction::toString(const InstructionBlock & ctxt)const{
	std::ostringstream out;
	switch(type){
	case I_ASSIGN_ATTRIBUTE:{
		out << "assignAttribute '" << getValue_Identifier().toString() << "'";
		break;
	}
	case I_ASSIGN_LOCAL:{
		out << "assignLocal $" << getValue_uint32() <<" // '" << ctxt.getLocalVarName(getValue_uint32()).toString()<<"'";
		break;
	}
	case I_ASSIGN_VARIABLE:{
		out << "assignVariable '" << getValue_Identifier().toString() << "'";
		break;
	}
	case I_CALL:{
		out << "call (numParams) " << getValue_uint32();
		break;
	}
	case I_CHECK_TYPE:{
		out << "checkType $" << getValue_uint32() <<" // '" << ctxt.getLocalVarName(getValue_uint32()).toString()<<"'";
		break;
	}
	case I_CREATE_INSTANCE:{
		out << "createInstance (numParams) " << getValue_uint32();
		break;
	}
	case I_DUP:{
		out << "dup";
		break;
	}
	case I_FIND_VARIABLE:{
		out << "findVariable '" << getValue_Identifier().toString() << "'";
		break;
	}
	case I_GET_ATTRIBUTE:{
		out << "getAttribute '" << getValue_Identifier().toString() << "'";
		break;
	}
	case I_GET_LOCAL_VARIABLE:{
		out << "getLocalVariable $" << getValue_uint32()<<" // '" << ctxt.getLocalVarName(getValue_uint32()).toString()<<"'";
		break;
	}
	case I_GET_VARIABLE:{
		out << "getVariable '" << getValue_Identifier().toString() << "'";
		break;
	}
	case I_INIT_CALLER:{
		out << "initCaller " << getValue_uint32();
		break;
	}
	case I_JMP:{
		out << "jmp ";
		if( getValue_uint32()<JMP_TO_MARKER_OFFSET)
			out << "-> "<<getValue_uint32();
		else
			out << "MARKER_" << getValue_uint32()-JMP_TO_MARKER_OFFSET<< ":";
		break;
	}
	case I_JMP_IF_SET:{
		out << "jmpIfSet ";
		if( getValue_uint32()<JMP_TO_MARKER_OFFSET)
			out << "-> "<<getValue_uint32();
		else
			out << "MARKER_" << getValue_uint32()-JMP_TO_MARKER_OFFSET<< ":";
		break;
	}
	case I_JMP_ON_TRUE:{
		out << "jmpOnTrue ";
		if( getValue_uint32()<JMP_TO_MARKER_OFFSET)
			out << "-> "<<getValue_uint32();
		else
			out << "MARKER_" << getValue_uint32()-JMP_TO_MARKER_OFFSET<< ":";
		break;
	}
	case I_JMP_ON_FALSE:{
		out << "jmpOnFalse ";
		if( getValue_uint32()<JMP_TO_MARKER_OFFSET)
			out << "-> "<<getValue_uint32();
		else
			out << "MARKER_" << getValue_uint32()-JMP_TO_MARKER_OFFSET<< ":";
		break;
	}
	case I_NOT:{
		out << "not";
		break;
	}
	case I_POP:{
		out << "pop";
		break;
	}
	case I_PUSH_BOOL:{
		out << "push (Bool) " << (getValue_Bool() ? "true" : "false");
		break;
	}
	case I_PUSH_ID:{
		out << "push (Indentifier) '" << getValue_Identifier().toString()<<"'";
		break;
	}
	case I_PUSH_FUNCTION:{
		out << "push (Function) #" << getValue_uint32();
		break;
	}
	case I_PUSH_NUMBER:{
		out << "push (Number) " << getValue_Number();
		break;
	}
	case I_PUSH_STRING:{
		out << "push (String) #"<<getValue_uint32()<<" // \"" << ctxt.getStringConstant(getValue_uint32())  << "\"";
		break;
	}
	case I_PUSH_UINT:{
		out << "push (uint) " << getValue_uint32();
		break;
	}
	case I_PUSH_UNDEFINED:{
		out << "push (Undefined)";
		break;
	}
	case I_PUSH_VOID:{
		out << "push (Void) Void";
		break;
	}
	case I_RESET_LOCAL_VARIABLE:{
		out << "reset $" << getValue_uint32() <<" // '" << ctxt.getLocalVarName(getValue_uint32()).toString()<<"'";
		break;
	}
	case I_SET_ATTRIBUTE:{
		out << "setAttribute '" << getValue_Identifier().toString() << "'";
		break;
	}
	case I_SET_EXCEPTION_HANDLER:{
		out << "setExceptionHandler ";
		if( getValue_uint32()<JMP_TO_MARKER_OFFSET)
			out << "-> "<<getValue_uint32();
		else
			out << "MARKER_" << getValue_uint32()-JMP_TO_MARKER_OFFSET<< ":";
		break;
	}
	case I_SET_MARKER:{
		if( getValue_uint32()<JMP_TO_MARKER_OFFSET)
			out << "!!!!!!!!!!!!!";
		else
			out << "MARKER_" << getValue_uint32()-JMP_TO_MARKER_OFFSET<< ":";

		break;
	}
	case I_SYS_CALL:{
		out << "sysCall (numParams) " << getValue_uint32();
		break;
	}
	case I_YIELD:{
		out << "yield";
		break;
	}

	default:
	case I_UNDEFINED:
			out << "????";
			break;

	}
	out<<" (line "<<line<<")";
	return out.str();
}
}
