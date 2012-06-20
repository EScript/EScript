// DefaultFileSystemHandler.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
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
