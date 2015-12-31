// StdConversions.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2013 Claudius Jähn <ClaudiusJ@live.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef ES_STD_CONVERSIONS_H
#define ES_STD_CONVERSIONS_H

#include "ConversionBasics.h"
#include "ObjRef.h"

namespace EScript {

// bool
template<>
inline bool convertTo<bool>(Runtime &, ObjPtr src) {
	return src.toBool();
}

// number
template<>
double convertTo<double>(Runtime & rt, ObjPtr src);
template<> 
inline float convertTo<float>(Runtime & rt, ObjPtr src) {
	return static_cast<float>(convertTo<double>(rt, src));
}
template<>
inline long long convertTo<long long>(Runtime & rt, ObjPtr src) {
	return static_cast<long long>(convertTo<double>(rt, src));
}
template<>
inline unsigned long long convertTo<unsigned long long>(Runtime & rt, ObjPtr src) {
	return static_cast<unsigned long long>(convertTo<double>(rt, src));
}
template<>
inline long convertTo<long>(Runtime & rt, ObjPtr src) {
	return static_cast<long>(convertTo<double>(rt, src));
}
template<>
inline unsigned long convertTo<unsigned long>(Runtime & rt, ObjPtr src) {
	return static_cast<unsigned long>(convertTo<double>(rt, src));
}
template<>
inline int convertTo<int>(Runtime & rt, ObjPtr src) {
	return static_cast<int>(convertTo<double>(rt, src));
}
template<>
inline unsigned int convertTo<unsigned int>(Runtime & rt, ObjPtr src) {
	return static_cast<unsigned int>(convertTo<double>(rt, src));
}
template<>
inline short convertTo<short>(Runtime & rt, ObjPtr src) {
	return static_cast<short>(convertTo<double>(rt, src));
}
template<>
inline unsigned short convertTo<unsigned short>(Runtime & rt, ObjPtr src) {
	return static_cast<unsigned short>(convertTo<double>(rt, src));
}

// string
template<>
inline std::string convertTo<std::string>(Runtime &,ObjPtr src) {
	return src.toString();
}

}
#endif // ES_STD_CONVERSIONS_H
