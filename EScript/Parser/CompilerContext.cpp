#include "CompilerContext.h"


namespace EScript{

void CompilerContext::pushLocalVars(const std::set<StringId> & variableNames){
	currentLocalVariableStack.push_back(indexNameMapping_t());
	for(std::set<StringId>::const_iterator it = variableNames.begin();it!=variableNames.end();++it){
		const size_t varIndex = localVariables.size();
		localVariables.push_back( *it );
		currentLocalVariableStack.back()[ *it ] = varIndex;
	}
}
int CompilerContext::getVarIndex(const StringId name)const{

	for(std::vector<indexNameMapping_t>::const_reverse_iterator it=currentLocalVariableStack.rbegin();
			it!=currentLocalVariableStack.rend();++it){
		const indexNameMapping_t::const_iterator fIt = it->find(name);
		if(fIt!=it->end()){
			return fIt->second;
		}
	}
	return -1;
}
StringId CompilerContext::getVar(const int index)const{
	if(index <0 || index>=localVariables.size())
		return StringId();
	return localVariables.at(static_cast<size_t>(index));

}

//		void pushLocalVars(const std::set<StringId> & variableNames);
void CompilerContext::popLocalVars(){
	currentLocalVariableStack.pop_back();
}

}