// Debug.h
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
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
		static void registerObj(Object * v);
		static void unRegisterObj(Object * v);
		static void showObjects();
		static void clearObjects();
};
}

#endif // UTILS_DEBUG_H
