// TypeNameMacro.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_PROVIDES_TYPE_NAME_MACRO_H_INCLUDED
#define ES_PROVIDES_TYPE_NAME_MACRO_H_INCLUDED

#define ES_PROVIDES_TYPE_NAME(_TYPE_NAME) \
public: \
	static const char * getClassName() { \
		return #_TYPE_NAME; \
	}\
	virtual const char * getTypeName() const { \
		return getClassName(); \
	} \
private:

#endif // ES_PROVIDES_TYPE_NAME_MACRO_H_INCLUDED
