// ReferenceObject.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef REFERENCE_OBJECT_H
#define REFERENCE_OBJECT_H

#include "Object.h"
#include "Exception.h"

namespace EScript {

/*! [ReferenceObject] ---|> [Object]    */
template <typename _T>
class ReferenceObject : public Object {
		ES_PROVIDES_TYPE_NAME(ReferenceObject)
	public:
		typedef ReferenceObject<_T> ReferenceObject_t;
		
		// ---
		ReferenceObject(const _T & _obj, Type * type=NULL):
				Object(type),obj(_obj)					{	}
		virtual ~ReferenceObject()						{	}

		inline const _T & ref() const 					{	return obj;	}
		inline _T & ref()  								{	return obj;	}

		/*! ---|> [Object]
			Direct cloning of a ReferenceObject is forbidden; but you may overide the clone function in the specific implementation */
		virtual ReferenceObject_t * clone()const {	
			throw new Exception(std::string("Trying to clone unclonable object '")+this->toString()+"'");
			
		}
		/// ---|> [Object]
		virtual bool rt_isEqual(Runtime &,const ObjPtr o){
			ReferenceObject_t * other=o.toType<ReferenceObject_t >();
			return other && obj == other->obj;
		}
	private:
		_T obj;
};

}

#endif // REFERENCE_OBJECT_H
