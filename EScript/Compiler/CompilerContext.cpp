// CompilerContext.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "CompilerContext.h"
#include "Compiler.h"
#include <sstream>

namespace EScript{

void CompilerContext::addExpression(EPtr<AST::ASTNode> expression){
	compiler.addExpression(*this,expression);
}
void CompilerContext::addStatement(EPtr<AST::ASTNode> stmt){
	compiler.addStatement(*this,stmt);
}

StringId CompilerContext::createOnceStatementMarker(){
	std::ostringstream s;
	s << "___once_"<<currentOnceMarkerCounter++;
	return s.str();
}

uint32_t CompilerContext::getCurrentMarker(setting_t type)const{
	for(std::vector<SettingsStackEntry>::const_reverse_iterator it = settingsStack.rbegin();it!=settingsStack.rend();++it){
		const SettingsStackEntry & entry = *it;
		if(entry.type == type)
			return entry.marker;
	}
	return Instruction::INVALID_JUMP_ADDRESS;
}

void CompilerContext::pushSetting_basicLocalVars(){
	settingsStack.push_back(SettingsStackEntry(VISIBLE_LOCAL_VARIABLES));
	SettingsStackEntry & entry = settingsStack.back();

	const std::vector<StringId> & names = instructions.getLocalVariables();
	for(size_t i = 0;i<names.size();++i){
		entry.localVariables[ names[i] ] = i;
		if(!variableCollectorStack.empty()){
			variableCollectorStack.top()->push_back(i);
		}
	}
}

void CompilerContext::pushSetting_localVars(const std::set<StringId> & variableNames){
	settingsStack.push_back(SettingsStackEntry(VISIBLE_LOCAL_VARIABLES));
	SettingsStackEntry & entry = settingsStack.back();

	for(const auto & varName : variableNames) {
		const size_t varIndex = instructions.declareLocalVariable(varName);
		entry.localVariables[varName] = varIndex;
		if(!variableCollectorStack.empty()){
			variableCollectorStack.top()->push_back(varIndex);
		}
	}
}

int CompilerContext::getCurrentVarIndex(const StringId & name)const{
	for(std::vector<SettingsStackEntry>::const_reverse_iterator it = settingsStack.rbegin();it!=settingsStack.rend();++it){
		const SettingsStackEntry & entry = *it;
		if(entry.type != VISIBLE_LOCAL_VARIABLES)
			continue;
		const nameToIndexMapping_t::const_iterator fIt = entry.localVariables.find(name);
		if(fIt!=entry.localVariables.end()){
			return fIt->second;
		}
	}
	return -1;
}

bool CompilerContext::collectLocalVariables(setting_t entryType,std::vector<size_t> & variableIndices){
	variableIndices.clear();
	for(std::vector<SettingsStackEntry>::const_reverse_iterator it = settingsStack.rbegin();it!=settingsStack.rend();++it){
		const SettingsStackEntry & entry = *it;
		if(entry.type == VISIBLE_LOCAL_VARIABLES){
			for(const auto & nameIndexPair : entry.localVariables) {
				variableIndices.push_back(nameIndexPair.second);
			}
		}
		if(entry.type == entryType)
			return true;
	}
	return false;

}

}
