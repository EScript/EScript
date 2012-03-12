#include "CompilerContext.h"
#include <stdexcept>


namespace EScript{

void CompilerContext::popLocalVars(const size_t count){
	if(count>localVariables.size())
		throw std::invalid_argument("popLocalVars");
	localVariables.resize(localVariables.size()-count);
}
void CompilerContext::pushLocalVars(const std::set<StringId> & variableNames){
	for(std::set<StringId>::const_iterator it = variableNames.begin();it!=variableNames.end();++it){
		localVariables.push_back(*it);
	}
}
int CompilerContext::getVarIndex(const StringId name)const{
	if(!localVariables.empty()){
		int index = localVariables.size()-1;
		for(std::vector<StringId>::const_reverse_iterator it = localVariables.rbegin();it!=localVariables.rend();++it){
			if(*it==name)
				return index;
			--index;
		}
	}
	return -1;
}
StringId CompilerContext::getVar(const int index)const{
	if(index <0 || index>=localVariables.size())
		return StringId();
	return localVariables.at(static_cast<size_t>(index));

}


}