// Hashing.h
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
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

extern const std::string ES_UNKNOWN_IDENTIFIER;

identifierId stringToIdentifierId(const std::string & s);
const std::string & identifierIdToString(identifierId id);

//! (internal)
hashvalue _hash(const std::string & s);


}
#endif // ES_HASHING_H
