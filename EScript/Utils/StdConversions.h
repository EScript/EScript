// StdConversions.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_STD_CONVERSIONS_H
#define ES_STD_CONVERSIONS_H

#include "ConversionBasics.h"
#include "ObjRef.h"

namespace EScript {

// bool
template<>inline bool convertTo<bool>(Runtime &,ObjPtr eObj)		{	return eObj.toBool();	}

// number
template<> double convertTo<double>(Runtime& rt,ObjPtr src);
template<> inline float convertTo<float>(Runtime& rt,ObjPtr src)		{	return static_cast<float>(convertTo<double>(rt,src));	}
template<> inline int64_t convertTo<int64_t>(Runtime& rt,ObjPtr src)	{	return static_cast<int64_t>(convertTo<double>(rt,src));	}
template<> inline uint64_t convertTo<uint64_t>(Runtime& rt,ObjPtr src)	{	return static_cast<uint64_t>(convertTo<double>(rt,src));	}
template<> inline int32_t convertTo<int32_t>(Runtime& rt,ObjPtr src)	{	return static_cast<int32_t>(convertTo<double>(rt,src));	}
template<> inline uint32_t convertTo<uint32_t>(Runtime& rt,ObjPtr src)	{	return static_cast<uint32_t>(convertTo<double>(rt,src));	}
template<> inline int16_t convertTo<int16_t>(Runtime& rt,ObjPtr src)	{	return static_cast<int16_t>(convertTo<double>(rt,src));	}
template<> inline uint16_t convertTo<uint16_t>(Runtime& rt,ObjPtr src)	{	return static_cast<uint16_t>(convertTo<double>(rt,src));	}

// string
template<> inline std::string convertTo<std::string>(Runtime &,ObjPtr src){	return src.toString();	}
}
#endif // ES_STD_CONVERSIONS_H
