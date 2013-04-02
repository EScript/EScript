// Array.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ARRAY_H
#define ARRAY_H

#include "Collection.h"
#include "../Iterator.h"
#include "../../Utils/ObjArray.h"
#include "../../Utils/StdFactories.h"
#include <vector>
#include <stack>

namespace EScript {

//! [Array]  ---|> [Collection] ---|> [Object]
class Array : public Collection {
		ES_PROVIDES_TYPE_NAME(Array)

	//---------------------

	//! @name Types
	// @{
	public:
		typedef ObjRef							value_type;

		typedef std::vector<value_type>			container_t;
		typedef container_t::iterator			iterator;
		typedef container_t::pointer			pointer;
		typedef container_t::const_pointer		const_pointer;
		typedef container_t::const_iterator		const_iterator;
		typedef container_t::reference			reference;
		typedef container_t::const_reference	const_reference;
		typedef container_t::size_type			size_type;

		typedef std::ptrdiff_t							difference_type;
		typedef std::reverse_iterator<iterator>			reverse_iterator;
		typedef std::reverse_iterator<const_iterator>	const_reverse_iterator;
	//	@}

	//---------------------

	//! @name Creation
	// @{
	private:
		static std::stack<Array *> pool;

		Array(Type * type = nullptr) : Collection(type?type:getTypeObject()){}

		void init(const ParameterValues & p);
		void init(size_t num,Object* const* objs);
		void init(size_t num,char ** strings);
	public:
		static Array * create(Type * type = nullptr);
		static Array * create(const ParameterValues & p,Type * type = nullptr);
		static Array * create(size_t num,Object* const* objs,Type * type = nullptr);
		static Array * create(size_t num,char ** strings,Type * type = nullptr);

		template<class Collection_T>
		static Array * create(const Collection_T & collection){
			ERef<Array> a = Array::create();
			a->reserve(collection.size());
			for(auto & elem : collection)
				a->pushBack(EScript::create(elem));
			return a.detachAndDecrease();
		}
		
		static void release(Array * b);
		virtual ~Array()	{ }
	//	@}

	//---------------------

	//! @name TypeObject
	// @{
	public:
		static Type* getTypeObject();
		static void init(EScript::Namespace & globals);
	//	@}

	//---------------------

	//! @name Data
	// @{
	private:
		container_t data;
	public:
		iterator begin()						{	return data.begin(); }
		const_iterator begin()const				{	return data.begin(); }
		iterator end()							{	return data.end(); }
		const_iterator end()const				{	return data.end(); }
		reverse_iterator rbegin()				{	return data.rbegin(); }
		const_reverse_iterator rbegin()const	{	return data.rbegin(); }
		reverse_iterator rend()					{	return data.rend(); }
		const_reverse_iterator rend()const		{	return data.rend(); }

		container_t & operator*()				{	return data;	}
		const container_t & operator*()const	{	return data;	}

		ObjRef & at(size_t idx)					{	return data.at(idx);	}
		const ObjRef & at(size_t idx)const		{	return data.at(idx);	}
		void append(Collection * c);
		Object * back()const					{	return empty() ? nullptr : (*(end()-1)).get();	}
		bool empty() const						{	return data.empty();	}
		iterator erase(iterator i)				{	return data.erase(i);	}
		iterator erase(iterator i,iterator j)	{	return data.erase(i,j);	}
		Object * front()const					{	return empty() ? nullptr : (*(begin())).get();	}
		Object * get(size_t index) const		{	return index<data.size() ?  data.at(index).get():nullptr;	}
		std::string implode(const std::string & delimiter=";");
		void popBack()							{	data.pop_back();	}
		void popFront()							{	data.erase(begin());	}
		void pushBack(const ObjPtr & obj)		{	if(!obj.isNull()) data.push_back(obj);	}
		void pushFront(const ObjPtr & obj)		{	if(!obj.isNull())	data.insert(begin(),obj.get());	}
		void removeIndex(size_t index);
		void reserve(size_t capacity);
		void resize(size_t newSize);
		void reverse();
		void rt_filter(Runtime & runtime,ObjPtr function, const ParameterValues & additionalValues);
		//! returns -1 if not found
		int rt_indexOf(Runtime & runtime,ObjPtr search,size_t begin = 0);
		size_t rt_removeValue(Runtime & runtime,const ObjPtr value,const int limit=-1,const size_t begin = 0);
		void rt_sort(Runtime & runtime,Object * function = nullptr,bool reverseOrder = false);
		size_t size() const						{	return data.size();		}
		Array * slice(int startIndex,int length);
		void splice(int startIndex,int length,Array * replacement);
		void swap(Array * other);
	//	@}

	//---------------------

	//! @name ---|> Collection
	// @{
	public:
		//!	[ArrayIterator] ---|> [Iterator] ---|> [Object]
		class ArrayIterator : public Iterator {
				ES_PROVIDES_TYPE_NAME(ArrayIterator)
			public:
				ArrayIterator(Array * ar);
				virtual ~ArrayIterator();

				//! ---|> [Object]
				virtual Object * key();
				virtual Object * value();
				virtual void reset();
				virtual void next();
				virtual bool end();

				//! ---|> [Object]
			private:
				ERef<Array> arrayRef;
				size_t index;
		};

		virtual Object * getValue(ObjPtr key);
		virtual void setValue(ObjPtr key,ObjPtr value);
		virtual size_t count()const;
		virtual ArrayIterator * getIterator();
		virtual void clear();
	//	@}

	//---------------------

	//! @name ---|> Object
	// @{
		virtual Object * clone()const;
		virtual internalTypeId_t _getInternalTypeId()const	{	return _TypeIds::TYPE_ARRAY;	}
	//	@}

	//---------------------
};
}

#endif // COLLECTION_H
