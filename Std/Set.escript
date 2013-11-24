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
T._constructor ::= fn( values=void ){
	if(void!=values)
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
T.getIntersection ::= fn(collection){
	var other = collection.getType()==this.getType() ? 
					collection : 
					new (this.getType())(collection);
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
T.getMerged ::= fn(collection){
	var a = this.clone();
	return a.merge(collection);
};
T.getRemoved ::= fn(collection){
	var a = this.clone();
	return a.remove(collection);
};
T.getSubstracted ::= fn(collection){
	var a = this.clone();
	return a.substract(collection);
};
T.intersect ::= fn(collection){
	this.swap(getIntersection(collection)); 
	return this;
};
T.merge ::= fn(collection){
	if(collection.getType()==this.getType()){
		this.data.merge(collection._accessData());
	}else{
		foreach(collection as var value)
			this += value;
	}
	return this;
};
T.remove ::= fn(value){
	this.data.unset(value);
	return this;
};
T.substract ::= fn(collection){
	foreach(collection as var value)
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
	data.swap(other._accessData());
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

Std.onModule('Std/ObjectSerialization', [T] => fn(Set, ObjectSerialization){
	ObjectSerialization.registerType(Set,'Std.Set')
		.addDescriber(fn(ctxt,obj,Map d){	
			d['entries'] = ctxt.createDescription( obj.toArray() );
		})
		.addInitializer(fn(ctxt, obj,Map d){
			var entries = d['entries'];
			if(entries){
				foreach(entries as var d)
					obj += ctxt.createObject(d);
			}
		});
});

return T;
