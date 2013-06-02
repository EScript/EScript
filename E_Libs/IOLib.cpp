// IOLib.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "IOLib.h"
#include "../EScript/Basics.h"
#include "../EScript/StdObjects.h"
#include "../EScript/Utils/IO/IO.h"
#include "../EScript/Utils/StringUtils.h"

namespace EScript{

static const int E_UNDEFINED_FILE=-1;
static const int E_FILE_DOES_NOT_EXIST = 0;
static const int E_FILE = 1;
static const int E_DIRECTORY = 2;

static const uint32_t E_DIR_FILES = 1;
static const uint32_t E_DIR_DIRECTORIES = 2;
static const uint32_t E_DIR_BOTH = 3;
static const uint32_t E_DIR_RECURSIVE = 4;

// ---------------------------------------------------

//! init
void IOLib::init(EScript::Namespace * o) {
	Namespace * lib = new Namespace;
	declareConstant(o,"IO",lib);

	//! [ESF] string fileGetContents(string filename,[format])
	ES_FUNCTION2(lib,"fileGetContents",1,2, {

		StringData content;
		try{
			content = IO::loadFile(parameter[0].toString());
		}catch(const std::ios::failure & e){
			rt.setException(e.what());
			return nullptr;
		}
		if(parameter.count()>1) {
			if(parameter[1].toString()=="UTF16LE") {
				const std::string contentS(StringUtils::UCS2LE_to_ANSII(content.str()));
				return contentS;
			} else {
				rt.setException("Unknown format");
				return nullptr;
			}
		}
		return create(content);
	})

	//! [ESF] void filePutContents(string filename,string)
	ES_FUNCTION2(lib,"filePutContents",2,2,{
		try{
			IO::saveFile(parameter[0].toString(),parameter[1].toString());
		}catch(const std::ios::failure & e){
			rt.setException(e.what());
			return nullptr;
		}
		return nullptr;
	})

	//! [ESF] array dir(string dirname[,int flags])
	ES_FUNCTION2(lib,"dir",1,2, {
		try {
			return Array::create( IO::getFilesInDir(parameter[0].toString(), parameter[1].toInt(E_DIR_FILES)));
		} catch (const std::string & s) {
			rt.setException(s);
			return nullptr;
		}
	})

	//! [ESF] string condensePath(string path)
	ES_FUN(lib,"condensePath",1,1,IO::condensePath(parameter[0].toString()))

	//! [ESF] bool isDir(string dirname)
	ES_FUN(lib,"isDir",1,1,IO::getEntryType(parameter[0].toString())==IO::TYPE_DIRECTORY)

	//! [ESF] bool isFile(string filename)
	ES_FUN(lib,"isFile",1,1,IO::getEntryType(parameter[0].toString())==IO::TYPE_FILE)

	//! [ESF] int fileMTime(string filename)
	ES_FUN(lib,"fileMTime",1,1,static_cast<int>(IO::getFileMTime(parameter[0].toString())))

	//! [ESF] int fileSize(string filename)
	ES_FUN(lib,"fileSize",1,1,static_cast<double>(IO::getFileSize(parameter[0].toString())))

	//! [ESF] string dirname(string path)
	ES_FUN(lib,"dirname", 1, 1,IO::dirname(parameter[0].toString()))
	// rename
	// copy
	// delete
	declareConstant(lib,"DIR_FILES",		static_cast<uint32_t>(E_DIR_FILES));
	declareConstant(lib,"DIR_DIRECTORIES",	static_cast<uint32_t>(E_DIR_DIRECTORIES));
	declareConstant(lib,"DIR_BOTH",			static_cast<uint32_t>(E_DIR_BOTH));
	declareConstant(lib,"DIR_RECURSIVE",	static_cast<uint32_t>(E_DIR_RECURSIVE));

}
}
