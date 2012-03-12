// Instruction.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Instruction.h"

namespace EScript {

//! (static)
Instruction Instruction::createCall(const Statement & _stmt,const size_t numParams){
	Instruction i(I_CALL,_stmt);
	i.setValue_NumParams(numParams);
	return i;
}

//! (static)
Instruction Instruction::createFindVariable(const Statement & _stmt,const StringId id){
	Instruction i(I_FIND_VARIABLE,_stmt);
	i.setValue_Identifier(id);
	return i;
}

//! (static)
Instruction Instruction::createGetAttribute(const Statement & _stmt,const StringId id){
	Instruction i(I_GET_ATTRIBUTE,_stmt);
	i.setValue_Identifier(id);
	return i;
}

//! (static)
Instruction Instruction::createGetVariable(const Statement & _stmt,const StringId id){
	Instruction i(I_GET_VARIABLE,_stmt);
	i.setValue_Identifier(id);
	return i;
}

//! (static)
Instruction Instruction::createJmp(const Statement & _stmt,const StringId id){
	Instruction i(I_JMP,_stmt);
	i.setValue_MarkerId(id);
	return i;
}

//! (static)
Instruction Instruction::createJmpOnTrue(const Statement & _stmt,const StringId id){
	Instruction i(I_JMP_ON_TRUE,_stmt);
	i.setValue_MarkerId(id);
	return i;
}

//! (static)
Instruction Instruction::createJmpOnFalse(const Statement & _stmt,const StringId id){
	Instruction i(I_JMP_ON_FALSE,_stmt);
	i.setValue_MarkerId(id);
	return i;
}


//! (static)
Instruction Instruction::createPop(const Statement & _stmt){
	return Instruction(I_POP,_stmt);
}

//! (static)
Instruction Instruction::createNot(const Statement & _stmt){
	return Instruction(I_NOT,_stmt);
}

//! (static)
Instruction Instruction::createPushBool(const Statement & _stmt,const bool value){
	Instruction i(I_PUSH_BOOL,_stmt);
	i.setValue_Bool(value);
	return i;
}

//! (static)
Instruction Instruction::createPushId(const Statement & _stmt,const StringId id){
	Instruction i(I_PUSH_ID,_stmt);
	i.setValue_Identifier(id);
	return i;
}

//! (static)
Instruction Instruction::createPushNumber(const Statement & _stmt,const double value){
	Instruction i(I_PUSH_NUMBER,_stmt);
	i.setValue_Number(value);
	return i;
}

//! (static)
Instruction Instruction::createPushString(const Statement & _stmt,const std::string & value){
	Instruction i(I_PUSH_STRING,_stmt);
	i.setValue_String(value);
	return i;
}

//! (static)
Instruction Instruction::createPushVoid(const Statement & _stmt){
	Instruction i(I_PUSH_VOID,_stmt);
	return i;
}

//! (static)
Instruction Instruction::createSetMarker(const Statement & _stmt,const StringId id){
	Instruction i(I_SET_MARKER,_stmt);
	i.setValue_MarkerId(id);
	return i;
}

std::string Instruction::toString()const{
	std::ostringstream out;
	switch(type){
	case I_CALL:{
		out << "call " << getValue_NumParams() << "\n";
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
		out << "jmp " << getValue_MarkerId().toString() << ":\n";
		break;
	}
	case I_JMP_ON_TRUE:{
		out << "jmpOnTrue " << getValue_MarkerId().toString() << ":\n";
		break;
	}
	case I_JMP_ON_FALSE:{
		out << "jmpOnFalse " << getValue_MarkerId().toString() << ":\n";
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
		out << "call " << getValue_NumParams() << "\n";
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
	case I_PUSH_VOID:{
		out << "push (Void) Void\n";
		break;
	}
	case I_SET_MARKER:{
		out << getValue_MarkerId().toString() << ":\n";
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
