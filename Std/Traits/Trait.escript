// basics.escript
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2013-2014 Claudius Jähn <ClaudiusJ@live.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------


// ---------------------------
/*! Base class for all Trait implementations.
	\note When creating a new Trait, you should consider using
		GenericTrait instead of this base class.	*/
static T = new Type;
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

//! Marks the trait as usable several times for a single object.
T.allowMultipleUses ::= 		fn(){	return this.setMultipleUsesAllowed(true);	};

/*! Marks the trait as removable. When calling Traits.removeTrait( obj, trait),
	the trait's onRemove( obj ) method is called and the trait's name is removed from
	the object's set of used traits.
	\note Normally traits should NOT be removable. Only add this feature if explicitly
		required.*/
T.allowRemoval ::= fn(){
	this.removalAllowed := true;
	if(!this.isSet($onRemove))
		this.onRemove := fn(){Runtime.exception("This method is not implemented.");};	// Std.ABSTRACT_METHOD;
	return this;
};
T.getMultipleUsesAllowed ::= 	fn(){	return multipleUsesAllowed;	};
T.getRemovalAllowed ::= 		fn(){	return this.isSet($removalAllowed);	};
T.getName ::=					fn(){	return _traitName ? _traitName : toString();	};
T.setMultipleUsesAllowed ::=	fn(Bool b){	multipleUsesAllowed = b; 	return this;	};

//! ---o
T.init ::= fn(...){	Runtime.exception("This method is not implemented. Implement in subtype, or do not call!");	};

/*! Parameter check:
	fn( Trait value )		... value should be a Trait
	fn( SomeTrait value )	... value should have SomeTrait
*/
T._checkConstraint @(override) ::= fn(value){
	@(once) static Traits = module('./basics');
	return T==this ? value.isA(T) : Traits.queryTrait(value,this);
};

module.on('../StdNamespace', fn(StdNamespace){
	module('./basics').Trait := T;
});


return T;
