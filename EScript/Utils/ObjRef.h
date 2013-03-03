// ObjRef.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef OBJREF_H
#define OBJREF_H

#include <cstddef>
#include <string>
#include <iostream>
#include "ConversionBasics.h"

namespace EScript {
template<class _T> class _Ptr;

//! Simple (counted) reference to use with EReferenceCounter
template<class _T>
class _CountedRef  {
		_T * obj;
	public:
		_CountedRef() : obj(nullptr)						 	{	}
		_CountedRef(_T * _obj) : obj(_obj)						{	_T::addReference(obj);	}
		_CountedRef(const _CountedRef & other):obj(other.get())	{	_T::addReference(obj);	}
		_CountedRef(_CountedRef && other):obj(other.get())		{	other.obj = nullptr;		}
		_CountedRef(const _Ptr<_T> & other):obj(other.get())	{	_T::addReference(obj);	}

		~_CountedRef()											{	_T::removeReference(obj);	}

		//! Just set; dont touch the counter.
		void _set(_T * _obj)									{	obj = _obj;	}

		//! Assignment
		_CountedRef& operator=(const _CountedRef& other) {
			if( other.obj!=obj ){
				_T::addReference(other.obj);
				_T::removeReference(obj);
				_set(other.obj);
			}
			return *this;
		}

		//! Assignment
		_CountedRef& operator=(_T * _obj) {
			if( _obj!=obj ){
				_T::addReference(_obj);
				_T::removeReference(obj);
				_set(_obj);
			}
			return *this;
		}
		
		//! Assignment
		_CountedRef& operator=(_CountedRef&& other) {
			swap(other);
			return *this;
		}
		
		/*! Detach the object from the Reference without decreasing
			the reference counter or deleting the object; sets the Reference
			to nullptr. */
		_T * detach() {
			_T * const o = obj;
			_set(nullptr);
			return o;
		}

		/*! Detach the object from the Reference with decreasing
			the reference counter and set the Reference to nullptr.
			The object is not deletet if the counter reaches 0. */
		_T * detachAndDecrease() {
			_T * const o = obj;
			_T::decreaseReference(o);
			_set(nullptr);
			return o;
		}

		//! Swap the referenced pointers without touching the reference counter.
		void swap(_CountedRef& other){
			_T * const tmp = obj;
			obj = other.obj;
			other.obj = tmp;
		}

	//! @name Information
	// @{
		//! Returns a pointer to the referenced Object.
		inline _T * get()const								{	return obj; }

		//! Direct access to the referenced Object.
		inline _T * operator->()const					 	{	return obj; }

		//! Returns true if the referenced object is nullptr.
		inline bool isNull()const						 	{	return obj==nullptr;	}

		//! Returns true if the referenced object is not nullptr.
		inline bool isNotNull()const							{	return obj!=nullptr;	}

		inline bool operator==(const _CountedRef & other)const	{	return obj==other.obj;	}
		inline bool operator==(const _Ptr<_T> & other)const	{	return obj==other.get();	}
		inline bool operator==(const _T * o2)const			{	return obj==o2; }
		inline bool operator!=(const _CountedRef & other)const	{	return obj!=other.obj;	}
		inline bool operator!=(const _Ptr<_T> & other)const		{	return obj!=other.get();	}
		inline bool operator!=(const _T * o2)const			{	return obj!=o2; }
	// @}
};

//! Simple wrapper for a pointer to a possibly
template<class _T>
class _Ptr  {
		_T * obj;
	public:
		_Ptr() : obj(nullptr)										{	}
		_Ptr(_T * _obj) : obj(_obj)								{	}
		_Ptr(const _CountedRef<_T> & other):obj(other.get())		{	}

		//! Swap the referenced pointers without touching the reference counter.
		void swap(_Ptr& other){
			_T * const tmp = obj;
			obj = other.obj;
			other.obj = tmp;
		}


		//! Tries to convert object to given Type; returns nullptr if object is nullptr or not of given type.
		template <class _T2> _T2 * toType()const	{	return isNull()?nullptr:dynamic_cast<_T2*>(obj);	}

	//! @name Information
	// @{
		//! Returns a pointer to the referenced Object.
		inline _T * get()const									{	return obj; }

		//! Direct access to the referenced Object.
		inline _T * operator->()const					 		{	return obj; }

		//! Returns true if the referenced object is nullptr.
		inline bool isNull()const						 		{	return obj==nullptr;	}

		//! Returns true if the referenced object is not nullptr.
		inline bool isNotNull()const								{	return obj!=nullptr;	}

		inline bool operator==(const _Ptr & other)const			{	return obj==other.obj;	}
		inline bool operator==(const _CountedRef<_T> & other)const	{	return obj==other.get();	}
		inline bool operator==(const _T * o2)const				{	return obj==o2; }
		inline bool operator!=(const _Ptr & other)const			{	return obj!=other.obj;	}
		inline bool operator!=(const _CountedRef<_T> & other)const	{	return obj!=other.get();	}
		inline bool operator!=(const _T * o2)const				{	return obj!=o2; }
	// @}
};



template<class _T>class EPtr;

/*! Smart pointer for referencing Objects (with implicit handling of the reference counter)
	with some safe conversion functions (toString etc.).
	ERef ---|> _CountedRef	*/
template<class _T>
class ERef : public _CountedRef<_T> {
	public:
		ERef() : _CountedRef<_T>(nullptr)							{	}
		ERef(_T * _obj) : _CountedRef<_T>(_obj)						{	}
		ERef(const EPtr<_T>& other) : _CountedRef<_T>(other.get())	{	}

	//! @name Conversion
	// @{
		//! Returns a value of the type target_t if possible or throws an exception.
		template<typename target_t>
		target_t to(Runtime &runtime){	return _Internals::doConvertTo<target_t>(runtime,*this); }

		/*! If the reference is null, the default value is returend; 
			otherwise a value of the type target_t is returned if possible or an exception is thrown.*/
		template<typename target_t>
		target_t to(Runtime &runtime,const target_t & defaultValue){	return this->isNull() ? defaultValue : _Internals::doConvertTo<target_t>(runtime,*this); }

		//! Returns object->toBool() if the referenced object is not nullptr, false otherwise.
		bool toBool()const						{	return this->isNull() ? false : this->get()->toBool();	}

		//! Returns object->toBool() if the referenced object is not nullptr, defaultValue otherwise.
		bool toBool(bool defaultValue)const	{	return this->isNull() ? defaultValue : this->get()->toBool(); }

		//! Returns object->toDbgString() if the referenced object is not nullptr, "[nullptr]" otherwise.
		std::string toDbgString()const			{	return this->isNull() ? "[nullptr]":this->get()->toDbgString(); }

		//! Returns object->toDouble() if the referenced object is not nullptr, 0.0 otherwise.
		double toDouble()const				{	return this->isNull() ? 0.0 : this->get()->toDouble();	}

		//! Returns object->toDouble() if the referenced object is not nullptr, defaultValue otherwise.
		double toDouble(double defaultValue)const { return this->isNull() ? defaultValue : this->get()->toDouble();	}

		//! Returns object->toFloat() if the referenced object is not nullptr, 0.0f otherwise.
		float toFloat()const					{	return this->isNull() ? 0.0f : this->get()->toFloat();	}

		//! Returns object->toFloat() if the referenced object is not nullptr, defaultValue otherwise.
		float toFloat(float defaultValue)const  {	return this->isNull() ? defaultValue : this->get()->toFloat();	}

		//! Returns object->toInt() if the referenced object is not nullptr, 0 otherwise.
		int toInt()const						{	return this->isNull() ? 0 : this->get()->toInt(); }

		//! Returns object->toInt() if the referenced object is not nullptr, defaultValue otherwise.
		int toInt(int defaultValue)const		{	return this->isNull() ? defaultValue : this->get()->toInt();	}

		//! Return object->toUInt() if the referenced object is not nullptr, 0 otherwise.
		unsigned int toUInt() const				{	return this->isNull() ? 0u : this->get()->toUInt();	}

		//! Return object->toUInt() if the referenced object is not nullptr, defaultValue otherwise.
		unsigned int toUInt(unsigned int defaultValue) const	{	return this->isNull() ? defaultValue  :  this->get()->toUInt();	}

		//! Returns object->toString() if the referenced object is not nullptr, "" otherwise.
		std::string toString()const			{	return this->isNull() ? "":this->get()->toString(); }

		//! Returns object->toString() if the referenced object is not nullptr, defaultValue otherwise.
		std::string toString(const std::string & defaultValue)const {
			return this->isNull() ? defaultValue : this->get()->toString();
		}

		//! Tries to convert object to given Type; returns nullptr if object is nullptr or not of given type.
		template <class _T2> _T2 * toType()const	{	return this->isNull()?nullptr : dynamic_cast<_T2*>(this->get());	}
};

/*! Weak smart pointer for referencing Objects (without implicit handling of the reference counter!)
	with some safe conversion functions (toString etc.).	*/
template<class _T>
class EPtr{
		_T * obj;
	public:
		EPtr() : obj(nullptr)								{	}
		EPtr(_T * _obj) : obj(_obj)							{	}
		EPtr(const ERef<_T>& other) : obj(other.get())		{	}

	//! @name Information
	// @{
		//! Returns a pointer to the referenced Object.
		inline _T * get()const								{	return obj; }

		//! Direct access to the referenced Object.
		inline _T * operator->()const						{	return obj; }

		//! Returns true if the referenced object is nullptr.
		inline bool isNull()const							{	return obj==nullptr;	}

		//! Returns true if the referenced object is not nullptr.
		inline bool isNotNull()const						{	return obj!=nullptr;	}

		inline bool operator==(const EPtr & other)const		{	return obj==other.obj;	}
		inline bool operator==(const _T * o2)const			{	return obj==o2; }
		inline bool operator!=(const EPtr & other)const		{	return obj!=other.obj;	}
		inline bool operator!=(const _T * o2)const			{	return obj!=o2; }
	// @}

	//! @name Conversion
	// @{
		//! Returns a value of the type target_t if possible or throws an exception.
		template<typename target_t>
		target_t to(Runtime &runtime){	return _Internals::doConvertTo<target_t>(runtime,*this); }

		/*! If the reference is null, the default value is returend; 
			otherwise a value of the type target_t is returned if possible or an exception is thrown.*/
		template<typename target_t>
		target_t to(Runtime &runtime,const target_t & defaultValue){	return isNull() ? defaultValue : _Internals::doConvertTo<target_t>(runtime,*this); }

		//! Returns object->toBool() if the referenced object is not nullptr, false otherwise.
		bool toBool()const					{	return isNull()?false:obj->toBool();	}

		//! Returns object->toBool() if the referenced object is not nullptr, defaultValue otherwise.
		bool toBool(bool defaultValue)const {	return isNull()?defaultValue:obj->toBool(); }

		//! Returns object->toDbgString() if the referenced object is not nullptr, "[nullptr]" otherwise.
		std::string toDbgString()const			{	return this->isNull() ? "[nullptr]":this->get()->toDbgString(); }

		//! Returns object->toDouble() if the referenced object is not nullptr, 0.0 otherwise.
		double toDouble()const				{	return isNull()?0.0:obj->toDouble();	}

		//! Returns object->toDouble() if the referenced object is not nullptr, defaultValue otherwise.
		double toDouble(double defaultValue)const { return isNull()?defaultValue:obj->toDouble();	}

		//! Returns object->toFloat() if the referenced object is not nullptr, 0.0f otherwise.
		float toFloat()const				{	return isNull()?0.0f:obj->toFloat();	}

		//! Returns object->toFloat() if the referenced object is not nullptr, defaultValue otherwise.
		float toFloat(float defaultValue)const	{	return isNull()?defaultValue:obj->toFloat();	}

		//! Returns object->toInt() if the referenced object is not nullptr, 0 otherwise.
		int toInt()const					{	return isNull()?0:obj->toInt(); }

		//! Returns object->toInt() if the referenced object is not nullptr, defaultValue otherwise.
		int toInt(int defaultValue)const	{	return isNull()?defaultValue:obj->toInt();	}

		//! Return object->toUInt() if the referenced object is not nullptr, 0 otherwise.
		unsigned int toUInt() const			{	return isNull() ? 0u : obj->toUInt();	}

		//! Return object->toUInt() if the referenced object is not nullptr, defaultValue otherwise.
		unsigned int toUInt(unsigned int defaultValue) const	{	return isNull() ? defaultValue : obj->toUInt();	}

		//! Returns object->toString() if the referenced object is not nullptr, "" otherwise.
		std::string toString()const			{	return isNull()?"":obj->toString(); }

		//! Returns object->toString() if the referenced object is not nullptr, defaultValue otherwise.
		std::string toString(const std::string & defaultValue)const {
			return isNull()?defaultValue:obj->toString();
		}
	
		//! Tries to convert object to given Type; returns nullptr if object is nullptr or not of given type.
		template <class _T2> _T2 * toType()const	{	return isNull()?nullptr:dynamic_cast<_T2*>(obj);	}

};
class Object;

typedef ERef<Object> ObjRef;
typedef EPtr<Object> ObjPtr;
typedef EPtr<const Object> ObjPtr_const;
}
#endif // OBJREF_H
