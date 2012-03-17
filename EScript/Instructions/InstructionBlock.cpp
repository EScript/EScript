#include "InstructionBlock.h"
#include <sstream>

namespace EScript{


StringId InstructionBlock::getLocalVarName(const int index)const{
	if(index <0 || index>=static_cast<int>(localVariables.size()))
		return StringId();
	return localVariables.at(static_cast<size_t>(index));

}

void InstructionBlock::emplace(InstructionBlock & other){
	using namespace std;
	swap(other.localVariables,localVariables);
	swap(other.stringConstants,stringConstants);
	swap(other.instructions,instructions);
	swap(other.internalFunctions,internalFunctions);
}

std::string InstructionBlock::toString()const{
	std::ostringstream out;
	
	if(!localVariables.empty()){
		out << "Local variables:";
		uint32_t i=0;
		for(std::vector<StringId>::const_iterator it = localVariables.begin();it!=localVariables.end();++it){
			out << " $"<<i<<"('" << it->toString() << "')";
			++i;
		}
		out << "\n";
	}
	
	if(!stringConstants.empty()){
		out << "String constants:";
		uint32_t i=0;
		for(std::vector<std::string>::const_iterator it = stringConstants.begin();it!=stringConstants.end();++it){
			out << " #"<<i<<"(\"" << *it << "\")";
			++i;
		}
		out << "\n";
	}
	out << "---\n";
//		std::vector<std::string> stringConstants;
	{
		uint32_t i=0;
		for(std::vector<Instruction>::const_iterator it = instructions.begin();it!=instructions.end();++it){
			out << (i++) << "\t" << it->toString(*this) << "\n";
		}

	}
	return out.str();
}

}