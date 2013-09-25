// StringUtils.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "StringUtils.h"
#include <cstdlib>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <cstdint>

namespace EScript{

double StringUtils::readNumber(const char * s, std::size_t & cursor, bool checkSign) {
	char c = s[cursor];
	std::string accum="";
	bool sign = true;

	if( checkSign && c=='-' && s[cursor+1]>='0' &&  s[cursor+1]<='9' ) {
		++cursor;
		sign = false;
		c = s[cursor];
	}

	if(c=='0' && (s[cursor+1]=='x'|| s[cursor+1]=='X')) {
		++cursor;
		accum="0x";
		while(true) {
			++cursor;
			c = s[cursor];
			if( (c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F')) {
				accum+=c;
				continue;
			}
			break;
		}
		unsigned int number = 0;
		sscanf(accum.c_str(),"%x",&number);
		return sign?static_cast<double>(number) : -static_cast<double>(number);
	} else if(c=='0' && (s[cursor+1]=='b'|| s[cursor+1]=='B')) { // binaryNumber
		++cursor;
		double number = 0;
		while(true) {
			++cursor;
			c = s[cursor];
			if( c=='0' ){
				number *= 2;
			} else if( c=='1' ){
				number *= 2;
				++number;
			}else{
				break;
			}
		}
		return sign?number : -number;
	} else if(c>='0' && c<='9') {
		//const char * begin = s+cursor;
		int dot = 0;
		char numAccum[100];
		int i = 0;
		while(i<99) {
			numAccum[i++]=c;
			++cursor;
			c = s[cursor];
			if( isdigit(c) || (c=='.' && isdigit(s[cursor+1]) && dot++ < 1))
				continue;
			else if( (c=='E' ||c=='e')&& (s[cursor+1]=='+' || s[cursor+1]=='-')) {
				numAccum[i++]=c;
				++cursor;
				c = s[cursor];
				continue;
			}
			break;
		}
		numAccum[i]=0;

		double number = std::strtod(numAccum,nullptr);
		return sign?number:-number;
	}
	return 0;
}

std::string StringUtils::trim(const std::string & s) {
	if(s.empty())
		return std::string();
	unsigned int start,end;
	for(start = 0;start<s.length();++start) {
		const char c = s[start];
		if(c!=' '&&c!='\t'&&c!='\n'&&c!='\r'&&c!='\0'&&c!=11)
			break;
	}
	for(end = s.length()-1;end>=start;--end) {
		const char c = s[end];
		if(c!=' '&&c!='\t'&&c!='\n'&&c!='\r'&&c!='\0'&&c!=11)
		break;
	}
	const int count = end-start+1;
	return count>0 ? s.substr(start,count) : std::string();
}
std::string StringUtils::rTrim(const std::string & s){
	for(int right = s.length()-1 ; right >= 0 ; --right){
		const char c = s[right];
		if( !(c==' '||c=='\t'||c=='\n'||c=='\r'||c=='\0'||c==11))
			return s.substr(0,right+1);
	}
	return std::string();
}
std::string StringUtils::lTrim(const std::string & s){
	for(size_t left = 0 ; left<s.length() ; ++left){
		const char c = s[left];
		if( !(c==' '||c=='\t'||c=='\n'||c=='\r'||c=='\0'||c==11))
			return s.substr(left,s.length()-left);
	}
	return std::string();
}

std::string StringUtils::replaceAll(const std::string &subject,const std::string &find,const std::string &replace,int count) {
	std::ostringstream s;

	unsigned int cursor = 0;
	unsigned int len = subject.length();
	unsigned int fLen = find.length();
	//unsigned int pos = string::npos;
	int nr = 0;
	while(cursor<len&& nr!=count) {
		size_t pos = subject.find(find,cursor);

		//std::cout << " found "<<search<<" at "<< pos<<"\n";
		if(pos==std::string::npos) {
			break;
		}

		if(pos>cursor) {
			s<<subject.substr(cursor,pos-cursor);
			cursor = pos;
		}
		s<<replace;
		cursor+=fLen;
		++nr;
	}

	if(cursor<len) {
		s<<subject.substr(cursor,len-cursor);
	}

	return s.str();
}

std::string StringUtils::replaceMultiple(const std::string &subject,const std::vector<std::pair<std::string,std::string> > & rules,int max){
	typedef std::pair<std::string,std::string> keyValuePair_t;
	const size_t ruleCount = rules.size();
	std::vector<size_t> findLen(ruleCount);
	std::vector<size_t> pos(ruleCount);

	size_t i = 0;
	for(const auto & keyValuePair : rules) {
		// length of the search pattern
		findLen[i] = keyValuePair.first.length();
		// first position
		pos[i] = subject.find(keyValuePair.first, 0);
		++i;
	}
	int nr = 0;
	std::ostringstream s;
	size_t cursor = 0;
	const size_t len = subject.length();
	while(cursor<len&& nr!=max) {
		// select next match
		size_t nextPos = std::string::npos;
		size_t nextFindLength = 0;

		std::vector<keyValuePair_t>::const_iterator nextReplace = rules.begin();
		std::vector<keyValuePair_t>::const_iterator ruleIt = rules.begin();

		for(i = 0;i<ruleCount;++i,++ruleIt) {

			// search not found -> continue
			if(pos[i]==std::string::npos) {
				continue;
			}
			// stepped over position (overlapping foundings) -> search again
			if(cursor>pos[i]) {
				pos[i]=subject.find((*ruleIt).first,cursor);
			}
			// nearest founding?
			if(pos[i]<nextPos) {
				nextReplace = ruleIt;
				nextPos = pos[i];
				nextFindLength = findLen[i];
			}

		}
		// found nothing? -> finished
		if(nextPos==std::string::npos)
			break;

		// append string
		s<<subject.substr(cursor,nextPos-cursor);
		s<<(*nextReplace).second;
		cursor = nextPos+nextFindLength;

		++nr;
	}
	// add ending
	if(cursor<len)
		s<<subject.substr(cursor,len-cursor);

	return s.str();
}
/**
 * TODO: 4byte encoding!
 */
std::string StringUtils::UCS2LE_to_ANSII(const std::string & source)   {
	std::ostringstream s;
	size_t length = source.length();
	if(length%2==1)
		length--;
	const uint8_t * c = reinterpret_cast<const uint8_t*>(source.data());
	for(size_t i = 0;i<length;i+=2) {
		// ascii char
		if(c[i+1]==0x00) {
			s<<static_cast<char>(c[i]);
		}
		// wrong encoding (illegal character)
		else if(c[i]==0xfe && c[i+1]==0xff)
			throw "UCS2LE_to_ANSII wrong encoding! Try big ending instead.";
		else if(c[i]==0xff && c[i+1]==0xfe) // ignore
			continue;

//            // 4 byte TODO: http://en.wikipedia.org/wiki/UTF-16/UCS-2
//            else if(){
//            }
		// 2 byte //&#x2295;
		else {
			s<<"&#x"<<std::hex<<std::setfill ('0') <<
			static_cast<int>(0x0100*c[i+1]+c[i]) <<std::dec<<";";
		}
	}
	return s.str();
}

std::vector<std::string> StringUtils::split(const std::string & subject,const std::string & delimiter, int max){
	std::vector<std::string> result;
	const size_t len = subject.length();
	if(len>0){
		const size_t delimiterLen = delimiter.length();
		if(delimiterLen>len || delimiterLen==0){
			result.emplace_back(subject);
		}else{
			size_t cursor = 0;
			for( int i = 1 ; i!=max&&cursor<=len-delimiterLen ; ++i){
				size_t pos = subject.find(delimiter,cursor);
				if( pos==std::string::npos ) // no delimiter found? -> to the end
					pos = len;
				result.push_back( subject.substr(cursor,pos-cursor) );
				cursor = pos+delimiterLen;

				if(cursor==len){ // ending on delimiter? -> add empty part
					result.push_back("");
				}
			}
			if(cursor<len)
				result.push_back( subject.substr(cursor,len-cursor) );
		}
	}
	return result;
}

static std::vector<std::pair<std::string,std::string>> getEscapeRules(){
	typedef std::pair<std::string,std::string> keyValuePair_t;
	std::vector<keyValuePair_t> replace;
	replace.emplace_back("\"","\\\"");
	replace.emplace_back("\b","\\b");
	replace.emplace_back("\f","\\f");
	replace.emplace_back("\n","\\n");
	replace.emplace_back("\r","\\r");
	replace.emplace_back("\t","\\t");
	replace.emplace_back(std::string("\0",1),"\\0");
	replace.emplace_back("\\","\\\\");
	return replace;
}

std::string StringUtils::escape(const std::string & s){
	static const auto escapeRules = getEscapeRules();
	return replaceMultiple(s,escapeRules);
}

std::string StringUtils::getLine(const std::string &s,const int lineIndex){
	size_t cursor = 0;
	for(int i = 0;i<lineIndex;++i){
		cursor = s.find('\n',cursor);
		if(cursor == std::string::npos){
			return "";
		}
		++cursor;
	}
	return s.substr(cursor, s.find('\n',cursor)-cursor );
}
}
