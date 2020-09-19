// JSON.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef ESCRIPT_JSON_H
#define ESCRIPT_JSON_H
#include <string>
#include <iosfwd>

namespace EScript {
class Object;

/**
 *	JSON support for EScript
 *	[static-helper]
 */
struct JSON	{
	ESCRIPTAPI static void toJSON(std::ostringstream & out,Object * obj,bool formatted = true,int level = 0);
	ESCRIPTAPI static std::string toJSON(Object * obj,bool formatted = true);
	ESCRIPTAPI static Object* parseJSON(const std::string &s);
};

}
#endif // ESCRIPT_JSON_H
