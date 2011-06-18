// StreamBase.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef STREAM_BASE_H
#define STREAM_BASE_H

#include "../ExtObject.h"
#include "../../Utils/ObjArray.h"

namespace EScript {

/*! (abstract) [StreamBase] ---|> [ExtObject] ---|> [Object]  */
class StreamBase : public ExtObject {
		ES_PROVIDES_TYPE_NAME(StreamBase)
	public:
		static Type * getTypeObject();
		static void init(EScript::Namespace & globals);
		// ---
		StreamBase(Type * type=NULL);
		virtual ~StreamBase();



};
}

#endif // STREAM_BASE_H
