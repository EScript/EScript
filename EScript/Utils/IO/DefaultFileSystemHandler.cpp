// DefaultFileSystemHandler.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "DefaultFileSystemHandler.h"
#include <fstream>

using namespace EScript;
using namespace EScript::IO;

//! ---|> AbstractFileSystemHandler
StringData DefaultFileSystemHandler::loadFile(const std::string & filename){

	std::ifstream inputFile( filename.c_str(), std::ios::in | std::ios::binary);
	if ( inputFile.fail())
		throw std::ios_base::failure(std::string("Could not open file for reading ('"+filename+"')."));

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
	std::ofstream outputFile( filename.c_str(), std::ios::out | std::ios::binary);
	if ( outputFile.fail())
		throw std::ios_base::failure(std::string("Could not open file for writing '("+filename+"')."));

	outputFile.write( content.data(), content.length() );
	outputFile.close();
}
