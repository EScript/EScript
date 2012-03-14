#include "InstructionBlock.h"
#include <sstream>

namespace EScript{



InstructionBlock::marker_t InstructionBlock::createMarker()	{
	const marker_t m = markerNames.size();
	std::ostringstream o; 
	o << "marker" << m; 
	markerNames.push_back(o.str());
	return NAMED_MARKER_OFFSET + m;
}		
InstructionBlock::marker_t InstructionBlock::createMarker(const std::string & prefix)	{	
	const marker_t m = markerNames.size();
	std::ostringstream o; 
	o << prefix << m; 
	markerNames.push_back(o.str());
	return NAMED_MARKER_OFFSET + m;
}
std::string InstructionBlock::getMarkerName(const marker_t m)const{
	std::ostringstream o;
	if(m<NAMED_MARKER_OFFSET){
		o<<m;
	}else{
		const uint32_t index = m - NAMED_MARKER_OFFSET;
		if(index<=markerNames.size()) 
			o<<markerNames[index];
	}
	o<<":";
	return o.str();
}

StringId InstructionBlock::getLocalVarName(const int index)const{
	if(index <0 || index>=static_cast<int>(localVariables.size()))
		return StringId();
	return localVariables.at(static_cast<size_t>(index));

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