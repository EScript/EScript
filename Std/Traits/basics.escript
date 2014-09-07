// basics.escript
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2013-2014 Claudius Jähn <ClaudiusJ@live.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------


/**
 **		This file contains the EScript Trait extension.
 **/
static Traits = new Namespace;
Traits._printableName @(override) := $Traits;
static Trait = module('./Trait');

//! (internal)
static _accessObjTraitRegistry = fn(obj,createIfNotExist = false){
	if(!obj.isSetLocally($__traits)){
		if(!createIfNotExist)
			return void;
		obj---|>Type ?
			(obj.__traits @(private) ::= new Map) :
			(obj.__traits @(private) := new Map);
	}
	return (obj->fn(){ return __traits;})(); // access private attribute
};

/*! Add a trait to the given object.
	The additional parameters are passed to the trait's init method. */
Traits.addTrait := fn(obj, Trait trait,params...){
	var name = trait.getName();

	var registry = _accessObjTraitRegistry(obj,true);
	if(registry[name] && !trait.multipleUsesAllowed){
		Runtime.exception("Adding a trait to an Object twice.\nObject:"+obj.toDbgString()+"\nTrait:"+name);
	}
	(trait->trait.init)(obj,params...);
	if(trait.multipleUsesAllowed){
		if(!registry[name])
			registry[name] = [];
		registry[name] += trait;

	}else{
		registry[name] = trait;
	}
};


/*! Add a trait to the given object. The trait is identified by its name.
	\note The trait's name must correspond to the EScript attribute structure beginning with GLOBALS.
			e.g. "Std.Traits.SingletonTrait" --> Std.Traits.SingletonTrait	*/
Traits.addTraitByName := fn(obj, String traitName, params...){
	Traits.addTrait(obj, Traits.getTraitByName(traitName), params...);
};

//! Adds the given trait if it has not already been added.
Traits.assureTrait := fn( object, trait ){
	if(!Traits.queryTrait(object,trait))
		Traits.addTrait(object,trait);
};

Traits.getTraitByName := fn(String traitName){
	var nameParts = traitName.split('.');
	var traitSearch = GLOBALS;
	foreach(nameParts as var p){
		traitSearch = traitSearch.getAttribute(p);
		if(!traitSearch)
			Runtime.exception("Unknown node trait '"+traitName+"'");
	}
	return traitSearch;

};

/*! Checks if the given object has a trait stored locally (and not by inheritance).*/
Traits.queryLocalTrait := fn(obj,traitOrTraitName){
	var registry = _accessObjTraitRegistry(obj,false);
	return registry ?
					registry[traitOrTraitName---|>Trait ? traitOrTraitName.getName():traitOrTraitName] :
					false;
};

/*! Checks if the given object has a trait (the trait may be inherited).*/
Traits.queryTrait := fn(obj,traitOrTraitName){
	var traitName = traitOrTraitName---|>Trait ? traitOrTraitName.getName():traitOrTraitName;
	while(obj){
		var reg = _accessObjTraitRegistry(obj,false);
		if(reg && reg[traitName])
			return reg[traitName];
		obj = obj---|>Type ? obj.getBaseType() : obj.getType();
	}
	return false;
};

/*! Collects all traits of an object (including inherited traits).*/
Traits.queryTraits := Traits -> fn(obj){
	var traits = _accessObjTraitRegistry(obj,false);
	traits = traits ? traits.clone() : new Map;
	for(var t = (obj---|>Type ? obj.getBaseType() : obj.getType()); t ; t = t.getBaseType()){
		var traits2 = _accessObjTraitRegistry(t,false);
		if(traits2)
			traits.merge(traits2);
	}
	return traits;
};

/*! Remove a trait from the given object. If the trait is not designed for removal, an exception is thrown.  */
Traits.removeTrait := fn(obj, Trait trait,params...){
	var name = trait.getName();
	if(!trait.getRemovalAllowed()){
		Runtime.exception("Trait '"+name+"' can not be removed from an object.");
	}
	var registry = _accessObjTraitRegistry(obj,true);
	if(!registry[name]){
		Runtime.exception("Trait '"+name+"' can not be removed from the object '"+obj.toDbgString()+
							"'.\nThe object does not have the trait.");
	}
	(trait->trait.onRemove)(obj,params...);
	registry.unset(name);
};

/*! Throws an exception if the given object does not have the given trait. */
Traits.requireTrait := fn(obj,traitOrTraitName){
	var trait = Traits.queryTrait(obj,traitOrTraitName);
	if(!trait)
		Runtime.exception("Required trait not found\nObject:"+obj.toDbgString()+"\nTrait:"+traitOrTraitName);
	return trait;
};


module.on('../StdNamespace', fn(StdNamespace){
	StdNamespace.Traits := Traits;
});
return Traits;
