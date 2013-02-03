// IOLib.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "IOLib.h"
#include "../EScript/EScript.h"
#include "../EScript/Utils/IO/IO.h"
#include "../EScript/Utils/StringUtils.h"

using namespace EScript;

static const int E_UNDEFINED_FILE=-1;
static const int E_FILE_DOES_NOT_EXIST = 0;
static const int E_FILE = 1;
static const int E_DIRECTORY = 2;

static const int E_DIR_FILES = 1;
static const int E_DIR_DIRECTORIES = 2;
static const int E_DIR_BOTH = 3;
static const int E_DIR_RECURSIVE = 4;

// ---------------------------------------------------

//! init
void IOLib::init(EScript::Namespace * o) {
	Namespace * lib = new Namespace;
	declareConstant(o,"IO",lib);

	//! [ESF] string fileGetContents(string filename,[format])
	ES_FUNCTION_DECLARE(lib,"fileGetContents",1,2, {

		StringData content;
		try{
			content = IO::loadFile(parameter[0].toString());
		}catch(const std::ios::failure & e){
			runtime.setException(e.what());
			return nullptr;
		}
		if(parameter.count()>1) {
			if(parameter[1].toString()=="UTF16LE") {
				const std::string contentS(StringUtils::UCS2LE_to_ANSII(content.str()));
				return contentS;
			} else {
				runtime.setException("Unknown format");
				return nullptr;
			}
		}
		return create(content);
	})

	//! [ESF] void filePutContents(string filename,string)
	ES_FUNCTION_DECLARE(lib,"filePutContents",2,2,{
		try{
			IO::saveFile(parameter[0].toString(),parameter[1].toString());
		}catch(const std::ios::failure & e){
			runtime.setException(e.what());
			return nullptr;
		}
		return Void::get();
	})

	//! [ESF] array dir(string dirname[,int flags])
	ES_FUNCTION_DECLARE(lib,"dir",1,2, {
		try {
			const auto files = IO::getFilesInDir(parameter[0].toString(), parameter[1].toInt(E_DIR_FILES));
			Array * ar = Array::create();
			for(const auto & file : files) 
				ar->pushBack(String::create(file));
			return ar;
		} catch (const std::string & s) {
			runtime.setException(s);
			return nullptr;
		}
	})

	//! [ESF] string condensePath(string path)
	ESF_DECLARE(lib,"condensePath",1,1,IO::condensePath(parameter[0].toString()))

	//! [ESF] bool isDir(string dirname)
	ESF_DECLARE(lib,"isDir",1,1,IO::getEntryType(parameter[0].toString())==IO::TYPE_DIRECTORY)

	//! [ESF] bool isFile(string filename)
	ESF_DECLARE(lib,"isFile",1,1,IO::getEntryType(parameter[0].toString())==IO::TYPE_FILE)

	//! [ESF] int fileMTime(string filename)
	ESF_DECLARE(lib,"fileMTime",1,1,static_cast<int>(IO::getFileMTime(parameter[0].toString())))

	//! [ESF] int fileSize(string filename)
	ESF_DECLARE(lib,"fileSize",1,1,static_cast<double>(IO::getFileSize(parameter[0].toString())))

	//! [ESF] string dirname(string path)
	ESF_DECLARE(lib,"dirname", 1, 1,IO::dirname(parameter[0].toString()))
	// rename
	// copy
	// delete
	declareConstant(lib,"DIR_FILES",		create(static_cast<uint32_t>(E_DIR_FILES)));
	declareConstant(lib,"DIR_DIRECTORIES",	create(static_cast<uint32_t>(E_DIR_DIRECTORIES)));
	declareConstant(lib,"DIR_BOTH",			create(static_cast<uint32_t>(E_DIR_BOTH)));
	declareConstant(lib,"DIR_RECURSIVE",	create(static_cast<uint32_t>(E_DIR_RECURSIVE)));

}
