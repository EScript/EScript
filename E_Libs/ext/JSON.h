#ifndef ESCRIPT_JSON_H
#define ESCRIPT_JSON_H
#include <string>
#include <sstream>

namespace EScript {
    class Object;

	/**
	 *    JSON support for EScript
	 *    [static-helper]
	 */
	class JSON	{
	    public:
        static void toJSON(std::ostringstream & out,Object * obj,bool formatted=true,int level=0);
        static std::string toJSON(Object * obj,bool formatted=true);
        static Object* parseJSON(const std::string &s);
	};

}
#endif // ESCRIPT_JSON_H
