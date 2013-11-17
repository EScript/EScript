// CommonTraits.escript
// This file is part of the EScript StdLib library.
// See copyright notice in ../basics.escript
// ------------------------------------------------------
var Traits = Std.require('Std/Traits/basics');
var GenericTrait = Std.require('Std/Traits/GenericTrait');

/*!
 **		Collection of common traits.
 **/
 

// these traits also count to the common traits
Std.require('Std/Traits/CallableTrait');

// -----

 /*!	Allows calling multiple methods when creating an instance of the type.
	- The methods can be added to the type's new static '_constructors' MultiProcedure.
	- All constructor parameters are passed to all registered methods.
	- Super-constructor parameters are not supported.
	- The type's '_constructor' method is set and should not be altered.
	\code 
		var T = new Type;
		Std.Traits.addTrait(T,Std.Traits.MultiConstructorTrait);
		T._constructors += fn(...){	outln("Constructing T..."); };
		
		var t = new T;
	\endcode
};
*/
Traits.MultiConstructorTrait := new GenericTrait("Std.Traits.MultiConstructorTrait");
Traits.MultiConstructorTrait.onInit += fn(Type type){
	if(type.isSetLocally($_constructor))
		Runtime.exception("Std.Traits.MultiConstructorTrait: Type already has a constructor.");
	type._constructors ::= new MultiProcedure;
	type._constructor @(public,const)::= fn(p...){
		(this->_constructors)(p...);
	};
};


// -----

Traits.PrintableNameTrait := new GenericTrait($PrintableNameTrait);
Traits.PrintableNameTrait.onInit += fn(obj,typename){	obj._printableName @(override) ::= typename;	};

 // ------

Traits.SingletonTrait := new GenericTrait($SingletonTrait);
{
	var t = Traits.SingletonTrait;
	t.attributes._constructor @(private) ::= fn(){};
	t.attributes.instance @(public) ::= fn(){
		if(!this.isSetLocally($__instance))
			this.__instance @(private) ::= new this;
		return this.__instance;
	};
	t.onInit += fn(Type t){}; // only accept types.
}
// ---------------------------------------
/*!sets up default compare functions for all objects ( <, >, <=, >=, ==, != )
 * @param smaller the < function has to be given as parameter, all others are redirected to it.
 * @note this changes the behaviour of == and != which no longer behave like === and !== 
 */
Traits.DefaultComparisonOperatorsTrait := new GenericTrait("Std.Traits.DefaultComparisonOperatorsTrait");
{
	var t = Traits.DefaultComparisonOperatorsTrait;

	t.attributes.'<'	::= fn(b){
		assert( b ---|> __compare_baseType , "tried to compare objects of different type.");
		return 	__compare_isSmaller(b);
	};
	t.attributes.'<='	::= fn(b){ return !(b < this);                };
	t.attributes.'>='	::= fn(b){ return !(this < b);                };
	t.attributes.'>'	::= fn(b){ return  (b < this);                };
	t.attributes.'=='	::= fn(b){ return !(this < b) && !(b < this); };
	t.attributes.'!='	::= fn(b){ return  (this < b) ||  (b < this); };

	t.onInit += fn(Type t,smaller){
		t.__compare_isSmaller @(private) ::= smaller;
		t.__compare_baseType @(private) ::= t; // store the calling Type as basetype. Objects of this type (or of a subtype) can be compared.
	};
};

return Std.Traits;
