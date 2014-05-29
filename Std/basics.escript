/*
**  Copyright (c) Claudius Jähn <claudiusj@live.de>, 2007-2014
**
**  Permission is hereby granted, free of charge, to any person obtaining a copy of this
**  software and associated documentation files (the "Software"), to deal in the Software
**  without restriction, including without limitation the rights to use, copy, modify,
**  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
**  permit persons to whom the Software is furnished to do so, subject to the following
**  conditions:
**
**  The above copyright notice and this permission notice shall be included in all copies
**  or substantial portions of the Software.
**
**  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
**  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
**  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
**  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
**  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
**  OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*! This file includes the features needed by all other components of the Std-lib:
	 - declareNamespace(...)
*/

 // ------------------------------------------

/*!	Declare a (possibly nested) namespace, if it is not already declared.
	\example
		declareNamespace( $Foo,$SomeNamespace,$SomeOtherNamespace );
		// makes sure that Foo.SomeNamespace.SomeOtherNamespace exists.
*/
var declareNamespace = fn( ids... ){
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

declareNamespace($Std);
Std.declareNamespace := declareNamespace;
// ------------------------

Std.ABSTRACT_METHOD @(const) := fn(...){	Runtime.exception("This method is not implemented.");	};

return Std;

// ------------------------------------------
