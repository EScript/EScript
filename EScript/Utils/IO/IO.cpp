// IO.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "IO.h"
#include "DefaultFileSystemHandler.h"
#include "../StringUtils.h"

#include <memory>
namespace EScript{
namespace IO{
std::auto_ptr<AbstractFileSystemHandler> fileSystemHandler(new DefaultFileSystemHandler);
}

//! (static)
void IO::setFileSystemHandler(AbstractFileSystemHandler * handler){
	fileSystemHandler.reset(handler);
}

//! (static)
IO::AbstractFileSystemHandler * IO::getFileSystemHandler(){
	return fileSystemHandler.get();
}

//! (static)
StringData IO::loadFile(const std::string & filename) {
	return getFileSystemHandler()->loadFile(filename);
}

//! (static)
void IO::saveFile(const std::string & filename,const std::string & content,bool overwrite){
	getFileSystemHandler()->saveFile(filename,content,overwrite);
}

//! (static)
uint32_t IO::getFileMTime(const std::string& filename) {
	return getFileSystemHandler()->getFileMTime(filename);
}

IO::entryType_t IO::getEntryType(const std::string& path) {
	return getFileSystemHandler()->getEntryType(path);
}

uint64_t IO::getFileSize(const std::string& filename) {
	return getFileSystemHandler()->getFileSize(filename);
}

void IO::getFilesInDir(const std::string & dirname,std::list<std::string> & files,uint8_t flags) {
	getFileSystemHandler()->dir(dirname,files,flags);
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
