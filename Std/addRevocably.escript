// JSONDataStore.escript
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2014 Claudius Jähn <ClaudiusJ@live.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------

/*! Add a @p value to a @p collection and returns a function that
	removes the value from the collection when called.
	\note Adding and removing the value is done using the collection's '+=' and '-=' operators.
	\note The revoce-function returns $REMOVE to work together with MultiProcedures
	
	\code
	var MultiProcedure = module('Std/MultiProcedure');
	
	var someExtensionPoint = new MultiProcedure;
	
	var cleanup = new MultiProcedure;
	// extend someExtensionPoint
	cleanup += addRevocable( someExtensionPoint, fn(){	outln("temporary extension...");};
	
	// ... do something with someExtensionPoint
	
	cleanup(); // remove all temporary extensions from someExtensionPoint
	
	\endcode
	 
*/
static addRevocably = fn( collection, value ){
	collection += value;
	var revocer = fn(){
		if(thisFn.collection){
			var value = thisFn.value;
			var collection = thisFn.collection;
			thisFn.collection = void;
			thisFn.value = void;
			collection -= value;
		}
		return $REMOVE;
	}.clone();
	revocer.collection := collection;
	revocer.value := value;
	return revocer;
};
module.on('./StdNamespace', fn(StdNamespace){
	StdNamespace.addRevocably := addRevocably;
});
return addRevocably;