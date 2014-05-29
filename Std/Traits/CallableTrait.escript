// CallableTrait.escript
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2013-2014 Claudius Jähn <ClaudiusJ@live.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------


/*!	Marks a Type as callable.
	\param (optional) fun	Function called when an instance of the type is used as function.
							The first parameter is the instance object.
*/
var t = new (module('./Trait'));

t.init @(override) := fn(t,fun=void){
	if(fun){
		if(t---|>Type){
			t._call ::= fun;
		}else{
			t._call := fun;
		}
	}
};

var Traits = module('./basics');
Traits.addTrait(Function,		t);
Traits.addTrait(UserFunction,	t);
Traits.addTrait(FnBinder,		t);


module.on('../StdNamespace', [t] => fn(t,StdNamespace){
	module('./basics').CallableTrait := t;
});

return t;
// ---------------------------------------
