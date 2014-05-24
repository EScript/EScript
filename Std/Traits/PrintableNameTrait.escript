// PrintableNameTrait.escript
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2013-2014 Claudius Jähn <claudiusj@live.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------

var GenericTrait = module('Std/Traits/GenericTrait');

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

var Traits = module('./basics');
Traits.PrintableNameTrait := t;

return t;
