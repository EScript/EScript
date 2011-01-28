// Debug.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef UTILS_DEBUG_H
#define UTILS_DEBUG_H

#include "../Object.h"

namespace EScript {
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
