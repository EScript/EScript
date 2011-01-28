#include "StringUtils.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>

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
        return static_cast<double>(sign?number:-number);
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
        return "";
    unsigned int start,end;
    for (start=0;start<s.length();start++) {
        char c=s[start];
        if (c==' '||c=='\t'||c=='\n'||c=='\r'||c=='\0'||c==11)
            continue;
        break;
    }
    for (end=s.length()-1;end>=start;end--) {
        char c=s[end];
        if (c==' '||c=='\t'||c=='\n'||c=='\r'||c=='\0'||c==11)
            continue;
        break;
    }
    int count=end-start+1;
    std::string s2="";
    if (count>0)
        s2=s.substr(start,count);
    return s2;
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
        nr++;
    }

    if (cursor<len) {
        s<<subject.substr(cursor,len-cursor);
    }

    return s.str();
}

string StringUtils::replaceMultiple(const string &subject,int replaceCount,const string find[],const string replace[],int max) {
    std::ostringstream s;

    unsigned int cursor=0;

    unsigned int  len=subject.length();
    unsigned int * findLen=new unsigned int[replaceCount];
    size_t * pos=new size_t[replaceCount];
    for (int i=0;i<replaceCount;++i) {
        findLen[i]=find[i].length();
        pos[i]=subject.find(find[i],0);
    }
    int nr=0;
    while (cursor<len&& nr!=max) {
        // select next match
        size_t nextPos=string::npos;
        int nextIndex=-1;
        for (int i=0;i<replaceCount;i++) {
            if (pos[i]==string::npos) continue;
            if (pos[i]<cursor) {
                pos[i]=subject.find(find[i],cursor);
            }
            if (pos[i]<nextPos) {
                nextIndex=i;
                nextPos=pos[i];
            }
        }
        if (nextPos==string::npos) break;

        s<<subject.substr(cursor,nextPos-cursor);
        s<<replace[nextIndex];
        cursor=nextPos+findLen[nextIndex];//findLen[nextIndex];

        nr++;
    }
    if (cursor<len) {
        s<<subject.substr(cursor,len-cursor);
    }
    delete [] findLen;
    delete [] pos;
//

    return s.str();
}

/**
 * TODO: 4byte encoding!
 */
string StringUtils::UCS2LE_to_ANSII(int length,unsigned char * c)   {
    std::ostringstream s;
    if (length%2==1)
        length--;
    for (int i=0;i<length;i+=2) {
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

bool StringUtils::nextLine(const char * subject,int & cursor) {
    while (subject[cursor]!=0) {
        cursor++;
        if (subject[cursor]=='\n') {
            cursor++;
            return subject[cursor] != 0;
        }
    }
    return false;
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

string StringUtils::charToString(char c) {
    char temp[2];
    temp[0]=c;
    temp[1]=0;
    return temp;
}

bool StringUtils::stepText(const char * subject,int & cursor,const char * search) {
    int _cursor=cursor;
    while (true) {
        char s=search[0];
        if (s==0) {
            cursor=_cursor;
            return true;
        }
        if (subject[_cursor]!=s ||subject[_cursor]==0 )
            return false;
        _cursor++;
        search++;
    }
}

string StringUtils::getLine(const char * subject) {
    std::ostringstream s;
    int cursor=0;
    char c=subject[cursor];
    while (c!=0&&c!='\n') {
        s<<c;
        c=subject[++cursor];
    }
    return s.str();
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


