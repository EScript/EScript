// DataWrapperContainer.escript
// This file is part of the EScript StdLib library.
// See copyright notice in basics.escript
// ------------------------------------------------------

loadOnce(__DIR__+"/basics.escript");

// ----------------------------------------------------------------------------------------------------
// DataWrapperContainer

var T = new Type;
Std.DataWrapperContainer := T;
T._printableName @(override) ::= $DataWrapperContainer;

T.dataWrappers @(init,private) := Map;

//! ---o
T.onDataChanged @(init) := Std.require('Std/MultiProcedure');

/*! (ctor) */
T._constructor ::= fn([void,Map] source=void){
	if(source)
		this.merge(source);
};

T.assign ::= fn(Map _values,warnOnUnknownKey = true){
	foreach(_values as var key,var value)
		this.setValue(key,value,warnOnUnknownKey);
	return this;
};

T.clear ::= fn(){
	foreach(this.dataWrappers as var dataWrapper)
		detachDataWrapperSubscription(dataWrapper);
	this.dataWrappers.clear();
	return this;
};

T.count ::=			fn(){	return dataWrappers.count();	};

//! (internal)
T.detachDataWrapperSubscription @(private) ::= fn(dataWrapper){
	dataWrapper.onDataChanged.filter(
		this->fn(fun){
			return !(fun.isSet($dataWrapperContainer) && this==fun.dataWrapperContainer);
		});
};

//! Call to remove all cycling dependencies with the contained DataWrappers
T.destroy ::= fn(){
	this.clear();
	this.dataWrappers = void; // prevent further usage.
	return this;
};
T.empty ::=			fn(){	return this.dataWrappers.empty();	};

T.getValue ::= fn(key,defaultValue = void){
	var dataWrapper = this.dataWrappers[key];
	return dataWrapper ? dataWrapper() : defaultValue;
};
T.getDataWrapper ::= fn(key){	return this.dataWrappers[key];	};
T.getDataWrappers ::= fn(){	return this.dataWrappers.clone();	};

T.getValues ::= fn(){
	var m = new Map;
	foreach(this.dataWrappers as var key,var dataWrapper)
		m[key] = dataWrapper();
	return m;
};
T.containsKey ::= fn(key){
	return this.dataWrappers.containsKey(key);
};
T.merge ::= fn(Map _dataWrappers){
	foreach(_dataWrappers as var key,var dataWrapper)
		this.addDataWrapper(key,dataWrapper);
	return this;
};

/*! Add a new DataWrapper with the given key.
	\note calls onDataChanged(key, valueOfTheDataWrapper)	*/
T.addDataWrapper ::= fn(key, Std.DataWrapper dataWrapper){
	if(this.dataWrappers[key])
		this.unset(key);
	this.dataWrappers[key] = dataWrapper;

	var changedListener = fn(value){
		thisFn.dataWrapperContainer.onDataChanged(thisFn.key,value);
	}.clone(); // each entry gets its own function object
	// attach as attribute to allow identification for removing.
	changedListener.dataWrapperContainer := this;
	changedListener.key := key;
	dataWrapper.onDataChanged += changedListener;
	this.onDataChanged(key,dataWrapper());
	return this;
};

T.setValue ::= fn(key,value,warnOnUnknownKey = true){
	var dataWrapper = this.dataWrappers[key];
	if(dataWrapper){
		dataWrapper(value);
	}else{
		if(warnOnUnknownKey)
			Runtime.warn("Std.DataWrapperContainer.setValue(...) unknown entry '"+key+"'.");
	}
	return this;
};

T.unset ::= fn(key){
	var dataWrapper = this.dataWrappers[key];
	if(dataWrapper){
		this.detachDataWrapperSubscription(dataWrapper);
		this.dataWrappers.unset(key);
	}
	return this;
};

T._get ::= T.getValue;
T._set ::= fn(key,value){
	this.setValue(key,value);
	return value;
};

T.getIterator ::= fn(){
	var mapIterator = this.dataWrappers.getIterator();

	var it = new ExtObject;
	it.end := mapIterator->mapIterator.end;
	it.next := mapIterator->mapIterator.next;
	it.key := mapIterator->mapIterator.key;
	it.value := mapIterator->fn(){
		var v = this.value();
		return v ? v() : void;
	};
	return it;

};


 Std._registerModule("Std/DataWrapperContainer",T); // support loading with Std.requireModule and loadOnce.
//Std._markAsLoaded(__DIR__,__FILE__);

return T;
