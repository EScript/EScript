// Debug.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef UTILS_DEBUG_H
#define UTILS_DEBUG_H

namespace EScript {
class Object;
/**
 *
 */
class Debug {
	public:
		ESCRIPTAPI static void registerObj(Object * v);
		ESCRIPTAPI static void unRegisterObj(Object * v);
		ESCRIPTAPI static void showObjects();
		ESCRIPTAPI static void clearObjects();
};
}

#endif // UTILS_DEBUG_H
