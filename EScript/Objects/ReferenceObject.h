// ReferenceObject.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef REFERENCE_OBJECT_H
#define REFERENCE_OBJECT_H

#include "Object.h"
#include "Exception.h"

namespace EScript {

struct ReferenceObjectClonePolicies{
	//! Create a clone using the same value
	struct ByReference {
		template<typename ReferenceObject_T>
		static ReferenceObject_T * createClone(const ReferenceObject_T * original){
			return new ReferenceObject_T( original->ref() );
		}
	};
	//! Create a clone using the clone() member function of the (pointer-)value (value->clone())
	struct ExplicitClone {
		template<typename ReferenceObject_T>
		static ReferenceObject_T * createClone(const ReferenceObject_T * original){
			return new ReferenceObject_T( original->ref()->clone() );
		}
	};
	//! Issue an exception when trying to clone
	struct Unclonable {
		template<typename ReferenceObject_T>
		static ReferenceObject_T * createClone(const ReferenceObject_T * original){
			throw new Exception(std::string("Trying to clone unclonable object")+original->toString());
		}
	};
};

/*! [ReferenceObject] ---|> [Object]    */
template <typename _T,typename ClonePolicy = ReferenceObjectClonePolicies::ByReference >
class ReferenceObject : public Object {
		ES_PROVIDES_TYPE_NAME(ReferenceObject)
	public:
		typedef ReferenceObject<_T,ClonePolicy> ReferenceObject_t;
		
		// ---
		ReferenceObject(const _T & _obj, Type * type=NULL):
				Object(type),obj(_obj)					{	}
		virtual ~ReferenceObject()						{	}

		inline const _T & ref() const 					{	return obj;	}
		inline _T & ref()  								{	return obj;	}

		/// ---|> [Object]
		virtual ReferenceObject * clone()const			{	
			return ClonePolicy::createClone(this);
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
