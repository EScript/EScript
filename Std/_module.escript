/*
**  Copyright (c) Claudius Jähn (claudiusj@users.berlios.de), 2007-2014
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

/*! Module loader and manager.
	\note do not load directly, but load module.escript instead.
*/



// ------------------------------------------
// module loading

static moduleRegistry = new Map;
static moduleListeners = new Map;
static moduleSearchPaths = [ __DIR__+"/../" ]; // use the folder containing the Std-library as default.

static T = new Type;

T.thisModuleId @(private) := false;

T._printableName @(override) ::= "Std/ModuleLoader";

T._constructor ::= fn([String,void] moduleId){
	this.thisModuleId = moduleId;
};
//! Factory
T.createLoader ::= fn(p...){	return new T(p...);	};

T.relModuleIdToAbsModuleId @(private) ::= fn(String relModuleId){
	return IO.condensePath( (this.thisModuleId&&relModuleId.beginsWith('.')) ? 
									thisModuleId+"/../"+relModuleId : 
									relModuleId );
};

//! \todo allow aliases Std.setModuleAlias
T.addSearchPath ::= fn(String path){
	if(!path.endsWith("/"))
		path += "/";
	if(!moduleSearchPaths.contains(path))
		moduleSearchPaths+= path;
};

T.get ::= fn(String moduleId){
	var module = moduleRegistry.get(moduleId); //! \todo allow relative paths
	return module==$__INVALID_MODULE ? void : module;
};

/*!	If the module with the given @p moduleId has not been loaded before it is loaded and returned.
	If the module has bee loaded, it is returned.
	If the module can not be returned( the loading attempt failed), an exception is thrown.	*/
T.require ::= fn(String relModuleId){
	var moduleId = this.relModuleIdToAbsModuleId(relModuleId);
	var module;
	if(moduleRegistry.containsKey(moduleId)){
		module = moduleRegistry[moduleId];
	}else{
		moduleRegistry[moduleId] = $__INVALID_MODULE; // set key to mark the loading attempt.
				
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
				module = load(filename,{ $module : new T(moduleId) });
				break;
			}
		}else{
			Runtime.exception("Std.require: Module not found '"+moduleId+"'");
		}
		
		moduleRegistry[moduleId] = module;
		var callbacks = moduleListeners[moduleId];
		if(callbacks){
			moduleListeners[moduleId] = void;
			foreach(callbacks as var fun)
				fun(module);
		}
	}
	if( $__INVALID_MODULE==module ){ // The loading already failed.
		Runtime.exception("Std.require: Invalid module '"+moduleId+"'");
	}
	return module;
};
T._call ::= fn(caller,p...){
	return this.require(p...);
};


//! experimental
T._registerModule ::= fn(String moduleId,module){
	if(moduleRegistry.containsKey(moduleId))
		Runtime.warn("Std._registerModule: Replacing existing module '"+moduleId+"'");
	moduleRegistry[moduleId] = module;
};

//! experimental
T._unregisterModule ::= fn(String moduleId){
	moduleRegistry.unset(moduleId);
};

/*! Execute the @p callback when the module with @p moduleId is loaded.
	If the module is already loaded, the callback is executed immediately.
	The callback's parameter is the module.	*/
T.on ::= fn(String relModuleId,callback){
	var moduleId = this.relModuleIdToAbsModuleId(relModuleId);
	var module = this.get(moduleId);
	if(void==module){
		if(moduleListeners[moduleId])
			moduleListeners[moduleId] += callback;
		else
			moduleListeners[moduleId] = [callback];
	}else{
		callback(module);
	}
};

return new T(void);

// ------------------------------------------
