// CommonTraits.escript
// This file is part of the EScript StdLib library.
// See copyright notice in ../basics.escript
// ------------------------------------------------------

var GenericTrait = Std.require('Std/Traits/GenericTrait');

/*! Changes a part of a the object's or type's name when converted to a string
	\param name (String or Identifier)
	\code
		var T = new Type;
		Std.Traits.addTrait( T, Std.Traits.PrintableNameTrait, "MyType" );
		outln( new T );
	\endcode
*/
var t = new GenericTrait('Std.Traits.PrintableNameTrait');
t.onInit += fn(obj,[String,Identifier] typename){
	if(obj---|>Type){
		obj._printableName @(override) ::= typename;
	}else{
		obj._printableName @(override) := typename;
	}
};

var Traits = Std.require('Std/Traits/basics');
Traits.PrintableNameTrait := t;

return t;
