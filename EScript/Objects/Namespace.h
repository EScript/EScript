#ifndef ES_Namespace_H
#define ES_Namespace_H

#include "ExtObject.h"

namespace EScript {

/*! [Namespace] ---|> [ExtObject] ---|> [Object]   */
class Namespace : public ExtObject {
        ES_PROVIDES_TYPE_NAME(Namespace)

	// -------------------------------------------------------------

	/*! @name Initialization */
	//	@{
	public:
		static Type * getTypeObject();
		static void init(Namespace & globals);
	//	@}

	// -------------------------------------------------------------

	/*! @name Main */
	//	@{
	public:
        Namespace();
        Namespace(Type * type);
        virtual ~Namespace();

        /// ---|> [Object]
        virtual Namespace * clone() const;
	//	@}
};

}
#endif // ES_Namespace_H
