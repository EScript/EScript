// Hashing.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_HASHING_H
#define ES_HASHING_H
#include <string>

namespace EScript {

typedef unsigned int hashvalue;
typedef hashvalue identifierId;

extern const std::string ES_UNKNOWN_IDENTIFIER;

identifierId stringToIdentifierId(const std::string & s);
const std::string & identifierIdToString(identifierId id);

//! (internal)
hashvalue _hash(const std::string & s);


}
#endif // ES_HASHING_H
