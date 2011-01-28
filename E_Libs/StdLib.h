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

}
}
#endif // STDLIB_H
