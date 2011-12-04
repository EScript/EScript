// AbstractFileSystemHandler.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ABSTRACTFILESYSTEMHANDLER_H
#define ABSTRACTFILESYSTEMHANDLER_H

#include "../StringData.h"
#include "IOBase.h"
#include <ios>
#include <list>
#include <map>
#include <string>
#include <cstddef>

namespace EScript{
namespace IO{

/*! A FileSystemHandler servers as interface to the file system.
	Exchaninging this handler allows to globally add support for
	additional file systems (e.g. by implementing a http interface),
	or to add an access restriction to file operations (e.g. limit
	all operations to certain folders).
	\todo
		 - (?) add flush()
		 - add streaming support
	*/
class AbstractFileSystemHandler  {
protected:
	AbstractFileSystemHandler(){}
public:
	virtual ~AbstractFileSystemHandler(){}

	//! ---o
	virtual void deleteFile(const std::string &){
		throw std::ios_base::failure("unsupported operation");
	}
	/*!	---o
	 *	@param   dirname
	 *         flags:       1 ... Files
	 *                      2 ... Directories
	 *                      4 ... Recurse Subdirectories
	 * 	@throw std::ios_base::failure on failure.	*/
	virtual void dir(const std::string &/*path*/, std::list<std::string> &/*result*/, uint8_t/*flags*/){
		throw std::ios_base::failure("unsupported operation");
	}
	//! ---o
	virtual entryType_t getEntryType(const std::string & path){
		return getEntryInfo(path).type;
	}
	//! ---o
	virtual EntryInfo getEntryInfo(const std::string &){
		throw std::ios_base::failure("unsupported operation");
	}
	//! ---o
	virtual uint32_t getFileCTime(const std::string & path){
		return getEntryInfo(path).cTime;
	}
	//! ---o
	virtual uint32_t getFileMTime(const std::string & path){
		return getEntryInfo(path).mTime;
	}
	//! ---o
	virtual uint64_t getFileSize(const std::string & path){
		return getEntryInfo(path).fileSize;
	}

	//! ---o
	virtual void makeDir(const std::string &){
		throw std::ios_base::failure("unsupported operation");
	}
	//! ---o
	virtual StringData loadFile(const std::string &){
		throw std::ios_base::failure("unsupported operation");
	}
	//! ---o
	virtual void saveFile(const std::string &, const std::string & /*data*/, bool /*overwrite*/){
		throw std::ios_base::failure("unsupported operation");
	}
};
}
}

#endif // ABSTRACTFILESYSTEMHANDLER_H
