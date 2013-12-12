// TypeNameMacro.h
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
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
