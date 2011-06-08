// StringUtils.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef STRINGUTILS_H
#define STRINGUTILS_H
#include <string>
#include <utility>
#include <vector>

namespace EScript {

namespace StringUtils {


double getNumber(const char * s,int & length,bool checkSign=false);
std::string rTrim(const std::string & s);
std::string lTrim(const std::string & s);
std::string trim(const std::string & s);
std::string replaceAll(const std::string & subject,const std::string & find,const std::string & replace,int count=-1);

//! Escape quotes, newlines and backslashes.
std::string escape(const std::string & s);

//! Replace all occurances of the rules.first with the corresponding rules.second
std::string replaceMultiple(const std::string & subject,const std::vector<std::pair<std::string,std::string> > & rules,int max=-1);

//! \note this is only a hack!
std::string UCS2LE_to_ANSII(int length,unsigned char * c);

bool beginsWith(const char * subject,const char * find);
bool nextLine(const char * subject,int & cursor);
std::string getLine(const char * subject);
bool stepWhitespaces(const char * subject,int & cursor);
bool stepText(const char * subject,int & cursor,const char * search);
std::string charToString(char c);

//! Split the subject at the occurence of delimiter into at most max parts.
void split(const std::string & subject,const std::string & delimiter, std::vector<std::string> & result,int max=-1);

}
}

#endif // STRINGUTILS_H
