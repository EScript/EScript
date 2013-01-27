// AttributeContainer.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_AttributeContainer_H
#define ES_AttributeContainer_H

#include <unordered_map>
#include "Attribute.h"
#include "Hashing.h"

namespace EScript {

class Runtime;

//! [AttributeContainer]
class AttributeContainer {
	void operator=(const AttributeContainer & other);

	public:
		typedef std::unordered_map<StringId,Attribute> attributeMap_t;
		typedef attributeMap_t::iterator iterator;
		typedef attributeMap_t::const_iterator const_iterator;
		typedef attributeMap_t::size_type size_type;
		typedef attributeMap_t::value_type value_type;

		explicit AttributeContainer(const AttributeContainer & other);
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
		void cloneAttributesFrom(const AttributeContainer & other);
		const attributeMap_t & getAttributes()const						{	return attributes;	}
		void collectAttributes(std::unordered_map<StringId,Object *> & attrs);
		void initAttributes(Runtime & rt);
		void setAttribute(const StringId & id,const Attribute & attr)	{	attributes[id] = attr;	}
		size_t size()const												{	return attributes.size();	}

	private:
		attributeMap_t attributes;
};

}
#endif // ES_AttributeContainer_H
