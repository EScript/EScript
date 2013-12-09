// IOBase.h
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef IOBASE_H_INCLUDED
#define IOBASE_H_INCLUDED
#include <cstddef>
#include <cstdint>

namespace EScript {
namespace IO {

enum entryType_t{
	TYPE_NOT_FOUND = -1,
	TYPE_FILE = 0,
	TYPE_DIRECTORY = 1,
	TYPE_UNKNOWN = 2
};
struct EntryInfo{
	uint64_t fileSize;
	entryType_t type;
	uint32_t mTime;
	uint32_t cTime;
	EntryInfo():fileSize(0),type(TYPE_UNKNOWN),mTime(0),cTime(0){}
};


}
}


#endif // IOBASE_H_INCLUDED
