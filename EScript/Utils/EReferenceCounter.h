// EReferenceCounter.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef EREFERENCECOUNTER_H
#define EREFERENCECOUNTER_H
#include <iostream>

namespace EScript{


/*! (Non virtual) base class for reference counting.
	@p Obj_t  Should be the new class itself.
	@p ObjReleaseHandler_T 	A class which has the function statc void release(Ojb_t *)	*/
template<class Obj_t, class ObjReleaseHandler_T>
class EReferenceCounter {
		int refCounter;

	public:
		/** Default constructor */
		EReferenceCounter():refCounter(0){}

		/** Default destructor */
		~EReferenceCounter(){}

		//! Returns the current number of references to this object.
		inline int countReferences()const			{	return refCounter;	}

		//! Increase the reference counter of @p o.
		static inline void addReference(Obj_t * o)	{
			if(o!=NULL)
				++o->refCounter;
		}

		//! Decrease the reference counter of @p o. If the counter is <= 0, the object is released.
		static inline void removeReference(Obj_t * o){
			if(o!=NULL && (--o->refCounter)==0)
				ObjReleaseHandler_T::release(o);
		}
		//! Decrease the reference counter of @p o. The object is never released.
		static inline void decreaseReference(Obj_t * o){
			if(o!=NULL)
				--o->refCounter;
		}
};

}
#endif // EREFERENCECOUNTER_H
