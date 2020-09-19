// IO.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef UTILS_IO_H
#define UTILS_IO_H

#include "../StringData.h"
#include "IOBase.h"
#include <cstddef>
#include <cstdint>
#include <ios>
#include <string>
#include <vector>

namespace EScript {
namespace IO{
class AbstractFileSystemHandler;

/*! Set a new fileSystemHandler responsible for all io-operations.
	The old handler is deleted.	*/
ESCRIPTAPI void setFileSystemHandler(AbstractFileSystemHandler * handler);
ESCRIPTAPI AbstractFileSystemHandler * getFileSystemHandler();

ESCRIPTAPI StringData loadFile(const std::string & filename);
ESCRIPTAPI void saveFile(const std::string & filename,const std::string & content,bool overwrite=true);

/*! @param filename
 *	@return file modification Time	*/
ESCRIPTAPI uint32_t getFileMTime(const std::string& filename);

/*!	@param filename
 *	@return  IO::entryType (\see IOBase.h)	*/
ESCRIPTAPI entryType_t getEntryType(const std::string& filename);

/*!	@param filename
 *	@return filsize in byte.	*/
ESCRIPTAPI uint64_t getFileSize(const std::string& filename);

/*!
 * @param dirname
 * @param flags		1 ... Files
 *					2 ... Directories
 *					4 ... Recurse Subdirectories
 * @throw std::ios_base::failure on failure.	*/
ESCRIPTAPI std::vector<std::string> getFilesInDir(const std::string & dirname, uint8_t flags);

ESCRIPTAPI std::string dirname(const std::string & filename);

/*! Remove "." and ".." from the inputPath if possible.
 *	\example
 *		foo						-> foo
 *		/var/bla/				-> /var/bla/
 *		bla/./foo				-> bla/foo
 *		bla/foo/./../../bar/.	-> bar
 *		///						-> /
 *		//						-> /
 *		../../foo/bla/..		-> ../../foo		*/
ESCRIPTAPI std::string condensePath(const std::string & inputPath);

}
}

#endif // UTILS_IO_H
