// StdLib.h
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef STDLIB_H
#define STDLIB_H
#include "../EScript/Utils/ObjRef.h"
#include <string>

namespace EScript{
class Namespace;
class Object;
class Runtime;

namespace StdLib {

void init(EScript::Namespace * o);



/*! Loads and executes the script from file __filename__ if the file has not already been loaded by this function.
	Returns the return value;
	\note May throw 'Object *' on error!
 */
ObjRef loadOnce(Runtime & runtime,const std::string & filename);

//! formatted output
void print_r(Object * o,int maxLevel = 7,int level = 1);

// returns "WINDOWS" | "MAX OS" | "LINUX" | "UNIX" | "UNKNOWN"
std::string getOS();

}
}
#endif // STDLIB_H
