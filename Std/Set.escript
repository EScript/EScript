// Set.escript
// This file is part of the EScript StdLib library.
// See copyright notice in basics.escript
// ------------------------------------------------------

loadOnce(__DIR__+"/basics.escript");

/*!
 ** Set data structure
 */
var T = new Type;
Std.Set := T;

T._printableName @(override) ::= $Set;

T.data @(private,init) := Map;

//! (ctor)
T._constructor ::= fn( values... ){
	foreach(values as var v)
		this += v;
};

T._accessData ::= fn(){
	return this.data;
};

T.add ::= fn(value){
	this.data[value]=value;
	return this;
};
T.clear ::= fn(){
	data.clear();
	return this;
};
T.clone ::= fn(){
	var c = new (this.getType());
	(c->fn(clonedData){ this.data = clonedData;})(this.data.clone());
	return c;
};
T.contains ::= fn(value){
	return data.containsKey(value);
};
T.count ::= fn(){
	return data.count();
};
T.getIntersection ::= fn(Std.Set other){
	if(other.count()<this.count())
		return other.getIntersection(this);
	var s = new (this.getType());
	foreach(this as var value){
		if(other.contains(value))
			s+=value;
	}
	return s;
};
T.getIterator ::= fn(){
	return data.getIterator();
};
T.getMerged ::= fn(Std.Set other){
	var a = this.clone();
	return a.merge(other);
};
T.getRemoved ::= fn(Std.Set other){
	var a = this.clone();
	return a.remove(other);
};
T.getSubstracted ::= fn(Std.Set other){
	var a = this.clone();
	return a.substract(other);
};
T.intersect ::= fn(Std.Set other){
	this.swap(getIntersection(other)); 
	return this;
};
T.merge ::= fn(Std.Set other){
	this.data.merge(other._accessData());
	return this;
};
T.remove ::= fn(value){
	this.data.unset(value);
	return this;
};
T.substract ::= fn(Std.Set other){
	foreach(other as var value)
		data.unset(value);
	return this;
};
T.toArray ::= fn(){
	var a = [];
	foreach(this.data as var value,var dummy)
		a+=value;
	return a;
};
T.swap ::= fn(Std.Set other){
	data.swap(other.data);
	return this;
};
T."+=" ::= T.add;
T."-=" ::= T.remove;
T."|=" ::= T.merge;
T."|" ::= T.getMerged;
T."&=" ::= T.intersect;
T."&" ::= T.getIntersection;

T."==" ::= fn(other){
	return (other---|>(this.getType())) ? (data==other._accessData()) : false;
};

Std._registerModule('Std/Set',T); // support loading with Std.requireModule and loadOnce.
return T;

// ------------------------------------------
