// FileUtils.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "FileUtils.h"
#include "StringUtils.h"

#if defined(_MSC_VER)
#include "ext/dirent.h"
#else
#include <dirent.h>
#endif

#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <ostream>
#include <deque>

using namespace EScript;

using std::ifstream;
using std::ofstream;
using std::ios;

char * FileUtils::loadFile(const std::string & filename,size_t & size) {
	//std::cout<< "\nopening "<<filename;

	ifstream inputFile( filename.c_str(), ios::in | ios::binary);
	if ( inputFile.fail())
		return 0; // "Couldn't open the model file."

	inputFile.seekg( 0, ios::end );
	size = static_cast<size_t>(inputFile.tellg());
	//std::cout<< "\nloading "<<size<<"byte";
	inputFile.seekg( 0, ios::beg );

	char *pBuffer = new char[size+1];
	inputFile.read( pBuffer, size );
	inputFile.close();
	pBuffer[size]=0;
	//std::cout<< "\n: "<<pBuffer<<"\n---\n";

	return pBuffer;
}

bool FileUtils::saveFile(const std::string & filename,const char * content,const size_t size) {
	ofstream outputFile( filename.c_str(), ios::out | ios::binary);
	if ( outputFile.fail())
		return false;

	outputFile.write( content, size );
	outputFile.close();
	return true;
}

unsigned int FileUtils::getFileMTime(const std::string& filename) {
	struct stat fileStat;
	return stat(filename.c_str(), &fileStat)!=0 ? 0 : static_cast<unsigned int>(fileStat.st_mtime);
}

int FileUtils::isFile(const std::string& filename) {
	struct stat fileStat;

	if ( stat(filename.c_str(), &fileStat)!=0 )
		return -1;

	return (S_ISDIR(fileStat.st_mode)) ? 2: (S_ISREG(fileStat.st_mode)) ? 1 : -1;
}

unsigned long FileUtils::getFileSize(const std::string& filename) {
	struct stat fileStat;

	return stat(filename.c_str(), &fileStat)!=0 ? 0 :  static_cast<unsigned long>(fileStat.st_size);
}

void FileUtils::getFilesInDir(const std::string & dirname,std::list<std::string> & files,int flags) {

	DIR *dir = opendir (dirname.c_str ());
	if (!dir)
		throw ( std::string("Could not open dir \""+dirname+"\"!"));

	for( dirent * entry=readdir(dir) ; entry!=NULL ; entry=readdir(dir)){

		if (entry->d_name[0] == '.')
			continue;

		std::string entryName( dirname+"/"+entry->d_name );

		int type=isFile(entryName);

		if ( (type==2 && (flags & 2)) || ( type==1 && (flags & 1) )) {
			files.push_back(entryName);
		}

		if (type==2 && (flags & 4)) { // recursive
			// std::cout << "Recusrive dir:"<<entryName<<"\n";
			getFilesInDir(entryName,files,flags);
		}
	}
	closedir(dir);
	return;
}

std::string  FileUtils::dirname(const std::string & filename) {
	size_t slash=filename.find_last_of("/\\");
	return slash==std::string::npos ? "." : filename.substr(0,slash);
}

std::string FileUtils::condensePath(const std::string & inputPath){
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
