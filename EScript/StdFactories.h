// StdFactories.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_STD_FACTORIES_H_INCLUDED
#define ES_STD_FACTORIES_H_INCLUDED

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>

namespace EScript {

class Object;
class Array;
class Number;
class String;
class StringData;
class Bool;
class Void;

Array* create(const std::vector<Object*>&);
Number* create(const double);
Number* create(const float);
Number* create(const int);
Number* create(const uint32_t);
String* create(const StringData &);
String* create(const std::string &);
String* create(const char*);
Bool* create(const bool);
Void* create(std::nullptr_t);

}

#endif // ES_STD_FACTORIES_H_INCLUDED
