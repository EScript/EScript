// AttributeContainer.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "AttributeContainer.h"

#include "../EScript.h"

using namespace EScript;


//! (ctor)
AttributeContainer::AttributeContainer(const AttributeContainer & other){
	cloneAttributesFrom(other);
}

void AttributeContainer::initAttributes(Runtime & rt){
	for(attributeMap_t::iterator it = attributes.begin() ; it!=attributes.end() ; ++it){
		Attribute & attr = it->second;
		if(attr.isInitializable()){
			Type * type = dynamic_cast<Type*>(attr.getValue());
			if(type!=NULL){
				ObjRef value = rt.createInstance(type,ParameterValues());
				attr.setValue( value.get() );
			}else{
				ObjRef value = rt.executeFunction2(attr.getValue(),NULL,ParameterValues());
				attr.setValue( value.get() );
			}
		}
	}
}

void AttributeContainer::cloneAttributesFrom(const AttributeContainer & other) {
	for(attributeMap_t::const_iterator it = other.attributes.begin() ; it!=other.attributes.end() ; ++it){
		setAttribute(it->first, Attribute(it->second.getValue()->getRefOrCopy(),it->second.getProperties() ));
	}

}

void AttributeContainer::collectAttributes(std::map<StringId,Object *> & attrs){
	for(attributeMap_t::iterator it = attributes.begin() ; it!=attributes.end() ; ++it)
		attrs[it->first] = it->second.getValue();
}
