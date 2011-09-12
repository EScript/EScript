// IOLib.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "IOLib.h"
#include "../EScript/EScript.h"
#include "../EScript/Utils/IO/IO.h"
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

		StringData content;
		try{
			content = IO::loadFile(parameter[0].toString());
		}catch(std::ios::failure e){
			runtime.setExceptionState(new Exception(e.what()));
			return NULL;
		}
		if (parameter.count()>1) {
			if (parameter[1]->toString()=="UTF16LE") {
				std::string contentS(StringUtils::UCS2LE_to_ANSII(content.str()));
				return String::create(contentS);
			} else {
				runtime.setExceptionState(new Exception("Unknown format"));
				return NULL;
			}
		}
		return String::create(content);
	})

	//! [ESF] void filePutContents(string filename,string)
	ES_FUNCTION_DECLARE(lib,"filePutContents",2,2,{
		try{
			IO::saveFile(parameter[0].toString(),parameter[1].toString());
		}catch(std::ios::failure e){
			runtime.setExceptionState(new Exception(e.what()));
			return NULL;
		}
		return Void::get();
	})

	//! [ESF] array dir(string dirname[,int flags])
	ES_FUNCTION_DECLARE(lib,"dir",1,2, {
		std::list<std::string> files;
		try {
			IO::getFilesInDir(parameter[0]->toString(),files,parameter[1].toInt(E_DIR_FILES));
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
	ESF_DECLARE(lib,"condensePath",1,1,String::create(IO::condensePath(parameter[0].toString())))

	//! [ESF] bool isDir(string dirname)
	ESF_DECLARE(lib,"isDir",1,1,Bool::create(IO::getEntryType(parameter[0]->toString())==IO::TYPE_DIRECTORY))

	//! [ESF] bool isFile(string filename)
	ESF_DECLARE(lib,"isFile",1,1,Bool::create(IO::getEntryType(parameter[0]->toString())==IO::TYPE_FILE))

	//! [ESF] int fileMTime(string filename)
	ESF_DECLARE(lib,"fileMTime",1,1,Number::create( static_cast<int>(IO::getFileMTime(parameter[0]->toString()))))

	//! [ESF] int fileSize(string filename)
	ESF_DECLARE(lib,"fileSize",1,1,Number::create(IO::getFileSize(parameter[0]->toString())))

	//! [ESF] string dirname(string path)
	ESF_DECLARE(lib,"dirname", 1, 1,String::create(IO::dirname(parameter[0]->toString())))
	// rename
	// copy
	// delete
	declareConstant(lib,"DIR_FILES",Number::create(E_DIR_FILES));
	declareConstant(lib,"DIR_DIRECTORIES",Number::create(E_DIR_DIRECTORIES));
	declareConstant(lib,"DIR_BOTH",Number::create(E_DIR_BOTH));
	declareConstant(lib,"DIR_RECURSIVE",Number::create(E_DIR_RECURSIVE));

}
