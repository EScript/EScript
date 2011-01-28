#ifndef STRING_H
#define STRING_H

#include "../Object.h"
#include <stack>
#include <string>

namespace EScript {

/*! [String] ---|> [Object] */
class String : public Object {
        ES_PROVIDES_TYPE_NAME(String)
	private:
		static std::stack<String *> stringPool;
    public:
        static Type* typeObject;
        static void init(EScript::Namespace & globals);

        static String * create(const std::string & s);
        static String * create(const std::string & s,Type*type);
        static void release(String * b);

        // ---
        String(const std::string & s,Type * type=NULL);
        virtual ~String();

        inline const std::string & getString()const;
        inline void setString(const std::string & s);
        inline void appendString(const std::string & s);

        /// ---|> [Object]
        virtual Object * clone()const;
        virtual std::string toString()const;
        virtual double toDouble()const;
        virtual int toInt()const;
        virtual bool toBool()const;
        virtual bool rt_isEqual(Runtime &rt,const ObjPtr o);
		virtual std::string toDbgString()const;

    private:
        std::string s;

};
// -----------------------------------------------------------------------------
// inline getter & setter
const std::string & String::getString()const {
    return s;
}
void String::setString(const std::string & _s) {
    this->s=_s;
}
void String::appendString(const std::string & _s) {
    this->s+=_s;
}
}

#endif // STRING_H
