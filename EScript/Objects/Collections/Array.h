// Array.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
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

		ESCRIPTAPI void init(const ParameterValues & p);
		ESCRIPTAPI void init(size_t num,Object* const* objs);
		ESCRIPTAPI void init(size_t num,char ** strings);
	public:
		ESCRIPTAPI static Array * create(Type * type = nullptr);
		ESCRIPTAPI static Array * create(const ParameterValues & p,Type * type = nullptr);
		ESCRIPTAPI static Array * create(size_t num,Object* const* objs,Type * type = nullptr);
		ESCRIPTAPI static Array * create(size_t num,char ** strings,Type * type = nullptr);

		template<class Collection_T>
		static Array * create(const Collection_T & collection){
			ERef<Array> a = Array::create();
			a->reserve(collection.size());
			for(auto & elem : collection)
				a->pushBack(EScript::create(elem));
			return a.detachAndDecrease();
		}

		ESCRIPTAPI static void release(Array * b);
		virtual ~Array()	{ }
	//	@}

	//---------------------

	//! @name TypeObject
	// @{
	public:
		ESCRIPTAPI static Type* getTypeObject();
		ESCRIPTAPI static void init(EScript::Namespace & globals);
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
		ESCRIPTAPI void append(Collection * c);
		Object * back()const					{	return empty() ? nullptr : (*(end()-1)).get();	}
		bool empty() const						{	return data.empty();	}
		iterator erase(iterator i)				{	return data.erase(i);	}
		iterator erase(iterator i,iterator j)	{	return data.erase(i,j);	}
		Object * front()const					{	return empty() ? nullptr : (*(begin())).get();	}
		Object * get(size_t index) const		{	return index<data.size() ?  data.at(index).get():nullptr;	}
		ESCRIPTAPI std::string implode(const std::string & delimiter=";");
		void popBack()							{	data.pop_back();	}
		void popFront()							{	data.erase(begin());	}
		void pushBack(const ObjPtr & obj)		{	if(!obj.isNull()) data.push_back(obj);	}
		void pushFront(const ObjPtr & obj)		{	if(!obj.isNull())	data.insert(begin(),obj.get());	}
		ESCRIPTAPI void removeIndex(size_t index);
		ESCRIPTAPI void reserve(size_t capacity);
		ESCRIPTAPI void resize(size_t newSize);
		ESCRIPTAPI void reverse();
		ESCRIPTAPI void rt_filter(Runtime & runtime,ObjPtr function);
		//! returns -1 if not found
		ESCRIPTAPI int rt_indexOf(Runtime & runtime,ObjPtr search,size_t begin = 0);
		ESCRIPTAPI size_t rt_removeValue(Runtime & runtime,const ObjPtr value,const int limit=-1,const size_t begin = 0);
		ESCRIPTAPI void rt_sort(Runtime & runtime,Object * function = nullptr,bool reverseOrder = false);
		size_t size() const						{	return data.size();		}
		ESCRIPTAPI ERef<Array> slice(int startIndex,int length)const;
		ESCRIPTAPI void splice(int startIndex,int length,Array * replacement);
		ESCRIPTAPI void swap(Array * other);
	//	@}

	//---------------------

	//! @name ---|> Collection
	// @{
	public:
		//!	[ArrayIterator] ---|> [Iterator] ---|> [Object]
		class ArrayIterator : public Iterator {
				ES_PROVIDES_TYPE_NAME(ArrayIterator)
			public:
				ESCRIPTAPI ArrayIterator(Array * ar);
				ESCRIPTAPI virtual ~ArrayIterator();

				//! ---|> [Object]
				ESCRIPTAPI Object * key() override;
				ESCRIPTAPI Object * value() override;
				ESCRIPTAPI void reset() override;
				ESCRIPTAPI void next() override;
				ESCRIPTAPI bool end() override;

				//! ---|> [Object]
			private:
				ERef<Array> arrayRef;
				size_t index;
		};

		ESCRIPTAPI Object * getValue(ObjPtr key) override;
		ESCRIPTAPI void setValue(ObjPtr key,ObjPtr value) override;
		ESCRIPTAPI size_t count()const override;
		ESCRIPTAPI ArrayIterator * getIterator() override;
		ESCRIPTAPI void clear() override;
	//	@}

	//---------------------

	//! @name ---|> Object
	// @{
		ESCRIPTAPI Object * clone()const override;
		internalTypeId_t _getInternalTypeId()const override	{	return _TypeIds::TYPE_ARRAY;	}
	//	@}

	//---------------------
};
}

#endif // COLLECTION_H
