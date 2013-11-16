// TraitBasics.escript
// This file is part of the EScript StdLib library.
// See copyright notice in ../basics.escript
// ------------------------------------------------------

loadOnce(__DIR__+"/../basics.escript");

/**
 **		This file contains the EScript Trait extension. 
 **/
Std.declareNamespace($Std,$Traits);

var Ns = Std.Traits;

//! (internal)
Ns._accessObjTraitRegistry := fn(obj,createIfNotExist = false){
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
Ns.addTrait := Std.Traits -> fn(obj, Std.Traits.Trait trait,params...){
	var name = trait.getName();

	var registry = this._accessObjTraitRegistry(obj,true);
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
Ns.addTraitByName := Ns -> fn(obj,String traitName,params...){
	this.addTrait(obj, this.getTraitByName(traitName), params...);
};

Ns.getTraitByName := fn(String traitName){
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
Ns.queryLocalTrait := Ns -> fn(obj,traitOrTraitName){
	var registry = this._accessObjTraitRegistry(obj,false);
	return registry ? 
					registry[traitOrTraitName---|>this.Trait ? traitOrTraitName.getName():traitOrTraitName] :
					false;
};

/*! Checks if the given object has a trait (the trait may be inherited).*/
Ns.queryTrait := Ns -> fn(obj,traitOrTraitName){
	var traitName = traitOrTraitName---|>Traits.Trait ? traitOrTraitName.getName():traitOrTraitName;
	while(obj){
		var reg = Traits._accessObjTraitRegistry(obj,false);
		if(reg && reg[traitName])
			return reg[traitName];
		obj = obj---|>Type ? obj.getBaseType() : obj.getType();
	}
	return false;
};

/*! Collects all traits of an object (including inherited traits).*/
Ns.queryTraits := Ns -> fn(obj){
	var traits = this._accessObjTraitRegistry(obj,false);
	traits = traits ? traits.clone() : new Map;
	for(var t = (obj---|>Type ? obj.getBaseType() : obj.getType()); t ; t = t.getBaseType()){
		var traits2 = this._accessObjTraitRegistry(t,false);
		if(traits2)
			traits.merge(traits2);
	}
	return traits;
};

/*! Throws an exception if the given object does not have the given trait. */
Ns.requireTrait := Ns -> fn(obj,traitOrTraitName){
	if(!this.queryTrait(obj,traitOrTraitName))
		Runtime.exception("Required trait not found\nObject:"+obj.toDbgString()+"\nTrait:"+traitOrTraitName);
};

// ---------------------------
/*! Base class for all Trait implementations.
	\note When creating a new Trait, you should consider using
		GenericTrait instead of this base class.	*/
Ns.Trait := new Type;
{
	var T = Ns.Trait;
	T._printableName @(override) ::= $Trait;
	T._traitName @(private) := void;

	//! If true, the Trait can be added multiple times to the same object.
	T.multipleUsesAllowed := false;

	/*! If a name is given, it is used to identify the trait. 
		Multiple traits offering the same behavior (with different implementations) 
		may provide the same name.	*/
	T._constructor ::= fn(name = void){
		if(name){
			_traitName = name;
			this._printableName @(override) := name;
		}
	};

	T.allowMultipleUses ::= 		fn(){	return this.setMultipleUsesAllowed(true);	};
	T.getMultipleUsesAllowed ::= fn(){	return multipleUsesAllowed;	};
	T.getName ::=				fn(){	return _traitName ? _traitName : toString();	};
	T.setMultipleUsesAllowed ::= fn(Bool b){	multipleUsesAllowed = b; 	return this;	};

	//! ---o
	T.init ::= fn(...){	Runtime.exception("This method is not implemented. Implement in subtype, or do not call!");	};
	
	
}


// ---------------------------


// Std._registerModuleResult("Std/Traits/basics",Ns); // support loading with Std.requireModule and loadOnce.
return Ns;
//
// ---------------------------------------
