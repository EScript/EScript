// AbstractFileSystemHandler.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ABSTRACTFILESYSTEMHANDLER_H
#define ABSTRACTFILESYSTEMHANDLER_H

#include "../StringData.h"
#include "IOBase.h"
#include <ios>
#include <list>
#include <map>
#include <string>
#include <cstddef>

namespace EScript{
namespace IO{


class AbstractFileSystemHandler  {
public:

	// -------------------------

	AbstractFileSystemHandler(){}
	virtual ~AbstractFileSystemHandler(){}

	//! ---o
	virtual void deleteFile(const std::string &){
		throw std::ios_base::failure("unsupported operation");
	}
	//! ---o
	virtual void dir(const std::string &/*path*/, std::list<std::string> &/*result*/, uint8_t/*flags*/){
		throw std::ios_base::failure("unsupported operation");
	}
	//! ---o
	virtual entryType getEntryType(const std::string &){
		throw std::ios_base::failure("unsupported operation");
	}
	//! ---o
	virtual size_t fileSize(const std::string &){
		throw std::ios_base::failure("unsupported operation");
	}
	//! ---o
	virtual void makeDir(const std::string &){
		throw std::ios_base::failure("unsupported operation");
	}
	//! ---o
	virtual StringData loadFile(const std::string &){
		throw std::ios_base::failure("unsupported operation");
	}
	//! ---o
	virtual void saveFile(const std::string &, const std::string & /*data*/, bool /*overwrite*/){
		throw std::ios_base::failure("unsupported operation");
	}

//! \todo...
//    struct FileStreamWrapper {
//            FileStreamWrapper(){}
//            virtual ~FileStreamWrapper(){}
//            virtual void close()=0;
//    };
//    template<typename stream_t> class _SpecificStreamWrapper : public FileStreamWrapper{
//    	stream_t * s;
//    	_SpecificStreamWrapper(stream_t * _s) : FileStreamWrapper(), s(_s){}
//    	virtual ~_SpecificStreamWrapper(){}
//    };
//	typedef _SpecificStreamWrapper<std::iostream> IOFileStreamWrapper;
//	typedef _SpecificStreamWrapper<std::istream> IFileStreamWrapper;
//	typedef _SpecificStreamWrapper<std::ostream> OFileStreamWrapper;
//
//    virtual IOFileStreamWrapper * open(const std::string & )             {   return NULL;    }
//    virtual IFileStreamWrapper * openForReading(const std::string & )    {   return NULL;    }
//    virtual OFileStreamWrapper * openForWriting(const std::string & )    {   return NULL;    }
//    virtual OFileStreamWrapper * openForAppending(const std::string & )  {   return NULL;    }
//
//    static void closeStream(FileStreamWrapper * & stream){
//    	if(stream !=NULL) {
//    		stream->close();
//			delete stream;
//			stream = NULL;
//    	}
//    }
//
//	/*! If some kind of internal caching is used, all data should be written to disk
//		\note May block if needed	*/
//    virtual void flush()  {   return ;    }
};
}
}

#endif // ABSTRACTFILESYSTEMHANDLER_H
