// ExtReferenceObject.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef EXT_REFERENCE_OBJECT_H
#define EXT_REFERENCE_OBJECT_H

#include "../Utils/AttributeContainer.h"
#include "ReferenceObject.h"
#include "Type.h"

namespace EScript {


namespace Policies_AttributeProviders{

//! Use this policy to directly store the AttributeContainer inside the ExtReferenceObject.
class storeInEObject{
		AttributeContainer attributeContainer;
	protected:
		static AttributeContainer * getAttributeContainer(storeInEObject * obj,bool /*create*/){
			return &(obj->attributeContainer);
		}
		static bool areObjAttributesInitialized(storeInEObject * /*obj*/){
			return false;
		}
};

}

/*! [ExtReferenceObject] ---|> [Object]
	A Ext(entable)ReferenceObject can be used as wrapper for user defined C++ objects that can be enriched by user
	defined attributes. For a description how the C++-object is handled and how the equalityComparator works, \see ReferenceObject.h
	The way the AttributeContainer is stored is controlled by @tparam attributeProvider.
*/
template <typename _T,typename equalityComparator = _RefObjEqComparators::EqualContent, typename attributeProvider = Policies_AttributeProviders::storeInEObject >
class ExtReferenceObject : public Object, private attributeProvider {
		ES_PROVIDES_TYPE_NAME(ExtReferenceObject)
	public:
		typedef ExtReferenceObject<_T,equalityComparator,attributeProvider> ExtReferenceObject_t;

		// ---
		ExtReferenceObject(const _T & _obj, Type * type=NULL) :
					Object(type), attributeProvider(), obj(_obj){
			
			if(type!=NULL && !areObjAttributesInitialized(this)){
				type->copyObjAttributesTo(this);
			}

		}
		virtual ~ExtReferenceObject()						{	}


		/*! ---|> [Object]
			Direct cloning of a ExtReferenceObject is forbidden; but you may override the clone function in the specific implementation */
		virtual ExtReferenceObject_t * clone()const {
			throw new Exception(std::string("Trying to clone unclonable object '")+this->toString()+"'");
		}
		/// ---|> [Object]
		virtual bool rt_isEqual(Runtime &,const ObjPtr o)	{	return equalityComparator::isEqual(this,o);	}


	// -----

	/*! @name Reference */
	//	@{
	public:
		inline const _T & ref() const 						{	return obj;	}
		inline _T & ref()  									{	return obj;	}

	private:
		_T obj;
	//	@}

	// -----

	/*! @name Attributes */
	//	@{
	public:
		using attributeProvider::getAttributeContainer;
		using Object::_initAttributes;
		using Object::_accessAttribute;
		using Object::setAttribute;

		/// ---|> [Object]
		virtual Attribute * _accessAttribute(const identifierId id,bool localOnly){
			AttributeContainer * attrContainer = getAttributeContainer(this,false);
			Attribute * attr = attrContainer!=NULL ? attrContainer->accessAttribute(id) : NULL;
			return  ( attr!=NULL || localOnly || getType()==NULL) ? attr : getType()->findTypeAttribute(id);
		}
		
		/// ---|> [Object]
		virtual void _initAttributes(Runtime & rt){
			// if the type contains obj attributes, this object will surely also have some, so it is safe to init the attribute container.
			if(getType()!=NULL && getType()->getFlag(Type::FLAG_CONTAINS_OBJ_ATTRS) ){
				getAttributeContainer(this,true)->initAttributes(rt);
			}
		}

		/// ---|> [Object]
		virtual bool setAttribute(const identifierId id,const Attribute & attr){
			getAttributeContainer(this,true)->setAttribute(id,attr);
			return true;
		}

		/// ---|> [Object]
		virtual void getLocalAttributes(std::map<identifierId,Object *> & attrs){
			AttributeContainer * attrContainer = getAttributeContainer(this,false);
			if(attrContainer!=NULL)
				attrContainer->getAttributes(attrs);
		}


		void cloneAttributesFrom(const ExtReferenceObject_t * other){
			getAttributeContainer(this).cloneAttributesFrom(other->objAttributes);
		}
	// @}

};

}

#endif // EXT_REFERENCE_OBJECT_H
