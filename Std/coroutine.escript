// coroutine.escript
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2013 Claudius Jähn <claudius@uni-paderborn.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------

loadOnce(__DIR__ + "/basics.escript");

/*! The Std.coroutine( fun ) function wraps the given @p function into an helper
	object, which simplifies the handling of a yield-iterator returned by
	the function. If the wrapped function yields, the yield-iterator is stored inside
	the wrapper. As long as the iterator remains valid, a call to the wrapper continues
	the wrapped function using the iterator.
	- The return value or the yield value of the wrapped function are passed as return value
		of the wrapper.
	- The caller and the parameters passed to the wrapper are only passed to the wrapped function
		if there is no active yield iterator. Otherwise, they are ignored.

	Example: Task handling using a coroutine.
	\code

	var mainLoop = new Std.MultiProcedure;
	//...

	// register a task
	mainLoop += coroutine( fn(){
		for(var i=0;i<1000;++i){
			// do some calculations...
			yield;
		}
		return $REMOVE;
	});

	while(true){
		mainLoop(); // performs the calculations; when finished, the task is removed and the main loop continues.
	}

	\endcode

*/
static Coroutine = new Type;
Coroutine._printableName @(override) := $Coroutine;
Coroutine.iterator @(private) := void;
Coroutine.fun @(private) := void;
Coroutine._constructor ::= fn(_fun){
	this.fun = _fun;
};
Coroutine._call ::= fn(caller,params...){
	var result;
	if(this.iterator){
		if(iterator.end()){
			this.iterator = void;
			result = (caller->this.fun)(params...);
		}else{
			this.iterator.next();
			result = this.iterator.value();
		}
	}else{
		result = (caller->this.fun)(params...);
	}

	if(result ---|> YieldIterator){
		this.iterator = result;
		result = result.value();
	}
	return result;
};
//! Returns true iff the yield-iterator is valid.
Coroutine.isActive ::= fn(){
	return this.iterator && !this.iterator.end();
};

//!	\see Std.Traits.CallableTrait
Std.onModule('Std/Traits/CallableTrait', fn( CallableTrait){
	Std.require('Std/Traits/basics').addTrait( Coroutine, CallableTrait );
});

var coroutine = fn(fun){	return new Coroutine(fun);	};
Std.coroutine := coroutine;
return coroutine;
