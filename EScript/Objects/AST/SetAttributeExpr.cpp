// SetAttributeExpr.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "SetAttributeExpr.h"
#include "../../Parser/CompilerContext.h"

using namespace EScript;
using namespace EScript::AST;

//! static
SetAttributeExpr * SetAttributeExpr::createAssignment(Object * obj,StringId attrId,Object * valueExp,int _line){
	SetAttributeExpr * sa  = new SetAttributeExpr(obj,attrId,valueExp,0,_line) ;
	sa->assign = true;
	return sa;
}

//! (ctor)
SetAttributeExpr::SetAttributeExpr(Object * obj,StringId _attrId,Object * _valueExp,Attribute::flag_t _attrFlags,int _line):
		objExpr(obj),valueExpr(_valueExp),attrId(_attrId),attrFlags(_attrFlags),line(_line),assign(false) {
	//ctor
}

//! (dtor)
SetAttributeExpr::~SetAttributeExpr() {
	//dtor
}

//! ---|> [Object]
std::string SetAttributeExpr::toString()const {
	std::string s;
	if (!objExpr.isNull()) {
		s+=objExpr.toString();
	} else s+='_';
	s+='.'+getAttrName();
	if(assign){
		s+='=';
	}else{
		if(attrFlags & (Attribute::CONST_BIT | Attribute::PRIVATE_BIT | Attribute::INIT_BIT)){
			s+="@(";
			if( attrFlags & Attribute::CONST_BIT)		s+="const,";
			if( attrFlags & Attribute::PRIVATE_BIT)		s+="private,";
			if( attrFlags & Attribute::INIT_BIT)		s+="init,";
			s+=')';
		}
		if( attrFlags & Attribute::TYPE_ATTR_BIT)
			s+="::=";
		else
			s+=":=";
	}
	s+='('+valueExpr.toString()+") ";
	return s;
}

//! ---|> Object
void SetAttributeExpr::_asm(CompilerContext & ctxt){
	valueExpr->_asm(ctxt);
//	ctxt.out << "dup\n";
	
	ctxt.setLine(line);
	ctxt.addInstruction(Instruction::createDup());
	
	
	if(assign){
		// no object given: a = ...
		if(objExpr.isNull()){
			// local variable: var a = ...	
			if(ctxt.getVarIndex(attrId)>=0){
				ctxt.addInstruction(Instruction::createAssignLocal(ctxt.getVarIndex(attrId)));
			}else{
				ctxt.addInstruction(Instruction::createAssignVariable(attrId));
			
			}
		}else{ // object.a = 
			objExpr->_asm(ctxt);
			ctxt.addInstruction(Instruction::createAssignAttribute(attrId));
		}
		
	}else{
			objExpr->_asm(ctxt);
			ctxt.addInstruction(Instruction::createPushUInt(static_cast<uint32_t>(getAttributeProperties())));
//			ctxt.out << "push (uint) " << static_cast<int>(getAttributeProperties())<<"\n";
			ctxt.addInstruction(Instruction::createSetAttribute(attrId));
//			ctxt.out << "setAttribute '" << attrId.toString() << "'\n";
	}
	
}