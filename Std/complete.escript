// JSONDataStore.escript
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2014 Claudius Jähn <ClaudiusJ@live.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------

/*!	Load this file to init and load the complete Std library in one step.
	All modules are automatically registered in the returned namespace.
	If your project is based on modules, this file should NOT be loaded, but 
	the modules of the Std library should be used on demand.
	
	\code 
		GLOBALS.Std := load("PathToEScript/Std/complete.escript");
	
		// use the functions of the Std library ...
		var mp = new Std.MultiProcedure; 
		
	\endcode
*/
var absModuleLoader = load( __DIR__+"/module.escript";);
absModuleLoader.addSearchPath(__DIR__+"/..");

var module = absModuleLoader.createLoader("Std/complete");

module('./addRevocably');
module('./DataWrapper');
module('./DataWrapperContainer');
module('./declareNamespace');
module('./enum');
module('./generatorFn');
module('./info');
module('./JSONDataStore');
module('./MultiProcedure');
module('./ObjectSerialization');
module('./PriorityQueue');
module('./Set');
module('./TypeExtensions');
module('./Traits/basics');
module('./Traits/CallableTrait');
module('./Traits/DefaultComparisonOperatorsTrait');
module('./Traits/GenericTrait');
module('./Traits/PrintableNameTrait');
module('./Traits/Trait');

var Std = module('./StdNamespace');
Std.module := absModuleLoader;

module._registerModule("Std/complete",Std);

return Std;