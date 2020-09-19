// Hashing.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef ES_HASHING_H
#define ES_HASHING_H
#include <string>

namespace EScript {

typedef unsigned int hashvalue;
typedef hashvalue identifierId;

ESCRIPTAPI extern const std::string ES_UNKNOWN_IDENTIFIER;

ESCRIPTAPI identifierId stringToIdentifierId(const std::string & s);
ESCRIPTAPI const std::string & identifierIdToString(identifierId id);

//! (internal)
hashvalue _hash(const std::string & s);


}
#endif // ES_HASHING_H
