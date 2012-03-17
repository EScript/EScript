#include "CompilerContext.h"
#include <sstream>

namespace EScript{

void CompilerContext::initBasicLocalVars(){

	visibleLocalVariableStack.push_back(indexNameMapping_t());
	const std::vector<StringId> & names = instructions.getLocalVariables();
	for(size_t i=0;i<names.size();++i){
		visibleLocalVariableStack.back()[ names[i] ] = i;
	}

}

void CompilerContext::pushLocalVars(const std::set<StringId> & variableNames){
	visibleLocalVariableStack.push_back(indexNameMapping_t());
	for(std::set<StringId>::const_iterator it = variableNames.begin();it!=variableNames.end();++it){
		visibleLocalVariableStack.back()[ *it ] = instructions.declareLocalVariable(*it);
	}
}
int CompilerContext::getVarIndex(const StringId name)const{

	for(std::vector<indexNameMapping_t>::const_reverse_iterator it=visibleLocalVariableStack.rbegin();
			it!=visibleLocalVariableStack.rend();++it){
		const indexNameMapping_t::const_iterator fIt = it->find(name);
		if(fIt!=it->end()){
			return fIt->second;
		}
	}
	return -1;
}


//		void pushLocalVars(const std::set<StringId> & variableNames);
void CompilerContext::popLocalVars(){
	visibleLocalVariableStack.pop_back();
}

	
//! (static) \todo // move to Compiler
void CompilerContext::finalizeInstructions( InstructionBlock & instructionBlock ){
	std::vector<Instruction> & instructions = instructionBlock._accessInstructions();
	
	if(instructionBlock.hasJumpMarkers()){
		std::map<uint32_t,uint32_t> markerToPosition;
	
		{ // pass 1: remove setMarker-instructions and store position
			std::vector<Instruction> tmp;
			for(std::vector<Instruction>::const_iterator it=instructions.begin();it!=instructions.end();++it){
				if( it->getType() == Instruction::I_SET_MARKER ){
					markerToPosition[it->getValue_uint32()] = tmp.size();
				}else{
					tmp.push_back(*it);
				}
			}
			tmp.swap(instructions);
//			instructionBlock.clearMarkerNames();
		}

		{ // pass 2: adapt jump instructions
			for(std::vector<Instruction>::iterator it=instructions.begin();it!=instructions.end();++it){
				if( it->getType() == Instruction::I_JMP 
						|| it->getType() == Instruction::I_JMP_ON_TRUE 
						|| it->getType() == Instruction::I_JMP_ON_FALSE){
					const uint32_t markerId = it->getValue_uint32();
					
					// is name of a marker (and not already a jump position)
					if(markerId>=Instruction::JMP_TO_MARKER_OFFSET){
						it->setValue_uint32(markerToPosition[markerId]);
					}
				}
			}
			
		}
		
	}
		

//	originalInstructions.swap(finalInstructions);
	
}
}