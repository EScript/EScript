// StdLib.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef STDLIB_H
#define STDLIB_H
#include <string>

namespace EScript{
class Namespace;
class Object;
class Runtime;

namespace StdLib {

void init(EScript::Namespace * o);

/*! Loads and executes the script from file __filename__.
 * Returns the return value; void otherwise. */
Object * load(Runtime & runtime,const std::string & filename);

/*! Loads and executes the script from file __filename__ if the file has not already been loaded by this function.
 * Returns the return value; void otherwise. */
Object * loadOnce(Runtime & runtime,const std::string & filename);

/*! formatted output */
void print_r(Object * o,int maxLevel=7,int level=1);

}
}
#endif // STDLIB_H
