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


double readNumber(const char * s, std::size_t & cursor, bool checkSign = false);
std::string rTrim(const std::string & s);
std::string lTrim(const std::string & s);
std::string trim(const std::string & s);
std::string replaceAll(const std::string & subject,const std::string & find,const std::string & replace,int count=-1);

//! Escape quotes, newlines and backslashes.
std::string escape(const std::string & s);

//! Replace all occurrences of the rules.first with the corresponding rules.second
std::string replaceMultiple(const std::string & subject,const std::vector<std::pair<std::string,std::string> > & rules,int max=-1);

//! \note this is only a hack!
std::string UCS2LE_to_ANSII(const std::string & str);

//! Split the subject at the occurrence of delimiter into at most max parts.
std::vector<std::string> split(const std::string & subject,const std::string & delimiter, int max=-1);

//! \note the first line has index 0
std::string getLine(const std::string &s,const int lineIndex);

size_t countCodePoints(const std::string& str_u8);

}
}

#endif // STRINGUTILS_H
