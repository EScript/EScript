// JSON.h
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef ESCRIPT_JSON_H
#define ESCRIPT_JSON_H
#include <string>
#include <sstream>

namespace EScript {
class Object;

/**
 *	JSON support for EScript
 *	[static-helper]
 */
struct JSON	{
	static void toJSON(std::ostringstream & out,Object * obj,bool formatted = true,int level = 0);
	static std::string toJSON(Object * obj,bool formatted = true);
	static Object* parseJSON(const std::string &s);
};

}
#endif // ESCRIPT_JSON_H
