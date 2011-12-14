// Debug.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
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
