// StringUtils.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef STRINGUTILS_H
#define STRINGUTILS_H
#include <string>
#include <vector>

namespace EScript {

namespace StringUtils {

using std::string;

double getNumber(const char * s,int & length,bool checkSign=false);
string rTrim(const string & s);
string lTrim(const string & s);
string trim(const string & s);
string replaceAll(const string & subject,const string & find,const string & replace,int count=-1);
string replaceMultiple(const string & subject,int replaceCount,const string find[],const string replace[],int max=-1);

//! \note this is only a hack!
string UCS2LE_to_ANSII(int length,unsigned char * c);

bool beginsWith(const char * subject,const char * find);
bool nextLine(const char * subject,int & cursor);
string getLine(const char * subject);
bool stepWhitespaces(const char * subject,int & cursor);
bool stepText(const char * subject,int & cursor,const char * search);
string charToString(char c);

//! Split the subject at the occurence of delimiter into at most max parts.
void split(const string & subject,const string & delimiter, std::vector<std::string> & result,int max=-1);

}
}

#endif // STRINGUTILS_H
