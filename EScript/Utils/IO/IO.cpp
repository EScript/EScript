// IO.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "IO.h"
#include "DefaultFileSystemhandler.h"
#include "../StringUtils.h"

#if defined(_MSC_VER)
#include "ext/dirent.h"
#else
#include <dirent.h>
#endif

#include <sys/stat.h>
#include <fstream>
#include <deque>
#include <vector>
#include <memory>
namespace EScript{
namespace IO{
std::auto_ptr<AbstractFileSystemHandler> fileSystemHandler(new DefaultFileSystemHandler);
}

void IO::setFileSystemHandler(AbstractFileSystemHandler * handler){
	fileSystemHandler.reset(handler);
}

IO::AbstractFileSystemHandler * IO::getFileSystemHandler(){
	return fileSystemHandler.get();
}

StringData IO::loadFile(const std::string & filename) {
	return getFileSystemHandler()->loadFile(filename);
}

void IO::saveFile(const std::string & filename,const std::string & content,bool overwrite){
	getFileSystemHandler()->saveFile(filename,content,overwrite);
}

uint32_t IO::getFileMTime(const std::string& filename) {
	struct stat fileStat;
	return stat(filename.c_str(), &fileStat)!=0 ? 0 : static_cast<unsigned int>(fileStat.st_mtime);
}

IO::entryType IO::getEntryType(const std::string& filename) {
	struct stat fileStat;

	if ( stat(filename.c_str(), &fileStat)!=0 )
		return TYPE_NOT_FOUND;

	return (S_ISDIR(fileStat.st_mode)) ? TYPE_DIRECTORY : (S_ISREG(fileStat.st_mode)) ? TYPE_FILE : TYPE_UNKNOWN;
}

uint64_t IO::getFileSize(const std::string& filename) {
	struct stat fileStat;
	return stat(filename.c_str(), &fileStat)!=0 ? 0 :  static_cast<unsigned long>(fileStat.st_size);
}

void IO::getFilesInDir(const std::string & dirname,std::list<std::string> & files,int flags) {

	DIR *dir = opendir (dirname.c_str ());
	if (!dir)
		throw std::ios_base::failure( std::string("Could not open dir \""+dirname+"\"!"));

	for( dirent * entry=readdir(dir) ; entry!=NULL ; entry=readdir(dir)){

		if (entry->d_name[0] == '.')
			continue;

		const std::string entryName( dirname+"/"+entry->d_name );

		const entryType type = getEntryType(entryName);

		if ( (type==IO::TYPE_DIRECTORY && (flags & 2)) || ( type==IO::TYPE_FILE && (flags & 1) )) {
			files.push_back(entryName);
		}

		if (type==IO::TYPE_DIRECTORY && (flags & 4)) { // recursive
			// std::cout << "Recusrive dir:"<<entryName<<"\n";
			getFilesInDir(entryName,files,flags);
		}
	}
	closedir(dir);
	return;
}

std::string IO::dirname(const std::string & filename) {
	const size_t slash=filename.find_last_of("/\\");
	return slash==std::string::npos ? "." : filename.substr(0,slash);
}

std::string IO::condensePath(const std::string & inputPath){
	// split
	std::vector<std::string> parts;
	StringUtils::split(inputPath,"/",parts);
	// condense
	std::deque<std::string> parts2;
	for(std::vector<std::string>::const_iterator it=parts.begin();it!=parts.end();++it){
		const std::string & part=*it;
		if(part == ".." && (!parts2.empty() && parts2.back()!=".." ) ){ // ".."? -> remove parent folder if not at the beginning
			parts2.pop_back();
		}else if( !part.empty() && part != ".") {
			parts2.push_back(part);
		}
	}
	// rebuild
	std::string output;
	for(std::deque<std::string>::const_iterator it=parts2.begin();it!=parts2.end();++it){
		if(it!=parts2.begin())
			output+="/";
		output += *it;
	}
	// finalize
	if(!parts.empty()){
		if(parts.front().empty() && (output.empty() || output.at(0)!='/' ) )// add '/' add the beginning
			output = "/"+output;
		if(parts.back().empty() && (output.empty() || output.at(output.length()-1) !='/') ) // add '/' add the end
			output += "/";
	}
	return output;
}
}
