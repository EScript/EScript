// IO.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef UTILS_H
#define UTILS_H

#include "../StringData.h"
#include <cstddef>
#include <list>
#include <string>
#include <ios>

namespace EScript {
namespace IO{
class AbstractFileSystemHandler;

/*! Set a new fileSystemHandler responsible for all io-operations.
	The old handler is deleted.	*/
void setFileSystemHandler(AbstractFileSystemHandler * handler);
AbstractFileSystemHandler * getFileSystemHandler();

StringData loadFile(const std::string & filename) throw (std::ios_base::failure);
void saveFile(const std::string & filename,const std::string & content,bool overwrite=true) throw (std::ios_base::failure);

/*! @param filename
 *	@return file modification Time	*/
unsigned int getFileMTime(const std::string& filename);

/*!	@param filename
 *	@return  	-1   undefined
 *				0   not_found
 *				1   file
 *				2   directory	*/
int isFile(const std::string& filename);

/*!	@param filename
 *	@return filsize in byte.	*/
unsigned long getFileSize(const std::string& filename);

/*!	@param   dirname
 *         flags:       1 ... Files
 *                      2 ... Directories
 *                      4 ... Recurse Subdirectories
 * 	@throw string	*/
void getFilesInDir(const std::string & dirname, std::list<std::string> & files,int flags) ;

std::string dirname(const std::string & filename);

/*! Remove "." and ".." from the inputPath if possible.
 *	\example
 *		foo 					-> foo
 *		/var/bla/ 				-> /var/bla/
 *		bla/./foo 				-> bla/foo
 *		bla/foo/./../../bar/. 	-> bar
 *		/// 					-> /
 *		// 						-> /
 *		../../foo/bla/.. 		-> ../../foo		*/
std::string condensePath(const std::string & inputPath);

}
}

#endif // UTILS_H
