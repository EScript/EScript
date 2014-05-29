// DefaultFileSystemHandler.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
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
	std::vector<std::string> dir(const std::string &, uint8_t) override;

	//! ---|> AbstractFileSystemHandler
	EntryInfo getEntryInfo(const std::string &) override;

	//! ---|> AbstractFileSystemHandler
	StringData loadFile(const std::string &) override;

	//! ---|> AbstractFileSystemHandler
	void saveFile(const std::string &, const std::string & /*data*/, bool /*overwrite*/) override;
};
}
}

#endif // DEFAULTFILESYSTEMHANDLER_H
