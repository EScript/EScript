// AttributeContainer.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "AttributeContainer.h"

#include "../EScript.h"

using namespace EScript;

void AttributeContainer::initAttributes(Runtime & rt){
	for(attributeMap_t::iterator it = attributes.begin() ; it!=attributes.end() ; ++it){
		Attribute & attr = it->second;
		if(attr.isInitializable()){
			Type * t = dynamic_cast<Type*>(attr.getValue());
			if(t!=NULL){
				attr.setValue( rt.executeFunction(t->getAttribute(Consts::IDENTIFIER_fn_constructor).getValue(), NULL, ParameterValues()));
			}else{
				attr.setValue( rt.executeFunction(attr.getValue(),NULL,ParameterValues()));
			}
		}
	}
}

void AttributeContainer::cloneAttributesFrom(const AttributeContainer & other) {
	for(attributeMap_t::const_iterator it = other.attributes.begin() ; it!=other.attributes.end() ; ++it){
		setAttribute(it->first, Attribute(it->second.getValue()->getRefOrCopy(),it->second.getFlags() ));
	}
		
}

void AttributeContainer::getAttributes(std::map<StringId,Object *> & attrs){
	for(attributeMap_t::iterator it = attributes.begin() ; it!=attributes.end() ; ++it)
		attrs[it->first] = it->second.getValue();
}
