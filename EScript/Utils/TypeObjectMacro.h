// TypeObjectMacro.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_PROVIDES_TYPE_OBJECT_MACRO_H_INCLUDED
#define ES_PROVIDES_TYPE_OBJECT_MACRO_H_INCLUDED

#define ES_PROVIDES_TYPE_OBJECT(_BASE_TYPE) \
public: \
	static Type * getTypeObject(){ \
		static Type * typeObject = new Type(_BASE_TYPE::getTypeObject()); \
		return typeObject; \
	} \
private:

#endif // ES_PROVIDES_TYPE_OBJECT_MACRO_H_INCLUDED
