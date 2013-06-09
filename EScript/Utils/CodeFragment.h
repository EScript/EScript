// CodeFragment.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_CODE_FRAGMENT_H
#define ES_CODE_FRAGMENT_H

#include "StringData.h"
#include "StringId.h"

namespace EScript {

//! Simple container used to store the code of a UserFunction
class CodeFragment{
		StringId filename;
		StringData data;
		size_t start,length;

	public:

		CodeFragment() : start(0),length(0){}
		CodeFragment(const StringId & _filename,const StringData & fullCode) :
				filename(_filename),data(fullCode), start(0),length(fullCode.getDataSize()){}
		CodeFragment(const StringId & _filename, const StringData & _data,const size_t _start, const size_t _length) :
				filename(_filename),data(_data), start(_start),length(_length){}
		CodeFragment(const CodeFragment & other,const size_t _start, const size_t _length) :
				filename(other.filename),data(other.data), start(_start),length(_length){}

		bool empty()const						{	return length==0;	}
		std::string getCodeString()const		{	return data.str().substr(start,length); }
		std::string getFilename()const			{	return filename.toString(); }
		const std::string & getFullCode()const	{	return data.str(); }
		size_t getLength()const					{	return length;	}
		size_t getStartPos()const				{	return start;	}
};


}
#endif // ES_CODE_FRAGMENT_H
