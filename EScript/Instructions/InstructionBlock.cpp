// InstructionBlock.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "InstructionBlock.h"
#include "../Objects/Callables/UserFunction.h"
#include "../Consts.h"
#include <sstream>

namespace EScript{

InstructionBlock::InstructionBlock() {
	// Magic local variables
	declareLocalVariable(Consts::IDENTIFIER_this); // $0 : Consts::LOCAL_VAR_INDEX_this
	declareLocalVariable(Consts::IDENTIFIER_thisFn); // $1 : Consts::LOCAL_VAR_INDEX_thisFn
	declareLocalVariable(Consts::IDENTIFIER_internalResult); // $2 : Consts::LOCAL_VAR_INDEX_internalResult
}

StringId InstructionBlock::getLocalVarName(const size_t index)const{
	if(index >= localVariables.size()) {
		return StringId();
	}
	return localVariables.at(index);
}

UserFunction * InstructionBlock::getUserFunction(const uint32_t index)const{
	if(index<=internalFunctions.size()){
		return dynamic_cast<UserFunction*>(internalFunctions.at(index).get());
	}else{
		return nullptr;
	}
}

std::string InstructionBlock::toString()const{
	std::ostringstream out;

	if(!localVariables.empty()){
		out << "Local variables:";
		uint32_t i = 0;
		for(const auto & localVar : localVariables) {
			out << " $"<<i<<"('" << localVar.toString() << "')";
			++i;
		}
		out << "\n";
	}

	if(!stringConstants.empty()){
		out << "String constants:";
		uint32_t i = 0;
		for(const auto & stringConst : stringConstants) {
			out << " #"<<i<<"(\"" << stringConst << "\")";
			++i;
		}
		out << "\n";
	}
	out << "---\n";
//		std::vector<std::string> stringConstants;
	{
		uint32_t i = 0;
		for(const auto & instruction : instructions) {
			out << (i++) << "\t" << instruction.toString(*this) << "\n";
		}

	}
	if(!internalFunctions.empty()){
		out << "Functions:\n-------\n";
		for(uint32_t i = 0;i<internalFunctions.size();++i){
			UserFunction * uFun = getUserFunction(i);
			if(uFun){
				out << "<Function #"<<i<<"\n";
				out << uFun->getInstructionBlock().toString();
				out << "Function #"<<i<<">\n";
			}
		}
		out << "-------\n\n";
	}
	return out.str();
}

}
