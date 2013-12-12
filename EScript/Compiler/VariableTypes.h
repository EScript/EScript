// VariableTypes.h
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2013 Claudius Jähn <claudius@uni-paderborn.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef ES_VARIABLE_TYPES_H
#define ES_VARIABLE_TYPES_H

#include <unordered_map>

namespace EScript {

enum variableType_t {
	LOCAL_VAR,
	STATIC_VAR
};
typedef std::unordered_map<StringId,variableType_t>  declaredVariableMap_t;

typedef std::pair<variableType_t,int> varLocation_t;
inline bool isLocalVarLocation(const varLocation_t & l){
	return l.second>=0 && l.first == variableType_t::LOCAL_VAR;
}
inline bool isStaticVarLocation(const varLocation_t & l){
	return l.second>=0 && l.first == variableType_t::STATIC_VAR;
}
inline bool isValidVarLocation(const varLocation_t & l){
	return l.second>=0;
}


}

#endif // ES_VARIABLE_TYPES_H
