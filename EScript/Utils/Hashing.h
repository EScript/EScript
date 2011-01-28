#ifndef ES_HASHING_H
#define ES_HASHING_H
#include <string>

namespace EScript {

typedef unsigned int hashvalue;
typedef hashvalue identifierId;

extern const identifierId IDENTIFIER_emptyStr;
extern const std::string ES_UNKNOWN_IDENTIFIER;

extern identifierId stringToIdentifierId(const std::string & s);
extern const std::string & identifierIdToString(identifierId id);

extern hashvalue _hash(const std::string & s);
extern hashvalue _hash(const char * c);

// @DEPRECEATED
inline hashvalue hash(const std::string & s) {
    return _hash(s);
}
// @DEPRECEATED
inline hashvalue hash(const char * c) {
    return _hash(c);
}

}
#endif // ES_HASHING_H
