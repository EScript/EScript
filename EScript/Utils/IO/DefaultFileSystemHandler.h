// DefaultFileSystemHandler.h
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef DEFAULTFILESYSTEMHANDLER_H
#define DEFAULTFILESYSTEMHANDLER_H

#include "AbstractFileSystemHandler.h"

namespace EScript{
namespace IO{

//! DefaultFileSystemHandler ---|> AbstractFileSystemHandler
class DefaultFileSystemHandler : public AbstractFileSystemHandler  {
public:

	DefaultFileSystemHandler(){}
	virtual ~DefaultFileSystemHandler(){}

	//! ---|> AbstractFileSystemHandler
	virtual std::vector<std::string> dir(const std::string &, uint8_t);

	//! ---|> AbstractFileSystemHandler
	virtual EntryInfo getEntryInfo(const std::string &);

	//! ---|> AbstractFileSystemHandler
	virtual StringData loadFile(const std::string &);

	//! ---|> AbstractFileSystemHandler
	virtual void saveFile(const std::string &, const std::string & /*data*/, bool /*overwrite*/);
};
}
}

#endif // DEFAULTFILESYSTEMHANDLER_H
