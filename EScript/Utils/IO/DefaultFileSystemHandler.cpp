// DefaultFileSystemHandler.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "DefaultFileSystemHandler.h"
#include <fstream>
#include <string>
#include <vector>
#include <sys/stat.h>

#if defined(_MSC_VER)
#include "ext/dirent.h"
#else
#include <dirent.h>
#endif


namespace EScript{

namespace IO{

std::vector<std::string> DefaultFileSystemHandler::dir(const std::string & dirname, uint8_t flags) {

	DIR *directoryHandle = opendir(dirname.c_str());
	if(!directoryHandle)
		throw std::ios_base::failure( std::string("Could not open dir: '"+dirname+'\''));

	std::vector<std::string> files;
	for( dirent * entry = readdir(directoryHandle) ; entry!=nullptr ; entry = readdir(directoryHandle)){
		if(entry->d_name[0] == '.')
			continue;

		const std::string entryName( dirname+'/'+entry->d_name );
		const entryType_t type = getEntryType(entryName);

		if( (type==IO::TYPE_DIRECTORY && (flags & 2)) ||
				( type==IO::TYPE_FILE && (flags & 1) )) {
			files.push_back(entryName);
		}

		if(type==IO::TYPE_DIRECTORY && (flags & 4)) { // recursive
			// std::cout << "Recusrive dir:"<<entryName<<"\n";
			const auto filesRecursive = dir(entryName, flags);
			files.insert(files.end(), filesRecursive.begin(), filesRecursive.end());
		}
	}
	closedir(directoryHandle);
	return files;
}


//! ---|> AbstractFileSystemHandler
StringData DefaultFileSystemHandler::loadFile(const std::string & filename){

	std::ifstream inputFile( filename.c_str(), std::ios::in | std::ios::binary);
	if( inputFile.fail())
		throw std::ios_base::failure(std::string("Could not open file for reading: '"+filename+'\''));

	inputFile.seekg( 0, std::ios::end );
	const size_t size = static_cast<size_t>(inputFile.tellg());
	inputFile.seekg( 0, std::ios::beg );
	if(size==0){
		inputFile.close();
		return StringData();
	}

	char *pBuffer = new char[size+1];
	inputFile.read( pBuffer, size );
	inputFile.close();
	StringData result(pBuffer,size);
	delete [] pBuffer;
	return result;
}

//! ---|> AbstractFileSystemHandler
void DefaultFileSystemHandler::saveFile(const std::string & filename, const std::string & content, bool overwrite){
	if(!overwrite && getEntryType(filename)==TYPE_FILE)
		throw std::ios_base::failure(std::string("File already exists: '"+filename+'\''));
	std::ofstream outputFile( filename.c_str(), std::ios::out | std::ios::binary);
	if( outputFile.fail())
		throw std::ios_base::failure(std::string("Could not open file for writing: '"+filename+'\''));

	outputFile.write( content.data(), content.length() );
	outputFile.close();
}

//! ---|> AbstractFileSystemHandler
EntryInfo DefaultFileSystemHandler::getEntryInfo(const std::string &filename){
	EntryInfo info;
	struct stat fileStat;
	if( stat(filename.c_str(), &fileStat)==0 ){
		info.cTime = fileStat.st_ctime;
		info.mTime = fileStat.st_mtime;
		if(S_ISDIR(fileStat.st_mode)){
			info.type = TYPE_DIRECTORY;
		}else if(S_ISREG(fileStat.st_mode)){
			info.type = TYPE_FILE;
			info.fileSize = static_cast<uint64_t>(fileStat.st_size);
		}else{
			info.type = TYPE_UNKNOWN;
		}
	}else{
		info.type = TYPE_NOT_FOUND;
	}
	return info;
}
}
}
