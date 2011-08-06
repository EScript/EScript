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
	virtual StringData loadFile(const std::string &)throw (std::ios_base::failure);

	//! ---|> AbstractFileSystemHandler
	virtual void saveFile(const std::string &, const std::string & /*data*/, bool /*overwrite*/)throw (std::ios_base::failure);

};
}
}

#endif // DEFAULTFILESYSTEMHANDLER_H
