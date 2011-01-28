#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "ExtObject.h"

namespace EScript {

/*! [Exception] ---|> [ExtObject] ---|> [Object]  */
class Exception : public ExtObject {
        ES_PROVIDES_TYPE_NAME(Exception)
    public:
        static Type* typeObject;
        static void init(EScript::Namespace & globals);
        // ----

        Exception(const std::string & msg,int line=0,Type * type=NULL);
        virtual ~Exception();

        inline void setMessage(const std::string & newMessage);
        inline std::string getMessage()const;
        inline void setFilename(const std::string & newFilename);
        inline std::string getFilename()const;
        inline int getLine()const;
        inline void setLine(int newLine);

        /// ---|> [Object]
        virtual Object * clone()const;
        virtual std::string toString()const;

    protected:
        std::string msg;
        std::string filename;
        int line;
};

// -----------------------------------------------------------------------------
// inline getter & setter
void Exception::setMessage(const std::string & newMessage) {
    msg=newMessage;
}
std::string Exception::getMessage()const {
    return msg;
}
void Exception::setFilename(const std::string & newFilename) {
    filename=newFilename;
}
std::string Exception::getFilename()const {
    return filename;
}
int Exception::getLine()const {
    return line;
}
void Exception::setLine(int newLine) {
    line=newLine;
}
}

#endif // EXCEPTION_H
