// ReferenceObject.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef REFERENCE_OBJECT_H
#define REFERENCE_OBJECT_H

#include "../Object.h"

namespace EScript {

/*! [ReferenceObject] ---|> [Object]    */
template <typename _T>
class ReferenceObject : public Object {
		ES_PROVIDES_TYPE_NAME(ReferenceObject)
	public:
		// ---
		ReferenceObject(const _T & _obj, Type * type=NULL):
				Object(type),obj(_obj)					{	}
		virtual ~ReferenceObject()						{	}

		inline const _T & ref() const 					{	return obj;	}
		inline _T & ref()  								{	return obj;	}

		/// ---|> [Object]
		virtual Object * clone()const					{	return new ReferenceObject<_T>(ref());	}

		/// ---|> [Object]
		virtual bool rt_isEqual(Runtime &,const ObjPtr o){
			ReferenceObject<_T> * other=o.toType<ReferenceObject<_T> >();
			return other && obj == other->obj;
		}
	private:
		_T obj;
};

}

#endif // REFERENCE_OBJECT_H
