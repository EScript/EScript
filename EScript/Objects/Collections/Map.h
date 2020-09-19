// Map.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef MAP_H
#define MAP_H

#include "Collection.h"
#include "../Iterator.h"
#include "../../Utils/StdFactories.h"
#include <map>

namespace EScript {

//! [Map] ---|> [Collection] ---|> [Object]
class Map : public Collection {
		ES_PROVIDES_TYPE_NAME(Map)

	//---------------------

	//! @name Types
	// @{
	public:
		//!	MapEntry:   (key,value)
		struct MapEntry {
			MapEntry() {	}
			MapEntry(const ObjPtr & _key,const ObjPtr & _value) : key(_key),value(_value) {	}
			MapEntry(const MapEntry &other) : key(other.key),value(other.value) {}
			MapEntry & operator=(const MapEntry &other){
				if(this!=&other){
					key = other.key;
					value = other.value;
				}
				return *this;
			}
			ObjRef key;
			ObjRef value;
		};
		typedef std::map<std::string,MapEntry>	container_t;
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
	public:
		ESCRIPTAPI static Map * create();
		ESCRIPTAPI static Map * create(const std::unordered_map<StringId,Object *> &);

		template<class Map_T>
		static Map * create(const Map_T & m){
			ERef<Map> eM = Map::create();
			for(auto & keyValue : m)
				eM->setValue(EScript::create(keyValue.first),EScript::create(keyValue.second));
			return eM.detachAndDecrease();
		}
		// ---
		Map(Type * type = nullptr) : Collection(type?type:getTypeObject()){}
		virtual ~Map(){}
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
		container_t & operator*()				{	return data;	}
		const container_t & operator*()const	{	return data;	}

		iterator begin()						{	return data.begin(); }
		const_iterator begin()const				{	return data.begin(); }
		iterator end()							{	return data.end(); }
		const_iterator end()const				{	return data.end(); }
		reverse_iterator rbegin()				{	return data.rbegin(); }
		const_reverse_iterator rbegin()const	{	return data.rbegin(); }
		reverse_iterator rend()					{	return data.rend(); }
		const_reverse_iterator rend()const		{	return data.rend(); }

		bool empty()const						{	return data.empty();	}
		size_type erase(const std::string & key){	return data.erase(key);	}
		ESCRIPTAPI Object * getValue(const std::string & key);
		ESCRIPTAPI Object * getKeyObject(const std::string & key);
		ESCRIPTAPI void merge(Collection * c,bool overwrite = true);
		ESCRIPTAPI void rt_filter(Runtime & runtime,ObjPtr function, const ParameterValues & additionalValues);
		ESCRIPTAPI void unset(ObjPtr key);
		ESCRIPTAPI void swap(Map * other);
	//	@}

	//---------------------

	//! @name ---|> [Collection]
	// @{
		//!	[MapIterator] ---|> [Iterator]
		class MapIterator : public Iterator {
				ES_PROVIDES_TYPE_NAME(MapIterator)
			public:
				ESCRIPTAPI MapIterator(Map * ar);
				virtual ~MapIterator() { }

				//! ---|> [Iterator]
				ESCRIPTAPI Object * key() override;
				ESCRIPTAPI Object * value() override;
				ESCRIPTAPI void reset() override;
				ESCRIPTAPI void next() override;
				ESCRIPTAPI bool end() override;

			private:
				ERef<Map> mapRef;
				container_t::iterator it;
		};
		ESCRIPTAPI void clear() override;
		ESCRIPTAPI size_t count()const override;
		ESCRIPTAPI MapIterator * getIterator() override;
		ESCRIPTAPI Object * getValue(ObjPtr key) override;
		ESCRIPTAPI void setValue(ObjPtr key,ObjPtr value) override;
	//	@}

	//---------------------

	//! @name ---|> [Object]
	// @{
		ESCRIPTAPI Object * clone()const;
	//	@}

	//---------------------
};
}

#endif // MAP_H
