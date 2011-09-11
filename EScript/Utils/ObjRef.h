// ObjRef.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef OBJREF_H
#define OBJREF_H

#include <string>

namespace EScript {

/*! Simple (counted) reference to use with EReferenceCounter */
template<class _T>
class _CountedRef  {
		_T * obj;
	public:
		_CountedRef() : obj(NULL)                              {   }
		_CountedRef(_T * _obj) : obj(_obj)                     {   _T::addReference(obj);  }
		_CountedRef(const _CountedRef & other):obj(other.get()){   _T::addReference(obj);  }

		~_CountedRef()     {   _T::removeReference(obj);   }

		/*! Assignment */
		_CountedRef& operator=(const _CountedRef& other) {
			if (this == &other || other.obj==obj)
				return *this;
			_T::addReference(other.obj);
			_T::removeReference(obj);
			obj=other.obj;
			return *this;
		}

		/*! Detach the object from the Reference without decreasing
			the reference counter or deleting the object; sets the Reference
			to NULL. */
		_T * detach() {
			_T * o=obj;
			obj=NULL;
			return o;
		}

		/*! Detach the object from the Reference with decreasing
			the reference counter and set the Reference to NULL.
			The object is not deletet if the counter reaches 0. */
		_T * detachAndDecrease() {
			_T * o=obj;
			_T::decreaseReference(o);
			obj=NULL;
			return o;
		}

		/*! Swap the referenced pointers without touching the reference counter. */
		void swap(_CountedRef& other){
		_T * tmp=this->obj;
		this->obj=other.obj;
		other.obj=tmp;
		}

	/*! @name Information */
	// @{
		/*! Returns a pointer to the referenced Object. */
		inline _T * get()const                              {   return obj; }

		/*! Direct access to the referenced Object. */
		inline _T * operator->()const                       {   return obj; }

		/*! Returns true if the referenced object is NULL.  */
		inline bool isNull()const                           {   return obj==NULL;   }

		/*! Returns true if the referenced object is not NULL.  */
		inline bool isNotNull()const							{   return obj!=NULL;   }

		inline bool operator==(const _CountedRef & other)const 	{   return obj==other.obj;  }
		inline bool operator==(const _T * o2)const          	{   return obj==o2; }
		inline bool operator!=(const _CountedRef & other)const 	{   return obj!=other.obj;  }
		inline bool operator!=(const _T * o2)const          	{   return obj!=o2; }
	// @}
};



template<class _T>class EPtr;

/*! Smart pointer for referencing Objects (with implicit handling of the reference counter)
	with some safe conversion functions (toString etc.).
	ERef ---|> _CountedRef	*/
template<class _T>
class ERef : public _CountedRef<_T> {
	public:
		ERef() : _CountedRef<_T>(NULL)                              {   }
		ERef(_T * _obj) : _CountedRef<_T>(_obj)                     {   }
		ERef(const ERef<_T>& other) : _CountedRef<_T>(other.get())  {   }
		ERef(const EPtr<_T>& other) : _CountedRef<_T>(other.get())  {   }

		~ERef()     {	}

	/*! @name Conversion */
	// @{

		/*! Returns object->toBool() if the referenced object is not NULL, false otherwise.*/
		bool toBool()const                 		{   return this->isNull()?false:this->get()->toBool();    }

		/*! Returns object->toBool() if the referenced object is not NULL, defaultValue otherwise.  */
		bool toBool(bool defaultValue)const 	{   return this->isNull()?defaultValue:this->get()->toBool(); }

		/*! Returns object->toDouble() if the referenced object is not NULL, 0.0 otherwise.*/
		double toDouble()const              	{   return this->isNull()?0.0:this->get()->toDouble();    }

		/*! Returns object->toDouble() if the referenced object is not NULL, defaultValue otherwise.    */
		double toDouble(double defaultValue)const { return this->isNull()?defaultValue:this->get()->toDouble();   }

		/*! Returns object->toFloat() if the referenced object is not NULL, 0.0f otherwise. */
		float toFloat()const                	{   return this->isNull()?0.0f:this->get()->toFloat();    }

		/*! Returns object->toFloat() if the referenced object is not NULL, defaultValue otherwise.*/
		float toFloat(float defaultValue)const  {   return this->isNull()?defaultValue:this->get()->toFloat();    }

		/*! Returns object->toInt() if the referenced object is not NULL, 0 otherwise.  */
		int toInt()const                    	{   return this->isNull()?0:this->get()->toInt(); }

		/*! Returns object->toInt() if the referenced object is not NULL, defaultValue otherwise.   */
		int toInt(int defaultValue)const    	{   return this->isNull()?defaultValue:this->get()->toInt();  }
		
		//! Return object->toUInt() if the referenced object is not NULL, 0 otherwise.
		unsigned int toUInt() const				{	return this->isNull() ? 0u : this->get()->toUInt();	}

		//! Return object->toUInt() if the referenced object is not NULL, defaultValue otherwise.
		unsigned int toUInt(unsigned int defaultValue) const	{	return this->isNull() ? defaultValue : this->get()->toUInt();	}

		/*! Returns object->toString() if the referenced object is not NULL, "" otherwise.  */
		std::string toString()const         	{   return this->isNull()?"":this->get()->toString(); }

		/*! Returns object->toString() if the referenced object is not NULL, defaultValue otherwise.    */
		std::string toString(const std::string & defaultValue)const {
			return this->isNull()?defaultValue:this->get()->toString();
		}

		/*! Tries to convert object to given Type; returns NULL if object is NULL or not of given type. */
		template <class _T2> _T2 * toType()const    {   return this->isNull()?NULL:dynamic_cast<_T2*>(this->get());   }
};

/*! Weak smart pointer for referencing Objects (without implicit handling of the reference counter!)
	with some safe conversion functions (toString etc.).    */
template<class _T>
class EPtr{
		_T * obj;
	public:
		EPtr() : obj(NULL)                              {   }
		EPtr(_T * _obj) : obj(_obj)                     {   }
		EPtr(const EPtr<_T>& other) : obj(other.obj)    {   }
		EPtr(const ERef<_T>& other) : obj(other.get())  {   }

		EPtr& operator=(const EPtr<_T>& other) {
			obj=other.obj;
			return *this;
		}

		~EPtr()     {   }

	/*! @name Information */
	// @{
		/*! Returns a pointer to the referenced Object. */
		inline _T * get()const                              {   return obj; }

		/*! Direct access to the referenced Object. */
		inline _T * operator->()const                       {   return obj; }

		/*! Returns true if the referenced object is NULL.  */
		inline bool isNull()const                           {   return obj==NULL;   }

		/*! Returns true if the referenced object is not NULL.  */
		inline bool isNotNull()const							{   return obj!=NULL;   }

		inline bool operator==(const EPtr<_T> & other)const {   return obj==other.obj;  }
		inline bool operator==(const _T * o2)const          {   return obj==o2; }
		inline bool operator!=(const EPtr<_T> & other)const {   return obj==other.obj;  }
		inline bool operator!=(const _T * o2)const          {   return obj==o2; }
	// @}

	/*! @name Conversion */
	// @{

		/*! Returns object->toBool() if the referenced object is not NULL, false otherwise.*/
		bool toBool()const                  {   return isNull()?false:obj->toBool();    }

		/*! Returns object->toBool() if the referenced object is not NULL, defaultValue otherwise.  */
		bool toBool(bool defaultValue)const {   return isNull()?defaultValue:obj->toBool(); }

		/*! Returns object->toDouble() if the referenced object is not NULL, 0.0 otherwise.*/
		double toDouble()const              {   return isNull()?0.0:obj->toDouble();    }

		/*! Returns object->toDouble() if the referenced object is not NULL, defaultValue otherwise.    */
		double toDouble(double defaultValue)const { return isNull()?defaultValue:obj->toDouble();   }

		/*! Returns object->toFloat() if the referenced object is not NULL, 0.0f otherwise. */
		float toFloat()const                {   return isNull()?0.0f:obj->toFloat();    }

		/*! Returns object->toFloat() if the referenced object is not NULL, defaultValue otherwise.*/
		float toFloat(float defaultValue)const  {   return isNull()?defaultValue:obj->toFloat();    }

		/*! Returns object->toInt() if the referenced object is not NULL, 0 otherwise.  */
		int toInt()const                    {   return isNull()?0:obj->toInt(); }

		/*! Returns object->toInt() if the referenced object is not NULL, defaultValue otherwise.   */
		int toInt(int defaultValue)const    {   return isNull()?defaultValue:obj->toInt();  }
		
		//! Return object->toUInt() if the referenced object is not NULL, 0 otherwise.
		unsigned int toUInt() const			{	return isNull() ? 0u : obj->toUInt();	}

		//! Return object->toUInt() if the referenced object is not NULL, defaultValue otherwise.
		unsigned int toUInt(unsigned int defaultValue) const	{	return isNull() ? defaultValue : obj->toUInt();	}

		/*! Returns object->toString() if the referenced object is not NULL, "" otherwise.  */
		std::string toString()const         {   return isNull()?"":obj->toString(); }

		/*! Returns object->toString() if the referenced object is not NULL, defaultValue otherwise.    */
		std::string toString(const std::string & defaultValue)const {
			return isNull()?defaultValue:obj->toString();
		}

		/*! Tries to convert object to given Type; returns NULL if object is NULL or not of given type. */
		template <class _T2> _T2 * toType()const    {   return isNull()?NULL:dynamic_cast<_T2*>(obj);   }

};
class Object;

typedef ERef<Object> ObjRef;
typedef EPtr<Object> ObjPtr;
typedef EPtr<const Object> ObjPtr_const;
}
#endif // OBJREF_H
