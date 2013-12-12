// FnCompileContext.cpp
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2012-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#include "FnCompileContext.h"
#include "Compiler.h"
#include "../Objects/Callables/UserFunction.h" // StaticData
#include <sstream>

namespace EScript{

void FnCompileContext::addExpression(EPtr<AST::ASTNode> expression){
	compiler.addExpression(*this,expression);
}
void FnCompileContext::addStatement(EPtr<AST::ASTNode> stmt){
	compiler.addStatement(*this,stmt);
}

StringId FnCompileContext::createOnceStatementMarker(){
	std::ostringstream s;
	s << "___once_"<<currentOnceMarkerCounter++;
	return s.str();
}

uint32_t FnCompileContext::getCurrentMarker(setting_t type)const{
	for(std::vector<SettingsStackEntry>::const_reverse_iterator it = settingsStack.rbegin();it!=settingsStack.rend();++it){
		const SettingsStackEntry & entry = *it;
		if(entry.type == type)
			return entry.marker;
	}
	return Instruction::INVALID_JUMP_ADDRESS;
}

void FnCompileContext::pushSetting_basicLocalVars(){
	settingsStack.push_back(SettingsStackEntry(VISIBLE_LOCAL_AND_STATIC_VARIABLES));
	SettingsStackEntry & entry = settingsStack.back();

	const std::vector<StringId> & names = instructions.getLocalVariables();
	for(size_t i = 0;i<names.size();++i){
		entry.declaredVariables[ names[i] ] = std::make_pair(variableType_t::LOCAL_VAR,i);
		if(!variableCollectorStack.empty()){
			variableCollectorStack.top()->push_back(i);
		}
	}
}

void FnCompileContext::pushSetting_declaredVars(const declaredVariableMap_t & variables){
	settingsStack.push_back(SettingsStackEntry(VISIBLE_LOCAL_AND_STATIC_VARIABLES));
	SettingsStackEntry & entry = settingsStack.back();

	for(const auto & var_idAndType : variables) {
		if(var_idAndType.second == variableType_t::LOCAL_VAR){
			const size_t varIndex = instructions.declareLocalVariable(var_idAndType.first);
			entry.declaredVariables[var_idAndType.first] = std::make_pair(variableType_t::LOCAL_VAR,varIndex);
			if(!variableCollectorStack.empty()){
				variableCollectorStack.top()->push_back(varIndex);
			}
		}else if(var_idAndType.second == variableType_t::STATIC_VAR){
			const size_t varIndex = staticData.declareStaticVariable(var_idAndType.first);
			entry.declaredVariables[var_idAndType.first] = std::make_pair(variableType_t::STATIC_VAR,varIndex);
		}
	}
}

varLocation_t FnCompileContext::getCurrentVarLocation(const StringId & name)const{
	for(auto it = settingsStack.rbegin();it!=settingsStack.rend();++it){
		if( (*it).type == VISIBLE_LOCAL_AND_STATIC_VARIABLES){
			const auto fIt = (*it).declaredVariables.find(name);
			if(fIt!=(*it).declaredVariables.end() && isValidVarLocation( fIt->second )){
				return fIt->second;
			}
		}
	}
	// search for static variables
	for(FnCompileContext *ctxt=parent;ctxt;ctxt=ctxt->parent){
		for(auto it = ctxt->settingsStack.rbegin();it!=ctxt->settingsStack.rend();++it){
			if( (*it).type == VISIBLE_LOCAL_AND_STATIC_VARIABLES){
				const auto fIt = (*it).declaredVariables.find(name);
				if(fIt!=(*it).declaredVariables.end() && isStaticVarLocation( fIt->second )){
					return fIt->second;
				}
			}
		}
	}
	return std::make_pair(variableType_t::LOCAL_VAR,-1); // invalid var
}

std::vector<size_t> FnCompileContext::collectLocalVariables(setting_t entryType){
	std::vector<size_t> variableIndices;
	for(std::vector<SettingsStackEntry>::const_reverse_iterator it = settingsStack.rbegin();it!=settingsStack.rend();++it){
		const SettingsStackEntry & entry = *it;
		if(entry.type == VISIBLE_LOCAL_AND_STATIC_VARIABLES){
			for(const auto & var_idAndLocation : entry.declaredVariables) { // id -> location
				if(isLocalVarLocation(var_idAndLocation.second))
					variableIndices.push_back(var_idAndLocation.second.second);
			}
		}
		if(entry.type == entryType)
			break;
	}
	return variableIndices;
}

}
