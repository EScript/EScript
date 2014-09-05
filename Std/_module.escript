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

static moduleAliases = new Map; // moduleId -> moduleId
static moduleRegistry = new Map; // moduleId -> moduleObj
static moduleListeners = new Map; // moduleId -> [ callbacks* ]
static moduleSearchPaths = [ __DIR__+"/../" ]; // use the folder containing the Std-library as default.

static T = new Type;

T.thisModuleId @(private) := false;

T._printableName @(override) ::= "Std/ModuleLoader";

T._constructor ::= fn([String,void] moduleId){
	this.thisModuleId = moduleId;
};
//! Factory
T.createLoader ::= fn(p...){	return new T(p...);	};

T.relModuleIdToAbsModuleId @(private) ::= fn(String relModuleId, referenceId=void){
	if(!referenceId)
		referenceId = this.thisModuleId;
	return IO.condensePath( (referenceId&&relModuleId.beginsWith('.')) ? 
									referenceId+"/../"+relModuleId : 
									relModuleId );
};

T.addSearchPath ::= fn(String path){
	path = path.replaceAll('\\','/');
	if(!path.endsWith("/"))
		path += "/";
	if(!moduleSearchPaths.contains(path))
		moduleSearchPaths+= path;
};
//! Return the module with the given id, if it has been loaded; void otherwise.
T.get ::= fn(String relModuleId){
	var moduleId = this.relModuleIdToAbsModuleId(relModuleId);
	if(moduleRegistry.containsKey(moduleId)){
		var moduleObj = moduleRegistry[ moduleId ];
		if(moduleObj!=$__INVALID_MODULE)
			return moduleObj;
	}
	if(moduleAliases.containsKey(moduleId)){
		return this.get( moduleAliases[moduleId] );
	}
	return void;
};
//! return the module's id
T.getId ::= fn(){	return this.thisModuleId;	};

/*!	If the module with the given @p moduleId has not been loaded before it is loaded and returned.
	If the module has bee loaded, it is returned.
	If the module can not be returned( the loading attempt failed), an exception is thrown.	*/
T.require ::= fn(String relModuleId){
	var moduleId = this.relModuleIdToAbsModuleId(relModuleId);
	var moduleObj;
	if(moduleRegistry.containsKey(moduleId)){
		moduleObj = moduleRegistry[moduleId];
	}else if( moduleAliases.containsKey(moduleId) ){
		return this.require( moduleAliases[moduleId] );
	}else{
		moduleRegistry[moduleId] = $__INVALID_MODULE; // set key to mark the loading attempt.
				
		if(moduleId.contains("\\") || moduleId.contains("..") ||
				moduleId.beginsWith("/") || moduleId.contains("//") || moduleId.endsWith(".escript")){
			Runtime.exception("Std.require: Invalid moduleId '"+moduleId+"'");
		}
		foreach(moduleSearchPaths as var p){
			var filename = p + moduleId + ".escript";
//			outln("Testing file: '" + filename +"'");
			if(IO.isFile(filename)){
				Runtime.log(Runtime.LOG_INFO,"[Std] Loading module '"+filename+"'.");
		//		outln("loading...");
				moduleObj = load(filename,{ $module : new T(moduleId) });
				break;
			}
		}else{
			Runtime.exception("Std.require: Module not found '"+moduleId+"'");
		}
		
		moduleRegistry[moduleId] = moduleObj;
		var callbacks = moduleListeners[moduleId];
		if(callbacks){
			moduleListeners[moduleId] = void;
			foreach(callbacks as var fun)
				fun(moduleObj);
		}
	}
	if( $__INVALID_MODULE==moduleObj ){ // The loading already failed.
		Runtime.exception("Std.require: Invalid module '"+moduleId+"'");
	}
	return moduleObj;
};
T._call ::= fn(caller,p...){
	return this.require(p...);
};


//! experimental
T._registerModule ::= fn(String relModuleId,moduleObj){
	var moduleId = this.relModuleIdToAbsModuleId(relModuleId);
	if(moduleRegistry.containsKey(moduleId))
		Runtime.warn("module._registerModule: Replacing existing module '"+moduleId+"'");
	moduleRegistry[moduleId] = moduleObj;
};

//! experimental
T._unregisterModule ::= fn(String moduleId){
	moduleRegistry.unset(moduleId);
};

T._setModuleAlias ::= fn(String relModuleAlias, String relTargetModuleId,Bool warnOnReplacement=true){
	var moduleAlias = this.relModuleIdToAbsModuleId(relModuleAlias);
	if(moduleRegistry.containsKey(moduleAlias))
		Runtime.warn("module._setModuleAlias: Setting alias for existing module '"+moduleAlias+"' (ignored)");
	if(moduleAliases.containsKey(moduleAlias) && warnOnReplacement)
		Runtime.warn("module._setModuleAlias: Replacing existing alias '"+moduleAlias+"'.");
//	outln( moduleAlias ," -> ", this.relModuleIdToAbsModuleId(relTargetModuleId));
	moduleAliases[ moduleAlias ] = this.relModuleIdToAbsModuleId(relTargetModuleId);
};

/*! Execute the @p callback when the module with @p moduleId is loaded.
	If the module is already loaded, the callback is executed immediately.
	The callback's parameter is the module.	*/
T.on ::= fn(String relModuleId,callback){
	var moduleId = this.relModuleIdToAbsModuleId(relModuleId);
	var moduleObj = this.get(moduleId);
	if(void==moduleObj){
		if(moduleListeners[moduleId])
			moduleListeners[moduleId] += callback;
		else
			moduleListeners[moduleId] = [callback];
	}else{
		callback(moduleObj);
	}
};

return new T(void);

// ------------------------------------------
