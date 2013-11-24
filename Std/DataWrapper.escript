// DataWrapper.escript
// This file is part of the EScript StdLib library.
// See copyright notice in basics.escript
// ------------------------------------------------------

loadOnce(__DIR__+"/basics.escript");

// Problem: JSONDataWrapper refresh on get
// map getOrSet(key, default)????

var DataWrapper = new Type;
{
	/*! A DataWrapper encapsulates a value that can be internally stored as attribute,
		inside of a JSONValueStore, or which is accessible only by function calls.
		A DataWrapper provides an unified interface, independently from the real location of the value.
	*/
	var T = DataWrapper;
	Std.DataWrapper := T;
	T._printableName @(override) ::= $DataWrapper;

	T.value @(private) := void;

	//! ctor
	T._constructor ::= 			fn(_value=void){	this.value = _value;	};

	//! (internal) ---o
	T.doGet @(private) ::= 		fn(){	return this.value; };

	//! (internal) ---o
	T.doSet @(private) ::= 		fn(newValue){	this.value = newValue; };

	/*! ---o
		\note Do NOT alter the returned value if it is not a primitive value! If you have to, clone it and the set it again!	*/
	T.get ::= 					fn(){	return value;	};

	//! Like refresh(), but always calls onDataChanged(newData) even if the value didn't change.
	T.forceRefresh ::= fn(){
		this.value = this.doGet();
		this.onDataChanged(this.value);
	};

	/*! Called when the value has been changed.
		To add a custom listener use the '+=' operator. E.g.
		\code
			myDataWrapper += fn(newData){	out("The value is now:",newData); };
	*/
	T.onDataChanged @(init) := Std.require('Std/MultiProcedure');

	/*! Refresh the internal data from the dataWrapper's data source. If the data has changed,
		onDataChanged(newData) is called. This function has only to be called  manually if the connected data may change externally.*/
	T.refresh ::= fn(){
		var newValue = this.doGet();
		if( !(this.value==newValue) ){
			this.value = newValue;
			this.onDataChanged(newValue);
		}
	};

	/*! ---o
		Set a new value. If the value does not equal the old value, onDataChanged(...) is called. */
	T.set ::= fn(newValue){
		if(! (value==newValue) ){
			this.doSet(newValue);
			this.value = this.doGet();
			this.onDataChanged(newValue);
		}
		return this;
	};

	//! (static) Factory
	T.createFromValue ::= T->fn(_value){
		return new this(_value);
	};
	
	/*! Use a DataWrapper as a function without parameters to get its value; use it with one parameter to set its value.
		\code
			var myDataWrapper = Std.DataWrapper.createFromValue(5);
			myDataWrapper(10);  // is the same as myDataWrapper.set(10);
			out( myDataWrapper() ); // outputs '10'. Is the same as out( myDataWrapper.get() );
	*/
	Std.require('Std/Traits/basics').addTrait(T,Std.require('Std/Traits/CallableTrait'),fn(obj,params...){
		return params.empty() ? this.get() : this.set(params...);
	});

}
// ------------------------------------------
// (internal) AttributeWrapper ---|> DataWrapper
{
	var T = new Type(DataWrapper);
	T._printableName @(override) ::= $AttributeWrapper;

	//! ctor
	T._constructor ::= fn(_obj,Identifier _attr){
		this.obj @(private) := _obj;
		this.attr @(private) := _attr;
		this.value = this.doGet();
	};

	/*! ---|> DataWrapper
		Always refresh the value before returning it.	*/
	T.get @(override) ::= fn(){
		this.refresh();
		return this.value;
	};
		
	//! ---|> DataWrapper
	T.doGet @(override,private) ::= 	fn(){	return obj.getAttribute(attr);	};

	//! ---|> DataWrapper
	T.doSet @(override,private) ::= 	fn(newValue){	obj.assignAttribute(attr, newValue);	};

	/*! (static) Factory
		Creates a DataWrapper connected to an object's attribute.
		\code var someValue = DataWrapper.createFromAttribute( someObject, $attr );
		\note refreshOnGet is set to true. */
	DataWrapper.createFromAttribute ::= T->fn( obj, Identifier attrName){
		return new this(obj,attrName);
	};

}

// ------------------------------------------
// (internal) EntryWrapper ---|> DataWrapper
{
	var T = new Type(DataWrapper);

	T._printableName @(override) ::= $EntryWrapper;
	//! ctor
	T._constructor ::= fn(_collection,_key,defaultValue=void){
		this.collection @(private) := _collection;
		this.key @(private) := _key;
		if(void==_collection[_key]&&void!=defaultValue)
			_collection[_key] = defaultValue;
		this.value = this.doGet();
	};
	
	/*! ---|> DataWrapper
		Always refresh the value before returning it.	*/
	T.get @(override) ::= fn(){
		this.refresh();
		return this.value;
	};
	
	//! ---|> DataWrapper
	T.doGet @(override,private) ::= 	fn(){	return this.collection[this.key];	};

	//! ---|> DataWrapper
	T.doSet @(override,private) ::= 	fn(newValue){	this.collection[this.key] = newValue;	};

	/*! (static) Factory
		Creates a DataWrapper connected to a collection's entry.
		\code	var values = { 'foo' : 1, 'bar' : 2 };
				var someValue = DataWrapper.createFromEntry( values, key, 'foo' );
		\note refreshOnGet is set to true. */
	DataWrapper.createFromEntry ::= T->fn(collection, key, defaultValue=void){
		//! \todo query collectionInterface?

		return new this(collection,key,defaultValue);
	};

}

// ------------------------------------------
// (internal) FnDataWrapper ---|> DataWrapper
{
	var T = new Type(DataWrapper);
	T._printableName @(override) ::= $FnDataWrapper;
	//! ctor
	T._constructor ::= fn(_getter,_setter,Bool refreshOnGet){
		this.doGet @(override,private) := _getter;
		this.doSet @(override,private) := _setter ? _setter : fn(data){ }; // ignore
		this.value = this.doGet();
		if(refreshOnGet){ // optionally refresh value before returning it
			//! ---|> DataWrapper
			T.get @(override) := fn(){
				this.refresh();
				return this.value;
			};
		}
	};

	/*! (static) Factory
		Create a dataWrapper from a pair of functions.
		\code var someValue = Std.DataWrapper.createFromFunctions( A->fn(){ return this.m1;},  A->fn(newValue){ this.m1 = newValue; } );
		\param getter Parameterless function called to get the current value
		\param setter (optional; may be void) Function with one parameter called to set the current value
		\param refreshOnGet Iff true, the data is refreshed implicitly (=the getter is called) when calling get().
				Iff false, the last queried value is returned by get().
	*/
	DataWrapper.createFromFunctions ::= T->fn(getter, setter = void,refreshOnGet = false){
		return new this(getter,setter,refreshOnGet);
	};
}

return DataWrapper;
