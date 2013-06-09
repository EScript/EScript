// StringData.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "StringData.h"
#include <iostream>
#include <cassert>


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



//
//static const uint32_t INVALID_CODE_POINT = ~0;
//
//static uint32_t readCodePoint_utf8(const char* &cursor,const char*utf8End){
//	if(cursor >= utf8End )
//		return INVALID_CODE_POINT;
//	const uint8_t byte0 = static_cast<uint8_t>(*cursor);
//	if(byte0<0x80){ // 1 byte
//		++cursor;
//		return static_cast<uint32_t>(byte0);
//	}else if(byte0<0xE0){ // 2 byte sequence
//		if(byte0<0xC2 || cursor+1 >= utf8End)
//			return INVALID_CODE_POINT;
//		const uint8_t byte1 = static_cast<uint8_t>(*(cursor+1));
//		if( (byte1&0xC0) != 0x80 )
//			return INVALID_CODE_POINT;
//		cursor += 2;
//		return	(static_cast<uint32_t>(byte0&0x1F) << 6) + (byte1&0x3F) ;
//	}else if(byte0<0xF0){ // 3 byte sequence
//		if(cursor+2 >= utf8End)
//			return INVALID_CODE_POINT;
//		const uint8_t byte1 = static_cast<uint8_t>(*(cursor+1));
//		const uint8_t byte2 = static_cast<uint8_t>(*(cursor+2));
//		if( (byte1&0xC0) != 0x80 || (byte2&0xC0) != 0x80 )
//			return INVALID_CODE_POINT;
//		cursor += 3;
//		return	(static_cast<uint32_t>(byte0&0x0F) << 12) + 
//				(static_cast<uint32_t>(byte1&0x3F) << 6) + 
//				(byte2&0x3F) ;
//	}else if(byte0<0xF5){ // 4 byte sequence
//		if(cursor+3 >= utf8End)
//			return INVALID_CODE_POINT;
//		const uint8_t byte1 = static_cast<uint8_t>(*(cursor+1));
//		const uint8_t byte2 = static_cast<uint8_t>(*(cursor+2));
//		const uint8_t byte3 = static_cast<uint8_t>(*(cursor+3));
//		if( (byte1&0xC0) != 0x80 || (byte2&0xC0) != 0x80 || (byte3&0xC0) != 0x80 )
//			return INVALID_CODE_POINT;
//		cursor += 4;
//		return	(static_cast<uint32_t>(byte0&0x07) << 18) + 
//				(static_cast<uint32_t>(byte1&0x3F) << 12) + 
//				(static_cast<uint32_t>(byte2&0x3F) << 6) + 
//				(byte3&0x3F);
//	}else{
//		return INVALID_CODE_POINT;
//	}
//}

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

size_t StringData::getCodePointLocation(const size_t codePointIdx){
	static const uint32_t JUMP_TABLE_STEP_SIZE = 8;
	
	switch(data->dataType){
		case Data::RAW:
		case Data::ASCII:
			return codePointIdx;
		case Data::UNKNOWN_UNICODE:
		case Data::UNICODE_WITH_LENGTH:{	// create jumpTable
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
				return codePointIdx;
			}else{
				data->dataType = Data::UNICODE_WITH_JUMTABLE;
				if(!jumpTable.empty())
					data->jumpTable.reset(new std::vector<size_t>(std::move(jumpTable)));
			}
			break;
		}
		case Data::UNICODE_WITH_JUMTABLE:
			break;
		default:
			assert(false);
	}

	if(codePointIdx>=data->numCodePoints){
		return getDataSize();
	}else{
		const size_t jumpTableEntry = codePointIdx/JUMP_TABLE_STEP_SIZE;
		
		size_t byteCursor = jumpTableEntry>0 ? (*data->jumpTable.get())[jumpTableEntry-1] : 0;
		size_t codePointCursor = jumpTableEntry*JUMP_TABLE_STEP_SIZE;
		while(codePointCursor<codePointIdx){
			if(byteCursor>=getDataSize())
				return getDataSize();
			byteCursor += getUTF8CodePointLength(data->s.c_str()+byteCursor);
			++codePointCursor;
		}
		return byteCursor;
	}
}


std::string StringData::getSubStr(const size_t codePointStart, const size_t numCodePoints){
	const size_t startPos = getCodePointLocation(codePointStart);
	const size_t endPos = getCodePointLocation(codePointStart+numCodePoints);
	if(startPos>=getDataSize())
		return "";
	return data->s.substr(startPos,endPos-startPos);


}

}
