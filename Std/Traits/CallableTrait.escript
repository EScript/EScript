// CallableTrait.escript
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2013 Claudius Jähn <claudius@uni-paderborn.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------

var Traits = Std.require('Std/Traits/basics');

// ---------------------------

/*!	Marks a Type as callable.
	\param (optional) fun	Function called when an instance of the type is used as function.
							The first parameter is the instance object.
*/
Traits.CallableTrait := new Traits.Trait('Std.Traits.CallableTrait');

Traits.CallableTrait.init @(override) := fn(t,fun=void){
	if(fun){
		if(t---|>Type){
			t._call ::= fun;
		}else{
			t._call := fun;
		}
	}
};

Traits.addTrait(Function,		Traits.CallableTrait);
Traits.addTrait(UserFunction,	Traits.CallableTrait);
Traits.addTrait(Delegate,		Traits.CallableTrait);


return Traits.CallableTrait;
// ---------------------------------------
