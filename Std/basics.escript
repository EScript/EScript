/*
**  Copyright (c) Claudius Jähn (claudiusj@users.berlios.de), 2007-2013
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
	 - Parameter bindings using [params] =>
	 - declareNamespace(...)
*/
Array."=>" ::= fn(callable){
	var myWrapper = thisFn.wrapperFn.clone();
	myWrapper.wrappedFun := callable;
	myWrapper.boundParams := this.clone();
	return myWrapper;
};

// _getCurrentCaller() is used instead of "this", as "this" may not be defined if this function
// is called without a caller. This then results in a warning due to an undefined variable "this".
Array."=>".wrapperFn := fn(params...){	return (Runtime._getCurrentCaller()->thisFn.wrappedFun)(thisFn.boundParams...,params...);	};

 // ------------------------------------------

/*!	Declare a (possibly nested) namespace, if it is not already declared.
	\example
		declareNamespace( $Foo,$SomeNamespace,$SomeOtherNamespace );
		// makes sure that Foo.SomeNamespace.SomeOtherNamespace exists.
*/
var declareNamespace = fn( ids... ){
	var currentNamespace = GLOBALS;
	while(!ids.empty()){
		var id = ids.popFront();
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

// ------------------------------------------
// module loading

var moduleRegistry = new Map;
var moduleListeners = new Map;
var moduleSearchPaths = [];

//! \todo allow aliases Std.setModuleAlias
// on load

Std.addModuleSearchPath := [moduleSearchPaths] => fn(moduleSearchPaths, String path){
	if(!path.endsWith("/"))
		path += "/";
	if(!moduleSearchPaths.contains(path))
		moduleSearchPaths+= path;
};

Std.getModule := [moduleRegistry] => fn(moduleRegistry, String moduleId){
	var module = moduleRegistry.get(moduleId);
	return module==$__INVALID_MODULE ? void : module;
};


//! (internal)
var loader = [moduleSearchPaths] => fn(moduleSearchPaths, String moduleId){
	if(moduleId.contains("\\") || moduleId.contains("..") ||
			moduleId.beginsWith("/") || moduleId.contains("//") || moduleId.endsWith(".escript")){
		Runtime.exception("Std.require: Invalid moduleId '"+moduleId+"'");
	}
	foreach(moduleSearchPaths as var p){
		var filename = p + moduleId + ".escript";
	//	outln("Testing file: '" + filename +"'");
		if(IO.isFile(filename)){
			Runtime.log(Runtime.LOG_INFO,"[Std] Loading module '"+filename+"'.");
	//		outln("loading...");
			return load(filename);
		}
	}
	Runtime.exception("Std.require: Module not found '"+moduleId+"'");
};

Std.require := [loader,moduleRegistry,moduleListeners] => fn(loader,moduleRegistry,moduleListeners, String moduleId){
	var module;
	if(moduleRegistry.containsKey(moduleId)){
		module = moduleRegistry[moduleId];
	}else{
		moduleRegistry[moduleId] = $__INVALID_MODULE; // set key to mark the loading attempt.
		module = loader(moduleId); // throws an exception on failure.
		moduleRegistry[moduleId] = module;
		var callbacks = moduleListeners[moduleId];
		if(callbacks){
			moduleListeners[moduleId] = void;
			foreach(callbacks as var fun)
				fun(module);
		}
	}
	if( $__INVALID_MODULE==module){ // The loading already failed.
		Runtime.exception("Std.require: Invalid module '"+moduleId+"'");
	}
	return module;
};


//! experimental
Std._registerModule := [moduleRegistry] => fn(moduleRegistry, String moduleId,module){
	if(moduleRegistry.containsKey('moduleId'))
		Runtime.warn("Std._registerModule: Replacing existing module '"+moduleId+"'");
	moduleRegistry[moduleId] = module;
};

//! experimental
Std._unregisterModule := [moduleRegistry] => fn(moduleRegistry, String moduleId){
	moduleRegistry.unset(moduleId);
};

Std.onModule := [moduleRegistry,moduleListeners] => fn(moduleRegistry,moduleListeners, String moduleId,callback){
	var module = Std.getModule(moduleId);
	if(void==module){
		if(moduleListeners[moduleId])
			moduleListeners[moduleId] += callback;
		else
			moduleListeners[moduleId] = [callback];
	}else{
		callback(module);
	}
};

// ------------------------------------------
