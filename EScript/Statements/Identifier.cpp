#include "Identifier.h"

namespace EScript{

/*! (static) */
Identifier * Identifier::create( identifierId id){
	return new Identifier(id);
}

/*! (static) */
Identifier * Identifier::create( const std::string & s){
	return new Identifier(stringToIdentifierId(s));
}

/*! (ctor) */
Identifier::Identifier(const identifierId &_id):
        Object(),id(_id) {
    //ctor
}

/*! (dtor) */
Identifier::~Identifier() {
    //dtor
}

std::string Identifier::toString()const {
    return identifierIdToString(getId());
}

}
