// Instruction.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Instruction.h"
#include <iostream>
#include <sstream>

namespace EScript {

//! (static)
Instruction Instruction::createAssign(const StringId varName){
	Instruction i(I_ASSIGN);
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
Instruction Instruction::createCall(const uint32_t numParams){
	Instruction i(I_CALL);
	i.setValue_uint32(numParams);
	return i;
}

//! (static)
Instruction Instruction::createFindVariable(const StringId id){
	Instruction i(I_FIND_VARIABLE);
	i.setValue_Identifier(id);
	return i;
}

//! (static)
Instruction Instruction::createGetAttribute(const StringId id){
	Instruction i(I_GET_ATTRIBUTE);
	i.setValue_Identifier(id);
	return i;
}

//! (static)
Instruction Instruction::createGetVariable(const StringId id){
	Instruction i(I_GET_VARIABLE);
	i.setValue_Identifier(id);
	return i;
}

//! (static)
Instruction Instruction::createJmp(const StringId id){
	Instruction i(I_JMP);
	i.setValue_Identifier(id);
	return i;
}

//! (static)
Instruction Instruction::createJmpOnTrue(const StringId id){
	Instruction i(I_JMP_ON_TRUE);
	i.setValue_Identifier(id);
	return i;
}

//! (static)
Instruction Instruction::createJmpOnFalse(const StringId id){
	Instruction i(I_JMP_ON_FALSE);
	i.setValue_Identifier(id);
	return i;
}


//! (static)
Instruction Instruction::createPop(){
	return Instruction(I_POP);
}

//! (static)
Instruction Instruction::createNot(){
	return Instruction(I_NOT);
}

//! (static)
Instruction Instruction::createPushBool(const bool value){
	Instruction i(I_PUSH_BOOL);
	i.setValue_Bool(value);
	return i;
}

//! (static)
Instruction Instruction::createPushId(const StringId id){
	Instruction i(I_PUSH_ID);
	i.setValue_Identifier(id);
	return i;
}

//! (static)
Instruction Instruction::createPushNumber(const double value){
	Instruction i(I_PUSH_NUMBER);
	i.setValue_Number(value);
	return i;
}

//! (static)
Instruction Instruction::createPushString(const std::string & value){
	Instruction i(I_PUSH_STRING);
	i.setValue_String(value);
	return i;
}

//! (static)
Instruction Instruction::createPushUInt(const uint32_t value){
	Instruction i(I_PUSH_UINT);
	i.setValue_uint32(value);
	return i;
}

//! (static)
Instruction Instruction::createPushVoid(){
	Instruction i(I_PUSH_VOID);
	return i;
}

//! (static)
Instruction Instruction::createSetAttribute(const StringId id){
	Instruction i(I_SET_ATTRIBUTE);
	i.setValue_Identifier(id);
	return i;
}
//! (static)
Instruction Instruction::createSetMarker(const StringId id){
	Instruction i(I_SET_MARKER);
	i.setValue_Identifier(id);
	return i;
}

std::string Instruction::toString()const{
	std::ostringstream out;
	switch(type){
	case I_ASSIGN_LOCAL:{
		out << "assignLocal $" << getValue_uint32() << "\n";
		break;
	}	
	case I_ASSIGN:{
		out << "assign '" << getValue_Identifier().toString() << "'\n";
		break;
	}
	case I_CALL:{
		out << "call " << getValue_uint32() << "\n";
		break;
	}	
	case I_DUP:{
		out << "dup\n";
		break;
	}
	case I_FIND_VARIABLE:{
		out << "findVariable '" << getValue_Identifier().toString() << "'\n";
		break;
	}
	case I_GET_ATTRIBUTE:{
		out << "getAttribute '" << getValue_Identifier().toString() << "'\n";
		break;
	}
	case I_GET_VARIABLE:{
		out << "getVariable '" << getValue_Identifier().toString() << "'\n";
		break;
	}
	case I_JMP:{
		out << "jmp " << getValue_Identifier().toString() << ":\n";
		break;
	}
	case I_JMP_ON_TRUE:{
		out << "jmpOnTrue " << getValue_Identifier().toString() << ":\n";
		break;
	}
	case I_JMP_ON_FALSE:{
		out << "jmpOnFalse " << getValue_Identifier().toString() << ":\n";
		break;
	}
	case I_NOT:{
		out << "not\n";
		break;
	}
	case I_POP:{
		out << "pop\n";
		break;
	}
	case I_PUSH_BOOL:{
		out << "push (Bool) " << getValue_Bool() << "\n";
		break;
	}
	case I_PUSH_ID:{
		out << "call " << getValue_uint32() << "\n";
		break;
	}
	case I_PUSH_NUMBER:{
		out << "push (Number) " << getValue_Number() << "\n";
		break;
	}
	case I_PUSH_STRING:{
		out << "push (String) \"" << getValue_String() << "\"\n";
		break;
	}	
	case I_PUSH_UINT:{
		out << "push (uint) \"" << getValue_uint32() << "\"\n";
		break;
	}
	case I_PUSH_VOID:{
		out << "push (Void) Void\n";
		break;
	}
	case I_SET_ATTRIBUTE:{
		out << "setAttribute '" << getValue_Identifier().toString() << "'\n";
		break;
	}	
	case I_SET_MARKER:{
		out << getValue_Identifier().toString() << ":\n";
		break;
	}
	default:
	case I_UNDEFINED:
			out << "????\n";
			break;
	
	}

	return out.str();
}
}
