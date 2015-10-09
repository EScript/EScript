// StringData.cpp
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#include "StringData.h"
#include <algorithm>
#include <cassert>
#include <iostream>

namespace EScript{

// (internals)

std::stack<StringData::Data*> StringData::dataPool;

//! (static,internal)
StringData::Data * StringData::createData(const std::string & s){
	if(s.empty())
		return getEmptyData();
	else if(dataPool.empty()){
		return new Data(s,Data::UNKNOWN_UNICODE);
	}else{
		Data * d = dataPool.top();
		dataPool.pop();
		d->s = s;
		d->dataType = Data::UNKNOWN_UNICODE;
		d->numCodePoints = 0;
		return d;
	}
}
//! (static,internal)
StringData::Data * StringData::createData(const char * c,size_t size){
	if(size==0)
		return getEmptyData();
	else if(dataPool.empty()){
		return new Data(c,size,Data::UNKNOWN_UNICODE);
	}else{
		Data * d = dataPool.top();
		dataPool.pop();
		std::string s(c,size);
		d->s.swap(s);
		d->dataType = Data::UNKNOWN_UNICODE;
		d->numCodePoints = 0;
		return d;
	}
}

//! (static,internal)
void StringData::releaseData(Data * data){
	data->s.clear();
	dataPool.push(data);
}

//! (internal)
void StringData::setData(Data * newData){
	if(newData!=data){
		if(--data->referenceCounter <=0 )
			releaseData(data);
		++newData->referenceCounter;
		data = newData;
	}
}

//! (static,internal)
StringData::Data * StringData::getEmptyData(){
	struct UndeletableEmptyStringFactory{
		static Data * create(){
			Data * emptyString = new Data("",Data::ASCII);
			++emptyString->referenceCounter;
			return emptyString;
		}
	};
	static Data * emptyString = UndeletableEmptyStringFactory::create();
	return emptyString;
}

//! (internal)
static size_t getUTF8CodePointLength(const char* cursor){
	const uint8_t byte0 = static_cast<uint8_t>(*cursor);
	if(byte0<0x80){ // 1 byte
		return 1;
	}else if(byte0<0xE0){ // 2 byte sequence
		if(byte0<0xC2 ) // INVALID CHARACTER!!
			return 1;
		return 2;
	}else if(byte0<0xF0){ // 3 byte sequence
		return 3;
	}else if(byte0<0xF5){ // 4 byte sequence
		return 4;
	}else{
		return 1; // INVALID CHARACTER!!
	}
}
uint32_t StringData::getCodePoint(const size_t codePointIdx)const{
	const size_t startPos = codePointToBytePos(codePointIdx);
	const size_t endPos = std::min(getDataSize(), codePointToBytePos(codePointIdx+1));
	if(startPos>=getDataSize())
		return ~0;
	auto p = startPos;
	const uint32_t b0 = static_cast<uint32_t>(data->s.at(p++));
	if(p>=endPos)
		return b0;
	const uint32_t b1 = static_cast<uint32_t>(data->s.at(p++));
	if(p>=endPos)
		return ((b0&0x1F)<<6) | (b1&0x3F);
	const uint32_t b2 = static_cast<uint32_t>(data->s.at(p++));
	if(p>=endPos)
		return ((b0&0x0F)<<12) | ((b1&0x3F)<<6) | (b2&0x3F);
	const uint32_t b3 = static_cast<uint32_t>(data->s.at(p++));
	if(p>=endPos)
		return ((b0&0x07)<<18) | ((b1&0x3F)<<12) | ((b2&0x3F)<<6) | (b3&0x3F);
	return ~0;
}

size_t StringData::getNumCodepoints()const{
	if(data->dataType == Data::UNKNOWN_UNICODE){
		size_t codePointCounter = 0;
		const char * const end = data->s.c_str()+getDataSize();
		for(const char * cursor = data->s.c_str(); cursor<end; cursor += getUTF8CodePointLength(cursor) )
			++codePointCounter;
		data->numCodePoints = codePointCounter;
		if(codePointCounter == getDataSize())
			data->dataType = Data::ASCII;
		else
			data->dataType = Data::UNICODE_WITH_LENGTH;

	}
	return data->numCodePoints;
}

static const uint32_t JUMP_TABLE_STEP_SIZE = 8;

//! (internal)
void StringData::initJumpTable()const{
	std::vector<size_t> jumpTable;
	size_t codePointCursor = 0;
	const size_t numBytes = getDataSize();
	for(size_t byteCursor = 0; byteCursor<numBytes;
			byteCursor += getUTF8CodePointLength(data->s.c_str()+byteCursor) ){

		if( (codePointCursor%JUMP_TABLE_STEP_SIZE)==0 && byteCursor>0) // skip the initial 0
			jumpTable.emplace_back(byteCursor);

		++codePointCursor;
	}
	data->numCodePoints = codePointCursor;

	if(codePointCursor == getDataSize()){
		data->dataType = Data::ASCII;
	}else{
		data->dataType = Data::UNICODE_WITH_JUMTABLE;
		if(!jumpTable.empty())
			data->jumpTable.reset(new std::vector<size_t>(std::move(jumpTable)));
	}
}

//! (internal)
size_t StringData::codePointToBytePos(const size_t codePointIdx)const{
	if(codePointIdx==0)
		return empty() ? std::string::npos : 0;

	// init if necessary
	if(data->dataType == Data::UNKNOWN_UNICODE || data->dataType == Data::UNICODE_WITH_LENGTH)
		initJumpTable();

	// 8bit string -> direct access
	if(data->dataType == Data::RAW || data->dataType == Data::ASCII)
		return codePointIdx < getDataSize() ? codePointIdx : std::string::npos;


	if(codePointIdx>=data->numCodePoints){
		return std::string::npos;
	}else{
		const size_t jumpTableEntry = codePointIdx/JUMP_TABLE_STEP_SIZE;

		size_t byteCursor = jumpTableEntry>0 ? (*data->jumpTable.get())[jumpTableEntry-1] : 0;
		size_t codePointCursor = jumpTableEntry*JUMP_TABLE_STEP_SIZE;
		while(codePointCursor<codePointIdx){
			if(byteCursor>=getDataSize())
				return std::string::npos;
			byteCursor += getUTF8CodePointLength(data->s.c_str()+byteCursor);
			++codePointCursor;
		}
		return byteCursor;
	}
}

std::string StringData::getSubStr(const size_t codePointStart, const size_t numCodePoints)const{
	const size_t startPos = codePointToBytePos(codePointStart);
	const size_t endPos = codePointToBytePos(codePointStart+numCodePoints);
	if(startPos>=getDataSize())
		return "";
	return data->s.substr(startPos,endPos-startPos);
}

bool StringData::beginsWith(const std::string& subj,const size_t codePointStart)const{
	const size_t subjLength = subj.length();
	const size_t bytePos = codePointToBytePos(codePointStart);
	return (subjLength>0 && bytePos<str().length() && (bytePos+subjLength)<=str().length() ) ?
				str().compare(bytePos,subjLength,subj)==0 :
				false;
}

size_t StringData::find(const std::string& subj,const size_t codePointStart)const{
	const size_t subjLength = subj.length();

	if(subjLength==0 || subjLength>str().length())
		return std::string::npos;

	// init if necessary
	if(data->dataType == Data::UNKNOWN_UNICODE || data->dataType == Data::UNICODE_WITH_LENGTH)
		initJumpTable();

	// 8bit string -> use normal find
	if(data->dataType == Data::RAW || data->dataType == Data::ASCII)
		return str().find(subj,codePointStart);

	// perform linear search
	size_t byteCursor = codePointToBytePos(codePointStart);
	size_t codePointCursor = codePointStart;

	const size_t endByte = str().length() - subj.length();
	while(byteCursor<endByte){
		if( str().compare(byteCursor,subjLength,subj)==0 )
			return codePointCursor;
		byteCursor += getUTF8CodePointLength(data->s.c_str()+byteCursor);
		++codePointCursor;
	}
	return std::string::npos;
}

size_t StringData::rFind(const std::string& subj,const size_t codePointStart)const{
	const size_t subjLength = subj.length();

	if(subjLength==0 || subjLength>str().length())
		return std::string::npos;

	// init if necessary
	if(data->dataType == Data::UNKNOWN_UNICODE || data->dataType == Data::UNICODE_WITH_LENGTH)
		initJumpTable();

	// 8bit string -> use normal rfind
	if(data->dataType == Data::RAW || data->dataType == Data::ASCII)
		return str().rfind(subj,codePointStart);

	const size_t bytePos = str().rfind(subj,codePointToBytePos(codePointStart));
	if(bytePos==std::string::npos)
		return std::string::npos;

	// find code point for byte pos
	size_t codePointCursor = codePointStart;
	for(size_t byteCursor = codePointToBytePos(codePointStart); byteCursor>bytePos; --byteCursor){
		const uint8_t c = static_cast<uint8_t>(str().at(byteCursor));
		if(c<0x80 || c>0xBF )
			--codePointCursor;
	}
	return codePointCursor;
}



}
