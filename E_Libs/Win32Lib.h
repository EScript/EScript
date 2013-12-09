// Win32Lib.h
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef WIN32LIB_H
#define WIN32LIB_H
#ifdef _WIN32
#include <string>

namespace EScript{
class Namespace;

namespace Win32Lib {

//LIB_EXPORT
void init(EScript::Namespace * o);

void setClipboard(const std::string & s);
std::string getClipboard();
}
}
#endif // _WIN32

#endif // WIN32LIB_H
