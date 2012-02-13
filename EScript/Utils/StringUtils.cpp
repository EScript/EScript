// StringUtils.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "StringUtils.h"
#include <cstdlib>
#include <cstdio>
#include <iomanip>
#include <locale>
#include <sstream>
#include <stdint.h>

using namespace EScript;
using std::string;


double StringUtils::getNumber(const char * s,int &cursor,bool checkSign) {
	char c=s[cursor];
	string accum="";
	bool sign=true;

	if ( checkSign && c=='-' && s[cursor+1]>='0' &&  s[cursor+1]<='9' ) {
		cursor++;
		sign=false;
		c=s[cursor];
	}

	if (c=='0' && (s[cursor+1]=='x'|| s[cursor+1]=='X')) {
		cursor++;
		accum="0x";
		while (true) {
			cursor++;
			c=s[cursor];
			if ( (c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F')) {
				accum+=c;
				continue;
			}
			break;
		}
		unsigned int number=0;
		sscanf(accum.c_str(),"%x",&number);
		return sign?static_cast<double>(number) : -static_cast<double>(number);
	} else if (c>='0' && c<='9') {
		//const char * begin=s+cursor;
		int dot=0;
		char numAccum[100];
		int i=0;
		while (i<99) {
			numAccum[i++]=c;
			cursor++;
			c=s[cursor];
			if ( isdigit(c) || (c=='.' && isdigit(s[cursor+1]) && dot++ < 1))
				continue;
			else if ( (c=='E' ||c=='e')&& (s[cursor+1]=='+' || s[cursor+1]=='-')) {
				numAccum[i++]=c;
				cursor++;
				c=s[cursor];
				continue;
			}
			break;
		}
		numAccum[i]=0;

		// float;
		if (dot) {
			double number=0;//std::atof(numAccum.c_str());
			sscanf(numAccum,"%lf",&number);
			//std::cout << number << " ";
			return sign?number:-number;
		} else {
			int number=static_cast<int>(std::atof(numAccum)); // atoi???
			//std::cout << number << " ";
			return sign?number:-number;
		}

	}
	return 0;
}

string StringUtils::trim(const string & s) {
	if(s.empty())
		return std::string();
	unsigned int start,end;
	for (start=0;start<s.length();++start) {
		char c=s[start];
		if (c==' '||c=='\t'||c=='\n'||c=='\r'||c=='\0'||c==11)
			continue;
		break;
	}
	for (end=s.length()-1;end>=start;--end) {
		char c=s[end];
		if (c==' '||c=='\t'||c=='\n'||c=='\r'||c=='\0'||c==11)
			continue;
		break;
	}
	const int count=end-start+1;
	return count>0 ? s.substr(start,count) : std::string();
}
string StringUtils::rTrim(const string & s){
	for(int right=s.length()-1 ; right >= 0 ; --right){
		char c=s[right];
		if( !(c==' '||c=='\t'||c=='\n'||c=='\r'||c=='\0'||c==11))
			return s.substr(0,right+1);
	}
	return std::string();
}
string StringUtils::lTrim(const string & s){
	for(size_t left=0 ; left<s.length() ; ++left){
		char c=s[left];
		if( !(c==' '||c=='\t'||c=='\n'||c=='\r'||c=='\0'||c==11))
			return s.substr(left,s.length()-left);
	}
	return std::string();
}

string StringUtils::replaceAll(const string &subject,const string &find,const string &replace,int count) {
	std::ostringstream s;

	unsigned int cursor=0;
	unsigned int len=subject.length();
	unsigned int fLen=find.length();
	//unsigned int pos=string::npos;
	int nr=0;
	while (cursor<len&& nr!=count) {
		size_t pos=subject.find(find,cursor);

		//std::cout << " found "<<search<<" at "<< pos<<"\n";
		if (pos==string::npos) {
			break;
		}

		if (pos>cursor) {
			s<<subject.substr(cursor,pos-cursor);
			cursor=pos;
		}
		s<<replace;
		cursor+=fLen;
		++nr;
	}

	if (cursor<len) {
		s<<subject.substr(cursor,len-cursor);
	}

	return s.str();
}

string StringUtils::replaceMultiple(const string &subject,const std::vector<std::pair<std::string,std::string> > & rules,int max){
	typedef std::pair<std::string,std::string> keyValuePair_t;
	const size_t ruleCount=rules.size();
	std::vector<size_t> findLen(ruleCount);
	std::vector<size_t> pos(ruleCount);

	size_t i=0;
	for(std::vector<keyValuePair_t>::const_iterator it=rules.begin();it!=rules.end();++it) {
		// length of the search pattern
		findLen[i]=(*it).first.length();
		// first position
		pos[i]=subject.find((*it).first,0);
		++i;
	}
	int nr=0;
	std::ostringstream s;
	size_t cursor=0;
	const size_t len=subject.length();
	while(cursor<len&& nr!=max) {
		// select next match
		size_t nextPos=string::npos;
		size_t nextFindLength=0;

		std::vector<keyValuePair_t>::const_iterator nextReplace=rules.begin();
		std::vector<keyValuePair_t>::const_iterator ruleIt=rules.begin();

		for(i=0;i<ruleCount;++i,++ruleIt) {

			// search not found -> continue
			if(pos[i]==string::npos) {
				continue;
			}
			// stepped over position (overlapping foundings) -> search again
			if(cursor>pos[i]) {
				pos[i]=subject.find((*ruleIt).first,cursor);
			}
			// nearest founding?
			if (pos[i]<nextPos) {
				nextReplace=ruleIt;
				nextPos=pos[i];
				nextFindLength=findLen[i];
			}

		}
		// found nothing? -> finished
		if (nextPos==string::npos)
			break;

		// append string
		s<<subject.substr(cursor,nextPos-cursor);
		s<<(*nextReplace).second;
		cursor=nextPos+nextFindLength;

		++nr;
	}
	// add ending
	if (cursor<len)
		s<<subject.substr(cursor,len-cursor);

	return s.str();
}
/**
 * TODO: 4byte encoding!
 */
string StringUtils::UCS2LE_to_ANSII(const std::string & source)   {
	std::ostringstream s;
	size_t length = source.length();
	if (length%2==1)
		length--;
	const uint8_t * c = reinterpret_cast<const uint8_t*>(source.data());
	for (size_t i=0;i<length;i+=2) {
		// ascii char
		if (c[i+1]==0x00) {
			s<<static_cast<char>(c[i]);
		}
		// wrong encoding (illegal character)
		else if (c[i]==0xfe && c[i+1]==0xff)
			throw "UCS2LE_to_ANSII wrong encoding! Try big ending instead.";
		else if (c[i]==0xff && c[i+1]==0xfe) // ignore
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

bool StringUtils::beginsWith(const char * subject,const char * find) {
	for(size_t i=0 ; find[i]!=0 ; ++i){
		if (subject[i]!=find[i])
			return false;
	}
	return true;
}

bool StringUtils::stepWhitespaces(const char * subject,int & cursor) {
	while (true) {
		char c=subject[cursor];

		if (c==' '||c=='\t'||c=='\n'||c=='\r'||c=='\0'||c==11) {
			cursor++;
		} else if (c==0) {
			return false;
		} else {
			return true;
		}
	}
}

bool StringUtils::stepText(const char * subject,int & cursor,const char * search) {
	int _cursor=cursor;
	while (true) {
		const char s=search[0];
		if (s==0) {
			cursor=_cursor;
			return true;
		}
		if (subject[_cursor]!=s ||subject[_cursor]==0 )
			return false;
		++_cursor;
		++search;
	}
}

void StringUtils::split(const string & subject,const string & delimiter, std::vector<std::string> & result, int max){
	const size_t len = subject.length();
	const size_t delimiterLen = delimiter.length();
	if(len==0){
		return;
	}else if(delimiterLen>len || delimiterLen==0){
		result.push_back(subject);
	}else{
		size_t cursor = 0;
		for( int i=1 ; i!=max&&cursor<=len-delimiterLen ; ++i){
			size_t pos = subject.find(delimiter,cursor);
			if( pos==string::npos ) // no delimiter found? -> to the end
				pos=len;
			result.push_back( subject.substr(cursor,pos-cursor) );
			cursor=pos+delimiterLen;

			if(cursor==len){ // ending on delimiter? -> add empty part
				result.push_back("");
			}
		}
		if (cursor<len)
			result.push_back( subject.substr(cursor,len-cursor) );
	}
}

std::string StringUtils::escape(const std::string & s){
	typedef std::pair<std::string,std::string> keyValuePair_t;
	std::vector<keyValuePair_t> replace;
	replace.push_back(keyValuePair_t("\"","\\\""));
	replace.push_back(keyValuePair_t("\b","\\b"));
	replace.push_back(keyValuePair_t("\f","\\f"));
	replace.push_back(keyValuePair_t("\n","\\n"));
	replace.push_back(keyValuePair_t("\r","\\r"));
	replace.push_back(keyValuePair_t("\t","\\t"));
	replace.push_back(keyValuePair_t(std::string("\0",1),"\\0"));
	replace.push_back(keyValuePair_t("\\","\\\\"));
	return replaceMultiple(s,replace);
}

std::string StringUtils::toLower(const std::string & s){
	const size_t length = s.length();
	if(length==0)
		return "";
	char * buffer = new char[length];
	std::copy(s.c_str(),s.c_str()+length,buffer);
	std::locale loc;
	std::use_facet< std::ctype<char> >(loc).tolower( buffer, buffer+length );
	const std::string output(buffer,length);
	delete[]buffer;
	return output;
}


std::string StringUtils::toUpper(const std::string & s){
	const size_t length = s.length();
	if(length==0)
		return "";
	char * buffer = new char[length];
	std::copy(s.c_str(),s.c_str()+length,buffer);
	std::locale loc;
	std::use_facet< std::ctype<char> >(loc).toupper( buffer, buffer+length );
	const std::string output(buffer,length);
	delete[]buffer;
	return output;
}
