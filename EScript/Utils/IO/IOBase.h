// IOBase.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef IOBASE_H_INCLUDED
#define IOBASE_H_INCLUDED

namespace EScript {
namespace IO {

enum entryType{
	TYPE_NOT_FOUND = 0,
	TYPE_FILE = 1,
	TYPE_DIRECTORY = 2,
	TYPE_UNKNOWN = 2
};


}
}


#endif // IOBASE_H_INCLUDED
