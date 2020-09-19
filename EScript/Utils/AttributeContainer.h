// AttributeContainer.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2012-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef ES_AttributeContainer_H
#define ES_AttributeContainer_H

#include <unordered_map>
#include "Attribute.h"
#include "Hashing.h"

namespace EScript {

class Runtime;

//! [AttributeContainer]
class AttributeContainer {
	ESCRIPTAPI void operator=(const AttributeContainer & other);

	public:
		typedef std::unordered_map<StringId,Attribute> attributeMap_t;
		typedef attributeMap_t::iterator iterator;
		typedef attributeMap_t::const_iterator const_iterator;
		typedef attributeMap_t::size_type size_type;
		typedef attributeMap_t::value_type value_type;

		ESCRIPTAPI explicit AttributeContainer(const AttributeContainer & other);
		explicit AttributeContainer(){}
		~AttributeContainer(){}

		Attribute * accessAttribute(const StringId & id){
			const attributeMap_t::iterator f = attributes.find(id);
			return f==attributes.end() ? nullptr : &f->second;
		}
		const_iterator begin()const										{	return attributes.begin();	}
		const_iterator end()const										{	return attributes.end();	}
		iterator begin()												{	return attributes.begin();	}
		iterator end()													{	return attributes.end();	}
		void clear()													{	attributes.clear();	}
		ESCRIPTAPI void cloneAttributesFrom(const AttributeContainer & other);
		const attributeMap_t & getAttributes()const						{	return attributes;	}
		ESCRIPTAPI void collectAttributes(std::unordered_map<StringId,Object *> & attrs);
		ESCRIPTAPI void initAttributes(Runtime & rt);
		void setAttribute(const StringId & id,const Attribute & attr)	{	attributes[id] = attr;	}
		size_t size()const												{	return attributes.size();	}

	private:
		attributeMap_t attributes;
};

}
#endif // ES_AttributeContainer_H
