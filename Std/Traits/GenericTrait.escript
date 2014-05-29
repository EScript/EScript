// GenericTrait.escript
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2013-2014 Claudius Jähn <ClaudiusJ@live.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------

/*! GenericTrait ---|> Trait
	A GenericTrait offers:
	- attributes (including properties like @(private,init,...) that are added to the object(or type).
	- an onInit-method that is called on initialization.
	\note If you want to restrict the type of object to which the trait can be added,
		add an corresponding constraint to the onInit-method.
*/
var T = new Type(module('./Trait'));
T._printableName @(override) ::= $GenericTrait;

T.attributes @(init,public,const) := fn(){	return new Type;	};
//! ---o
T.onInit @(init,public,const) := module('../MultiProcedure');

//! ---|> Trait
T.init @(const,override) ::= fn(obj,params...){
	// init attributes
	if(obj---|>Type){// set attribute directly
		foreach(attributes._getAttributes() as var name,var value){
			var attrProperties = attributes.getAttributeProperties(name);
			if(obj.isSetLocally(name) && (attrProperties&EScript.ATTR_OVERRIDE_BIT) == 0){
				Runtime.warn("GenericTrait overwrites existing attribute: "+obj+"."+name);
			}
			obj.setAttribute(name,value,attrProperties);
		}
	}else{
		// set attributes, but convert type-attribute and initialize the attributes marked with 'init'.
		foreach(attributes._getAttributes() as var name,var value){
			var attrProperties = attributes.getAttributeProperties(name);
			if(obj.isSetLocally(name) && (attrProperties&EScript.ATTR_OVERRIDE_BIT) == 0){
				Runtime.warn("GenericTrait overwrites existing attribute: "+obj+"."+name);
			}
			// silently convert type-attributes to object-attributes
			if((attrProperties&EScript.ATTR_TYPE_ATTR_BIT)>0){
				attrProperties^=EScript.ATTR_TYPE_ATTR_BIT;
			}
			if((attrProperties&EScript.ATTR_INIT_BIT)>0){
				if(value---|>Type){
					value = new value;
				}else{
					value = value();
				}
			}
			obj.setAttribute(name,value,attrProperties);
		}
	}

	// call onInit(type,params...)
	this.onInit(obj,params...);
};

module.on('../StdNamespace', [T] => fn(T,StdNamespace){	
	module('./basics').GenericTrait := T;
});
return T ;
// ---------------------------------------
