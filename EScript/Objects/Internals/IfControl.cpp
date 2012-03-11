// IfControl.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "IfControl.h"

using namespace EScript;

//! (ctor)
IfControl::IfControl(Object * condition,const Statement & action,const Statement & elseAction):
		conditionRef(condition),actionRef(action),elseActionRef(elseAction) {
	//ctor
}

//! (dtor)
IfControl::~IfControl() {
	//dtor
}

//! ---|> Statement
void IfControl::_asmOut(std::ostream & out){
	static int markerNr = 0;
	
	out << "//<IfControl '"<<toString()<<"'\n";
	if(conditionRef.isNull()){
		if(elseActionRef.isValid()){
			elseActionRef._asmOut(out);
		}
	}else{
		const int elseMarker = ++markerNr;
		
		
		
		conditionRef->_asmOut(out);
		out << "jmpIfFalse marker"<<elseMarker<<":\n";
		if(actionRef.isValid()){
			actionRef._asmOut(out);
		}
		
		if(elseActionRef.isValid()){
			const int endMarker = ++markerNr;
			out << "jmp marker"<<endMarker<<"\n";
			out << "marker"<<elseMarker<<":\n";
			elseActionRef._asmOut(out);
			out << "marker"<<endMarker<<":\n";
		}else{
			out << "marker"<<elseMarker<<":\n";
		}
		
		
	
	}
	out << "//IfControl>\n";

}