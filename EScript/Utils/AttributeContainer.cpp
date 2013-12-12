// AttributeContainer.cpp
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2012-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#include "AttributeContainer.h"

#include "../Basics.h"

namespace EScript{

//! (ctor)
AttributeContainer::AttributeContainer(const AttributeContainer & other){
	cloneAttributesFrom(other);
}

void AttributeContainer::initAttributes(Runtime & rt){
	for(auto & keyValuePair : attributes) {
		Attribute & attr = keyValuePair.second;
		if(attr.isInitializable()){
			Type * type = dynamic_cast<Type*>(attr.getValue());
			if(type!=nullptr){
				ObjRef value = rt.createInstance(type,ParameterValues());
				attr.setValue( value.get() );
			}else{
				ObjRef value = rt.executeFunction(attr.getValue(),nullptr,ParameterValues());
				attr.setValue( value.get() );
			}
		}
	}
}

void AttributeContainer::cloneAttributesFrom(const AttributeContainer & other) {
	for(const auto & keyValuePair : other.attributes) {
		setAttribute(keyValuePair.first, Attribute(keyValuePair.second.getValue()->getRefOrCopy(), keyValuePair.second.getProperties()));
	}

}

void AttributeContainer::collectAttributes(std::unordered_map<StringId,Object *> & attrs){
	for(const auto & keyValuePair : attributes) {
		attrs[keyValuePair.first] = keyValuePair.second.getValue();
	}
}
}
