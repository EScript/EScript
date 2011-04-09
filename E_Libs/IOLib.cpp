// IOLib.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "IOLib.h"
#include "../EScript/EScript.h"
#include "../EScript/Utils/FileUtils.h"
#include "../EScript/Utils/StringUtils.h"
#include <list>

using namespace EScript;

static const int E_UNDEFINED_FILE=-1;
static const int E_FILE_DOES_NOT_EXIST=0;
static const int E_FILE=1;
static const int E_DIRECTORY=2;

static const int E_DIR_FILES=1;
static const int E_DIR_DIRECTORIES=2;
static const int E_DIR_BOTH=3;
static const int E_DIR_RECURSIVE=4;

// ---------------------------------------------------

//! init
void IOLib::init(EScript::Namespace * o) {
	Namespace * lib=new Namespace();
	declareConstant(o,"IO",lib);

	//! [ESF] string fileGetContents(string filename,[format])
	ES_FUNCTION_DECLARE(lib,"fileGetContents",1,2, {
		std::string fs=parameter[0]->toString();
		size_t size;
		char * content=FileUtils::loadFile(parameter[0]->toString().c_str(),size);
		if (content==0){
			runtime.setExceptionState(new Exception("Could not open File!"));
			return NULL;
		}
		if (parameter.count()>1) {
			if (parameter[1]->toString()=="UTF16LE") {
				std::string contentS(StringUtils::UCS2LE_to_ANSII(size,reinterpret_cast<unsigned char *>(content)));
				delete [] content;
				return String::create(contentS);
			} else {
				runtime.setExceptionState(new Exception("Unknown format"));
				return NULL;
			}
		}
		std::string contentS(content);
		delete [] content;
		return String::create(contentS);
	})

	//! [ESF] bool filePutContents(string filename,string)
	ES_FUNCTION_DECLARE(lib,"filePutContents",2,2,{
		std::string contentS(parameter[1]->toString());
		return Bool::create(
				FileUtils::saveFile(parameter[0]->toString().c_str(),contentS.c_str(),contentS.length()));
	})

	//! [ESF] array dir(string dirname[,int flags])
	ES_FUNCTION_DECLARE(lib,"dir",1,2, {
		std::list<std::string> files;
		try {
			FileUtils::getFilesInDir(parameter[0]->toString(),files,parameter[1].toInt(E_DIR_FILES));
		} catch (std::string s) {
			runtime.setExceptionState( new Exception(s));
		}

		Array * ar=Array::create();
		for (std::list<std::string> ::iterator  it=files.begin();it!=files.end();++it) {
			ar->pushBack(String::create(*it));
		}
		return ar;
	})

	//! [ESF] string condensePath(string path)
	ESF_DECLARE(lib,"condensePath",1,1,String::create(FileUtils::condensePath(parameter[0].toString())))

	//! [ESF] bool isDir(string dirname)
	ESF_DECLARE(lib,"isDir",1,1,Bool::create(FileUtils::isFile(parameter[0]->toString())==2))

	//! [ESF] bool isFile(string filename)
	ESF_DECLARE(lib,"isFile",1,1,Bool::create(FileUtils::isFile(parameter[0]->toString())==1))

	//! [ESF] int fileMTime(string filename)
	ESF_DECLARE(lib,"fileMTime",1,1,Number::create( static_cast<int>(FileUtils::getFileMTime(parameter[0]->toString()))))

	//! [ESF] int fileSize(string filename)
	ESF_DECLARE(lib,"fileSize",1,1,Number::create(FileUtils::getFileSize(parameter[0]->toString())))

	//! [ESF] string dirname(string path)
	ESF_DECLARE(lib,"dirname", 1, 1,String::create(FileUtils::dirname(parameter[0]->toString())))
	// rename
	// copy
	// delete
	declareConstant(lib,"DIR_FILES",Number::create(E_DIR_FILES));
	declareConstant(lib,"DIR_DIRECTORIES",Number::create(E_DIR_DIRECTORIES));
	declareConstant(lib,"DIR_BOTH",Number::create(E_DIR_BOTH));
	declareConstant(lib,"DIR_RECURSIVE",Number::create(E_DIR_RECURSIVE));

}
