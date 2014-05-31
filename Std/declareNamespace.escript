// generatorFn.escript
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2013-2014 Claudius Jähn <ClaudiusJ@live.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------


/*!	Declare a (possibly nested) namespace, if it is not already declared.
	\example
		declareNamespace( $Foo,$SomeNamespace,$SomeOtherNamespace );
		// makes sure that Foo.SomeNamespace.SomeOtherNamespace exists.
*/
static declareNamespace = fn( ids... ){
	var currentNamespace = GLOBALS;
	foreach(ids as var id){
		if( !currentNamespace.isSet(id) ){
			var ns = new Namespace;
			ns._printableName @(override) := id;
			currentNamespace.setAttribute(id,ns,EScript.ATTR_CONST_BIT);
		}
		currentNamespace = currentNamespace.getAttribute(id);
	}
	return currentNamespace;
};
module.on('./StdNamespace', fn(StdNamespace){
	StdNamespace.declareNamespace := declareNamespace;
});
return declareNamespace;