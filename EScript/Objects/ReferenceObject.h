// ReferenceObject.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef REFERENCE_OBJECT_H
#define REFERENCE_OBJECT_H

#include "Object.h"
#include "../Utils/ObjArray.h"
#include "../Utils/RuntimeHelper.h"
#include <type_traits>
#include <utility>

namespace EScript {

//! (internal) Collection of comparators used for comparing ReferenceObjects.
namespace Policies{ //! \todo --> Policies_RefObjEqComparators
	struct EqualContent_ComparePolicy{
		template <typename ReferenceObject_t>
		static inline bool isEqual(ReferenceObject_t * a,const ObjPtr & b)	{
			ReferenceObject_t * other = b.toType<ReferenceObject_t >();
			return other && a->ref() == other->ref();
		}
	};

	struct SameEObjects_ComparePolicy{
		static inline bool isEqual(Object * a,const ObjPtr & b)	{	return a==b.get();	}
	};
}

/*! [ReferenceObject] ---|> [Object]
	A ReferenceObject can be used as wrapper for user defined C++ objects. The encapsulated
	data can be an object, a pointer or a smart reference and is defined by the first template parameter.

	The second template parameter defines how two instances are compared during an test for equality.
	If the default value 'Policies::EqualContent_ComparePolicy' is used, the two referenced values are
	compared using their '=='-operator (which has to be defined for the values's type).
	If 'Policies::SameEObjects' is used, the pointers of the two compared ReferenceObject are used
	for equality testing. The latter can be used if the values is an object (and not a reference) and
	the this object does not define a '==' operator.
*/
template <typename _T,typename comparisonPolicy = Policies::EqualContent_ComparePolicy >
class ReferenceObject : public Object {
		ES_PROVIDES_TYPE_NAME(ReferenceObject)
	public:
		typedef ReferenceObject<_T,comparisonPolicy> ReferenceObject_t;

		// ---
		
		//! (ctor) Uses referenced object's default constructor.
		ReferenceObject(Type * type) :
				Object(type), obj()							{	}

		//! (ctor) Passes a reference or rvalue reference to the object's constructor (if possible)
		template<typename other_type_t,
				 typename = typename std::enable_if<std::is_convertible<other_type_t, _T>::value>::type>
		explicit ReferenceObject(other_type_t && otherObject, Type * type = nullptr) :
			Object(type), obj(std::forward<other_type_t>(otherObject)) {
		}
		
		//! (ctor) Passes arbitrary parameters to the object's constructor.
		template<typename ...args>
		explicit ReferenceObject(Type * type,args&&... params) :
			Object(type), obj(std::forward<args>(params)...) {
		}

		virtual ~ReferenceObject()							{	}

		inline const _T & ref() const						{	return obj;	}
		inline _T & ref() 									{	return obj;	}

		inline const _T & operator*()const					{	return obj;	}
		inline _T & operator*()								{	return obj;	}

		/*! ---|> [Object]
			Direct cloning of a ReferenceObject is forbidden; but you may override the clone function in the specific implementation */
		virtual ReferenceObject_t * clone()const {
			throwRuntimeException("Trying to clone unclonable object '"+this->toString()+"'");
			return nullptr;
		}
		//! ---|> [Object]
		virtual bool rt_isEqual(Runtime &,const ObjPtr & o)	{	return comparisonPolicy::isEqual(this,o);	}
	private:
		_T obj;
};

}

#endif // REFERENCE_OBJECT_H
