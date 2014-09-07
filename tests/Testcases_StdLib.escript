// Testcases_StdLib.escript
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
addSearchPath(__DIR__ + "/..");
static module = load("Std/module.escript");
module.addSearchPath(".");

//Runtime.setLoggingLevel(Runtime.LOG_INFO);
// ----------------------------------------------------------
{
	var ok = true;
	var loader = module.createLoader( 'Foo/ExampleModule' );
	ok &= loader.getId() == 'Foo/ExampleModule';
	ok &= loader.get('./Module2') === void;
	loader._registerModule('Foo/Module2',42);
	ok &= loader.get('./Module2') == 42;
	ok &= loader.require('./Module2') == 42;
	ok &= loader('./Module2') == 42;
	ok &= module.get('Foo/Module2') == 42;
	loader._setModuleAlias('Foo17/Module2','./Module2');
	ok &= loader.get('Foo17/Module2') == 42;
	loader._unregisterModule('Foo/Module2');
	ok &= loader.get('Foo17/Module2') === void;
	var exceptionIfNotFound=false;
	try{
		loader('Foo17/Module2');
	}catch(e){
		exceptionIfNotFound = true;
	}
	ok &= exceptionIfNotFound;
	test("Std.module", ok );
}

{
	var ok = true;
	var generatorFn = module('Std/generatorFn');
	{
		var f = generatorFn(fn( max ){
			for(var i=0;i<max;++i){
				yield i+this;
			}
		});
		var v = (10->f)(5); // pass 10 as this object.
		while(var result = f())
			v += result;
		ok &= (v == 14+13+12+11+10);
	}
	{
		var f = generatorFn(fn(max){
			for(var i=0;i<max;++i)
				yield i;
			return 100;
		});
		var v = f(5);
		while(f.isActive())
			v += f();
		ok &= (v == 100+4+3+2+1);
		// start a second time
		v = f(6);
		while(f.isActive())
			v += f();
		ok &= (v == 100+5+4+3+2+1);

	}

	test("Std.generatorFn", ok );
}
{
	var MultiProcedure = module('Std/MultiProcedure');
	var ok = true;

	var p1 = new MultiProcedure;
	ok &= p1.empty();
	ok &= p1.count()==0;
	p1 += fn(result, v){	result += v;	};
	p1 += fn(result, v){	result += v*10;	return $REMOVE;	};
	var f1 = fn(result, v){	result += v*100;	};
	p1 += f1;
	ok &= !p1.empty();
	ok &= p1.count()==3;

	var result = [];

	p1(result,7);
	ok &= (result == [7,70,700]);
	// the second procedure has been removed
	result.clear();
	p1(result,8);
	ok &= (result == [8,800]);

	// remove f1
	var p2 = p1.clone();
	p1.filter( [f1]=>fn(f1,fun){	return f1!=fun;	});
	result.clear();
	p1(result,9);
	ok &= (result == [9]);

	result.clear();
	p2(result,1);
	ok &= (result == [1,100]);

	// calling object
	p1.clear();
	p1 += fn(result){	result+=this;	};
	result.clear();
	("Foo"->p1)(result);
	ok &= (result == ["Foo"]);
	
	
	{// revocing
		p1.clear();
		var f = fn(result){	result+="a";	};
		p1 += f;
		var r = p1.addRevocably( f );
		
		
		var result = [];
		p1( result );
		ok &= result == [ "a","a" ];
		result.clear();
		
		r();
		p1( result );
		ok &= result == [ "a" ];
	}

	test("Std.MultiProcedure", ok);
}
// ----------------------------------------------------------
{
	var Set = module('Std/Set');

	var ok = true;
	var s1 = new Set([4,5,1,3,4]);

	var s2 = new Set([1,3,4,7]);
	var s3 = s2.clone();
	s2+=5;
	s2-=7;

	var s4 = new Set(["foo","blub"]);
	var s5 = new Set(["foo","bar"]);

	var s6 = s4|s5;
	s4|=s5;

	ok &= 	s1==s2 && s1!=s3 && s1.count()==4 && s4==new Set(["foo","blub","bar"]) && s4==s6 && s5!=s6 &&
			(s1 & new Set([3,4,9,"bla"])) == new Set([3,4]) &&
			s1.getSubstracted([3,4,9,"bla"]) == new Set([1,5]);


	var sum=0;
	foreach(s1 as var value)
		sum+=value;
	ok &= (sum==1+3+4+5);

	var s7 = new Set(["1"]);
	var s8 = new Set(["2"]);
	s7.swap(s8);
	ok &= (s7.toArray() == ["2"] && s8.toArray() == ["1"]);

	test("Std.Set",	ok );
}
// ----------------------------------------------------------
{
	var PriorityQueue = module('Std/PriorityQueue');

	var ok = true;
	var q = new PriorityQueue;
	ok &= q.empty();

	{
		var r = new Math.RandomNumberGenerator;
		for(var i=0;i<50;++i)
			q += r.equilikely(0,30);

		ok &= q.count() == 50;
		var n = 0;
		for(var i=0;i<10;++i){
			var n2 = q.extract();
			ok &= n2>=n;
			n = n2;
		}
		ok &= q.count() == 40;
		for(var i=0;i<10;++i)
			q += r.equilikely(0,30);
		ok &= q.count() == 50;

		n = 0;
		while(!q.empty()){
			var n2 = q.extract();
			ok &= n2>=n;
			n = n2;
		}
	}
	{
		var arr = [4,7,1,3];
		foreach(arr as var n2)
			q += n2;
		var q2 = q.clone();
		arr.sort();
		while(!q.empty()){
			var n2 = q.extract();
			ok &= n2 == q2.extract() && n2 == arr.popFront();
		}
	}
	q += 2;
	q.clear();
	ok &= q.empty();

	test("Std.PriorityQueue", ok );
}
// ----------------------------------------------------------
{
	var DataWrapper = module('Std/DataWrapper');
	var ok = true;
	{

		var sideLength = DataWrapper.createFromValue( 10 );
		var area = DataWrapper.createFromFunctions(
						[sideLength] => fn(sideLength){	return sideLength()*sideLength(); },
						[sideLength] => fn(sideLength, data){	sideLength.set(data.sqrt());});

		// propagate changes of the sideLength to the area. sideLength and data are now directly connected.
		sideLength.onDataChanged += area->fn(data){refresh();};

		// --------

		ok &= (area() == 100);
		area(81);
		ok &= (sideLength() == 9);
		sideLength(5);
		ok &= (area() == 25);

	}
	{
		var obj = new ExtObject({$a:5});
		var a = DataWrapper.createFromAttribute(obj,$a);
		ok &= a() == obj.a;
		obj.a++;
		ok &= a() == obj.a;
		a(42);
		ok &= (obj.a==42 && a() == obj.a);
	}
	{
		var obj = {"a":5};
		var a = DataWrapper.createFromEntry(obj,$a);
		ok &= a() == obj["a"];
		obj["a"]++;
		ok &= a() == obj["a"];
		a(42);
		ok &= (obj["a"]=42 && a() == obj["a"]);
	}

	test("Std.DataWrapper",ok);

}
// ----------------------------------------------------------

{
	var DataWrapperContainer = module('Std/DataWrapperContainer');
	var DataWrapper = module('Std/DataWrapper');
	var ok = true;

	var result = [];

	var container = new DataWrapperContainer({
		"a" : new DataWrapper("a"),
		"b" : new DataWrapper("b"),
	});
	container.onDataChanged += [result]=>fn(result, key,value){
		result += ""+key+":"+value;
	};
	container.addDataWrapper("c" , new DataWrapper("c") ); // result += "c:c"

	ok &= (container.count() == 3);

	container["a"] = container["a"]+"2";
	container.getDataWrapper("b")("b2");
	var c = container.getDataWrapper("c");
	container.unset("c");

	ok &= (container.count() == 2);
	ok &= container.containsKey("a");
	ok &= !container.containsKey("c");

	c("c3"); // should not influence the result

	container.assign({ "a":"a3" });

	ok &= (result == [ "c:c", "a:a2", "b:b2", "a:a3" ]);

	// iterator
	var result2 = [];
	foreach(container as var key,var value){
		result2 += key + ":" +value;
	}
	ok &= (result2 == ["a:a3","b:b2"]);

	// clear
	ok &= !container.empty();
	container.clear();
	ok &= container.empty();
	ok &= !container.containsKey("a");
	ok &= container.count() == 0;

	test("Std.DataWrapperContainer",ok);
}
// ----------------------------------------------------------
{
	var info = module('Std/info');
	var ok = true;
	var T = new Type;
	T.foo := 4;
	info += [T,fn(obj,result){
		result += "foo:"+obj.foo;
	}];
	var t = new T;
	t.foo = 17;
	ok &= info.get(t).contains("foo:17");
	//info(t);
	test("Std.info",ok);
}
// ----------------------------------------------------------

{
	var ObjectSerialization = module('Std/ObjectSerialization');
	var ok = true;

	var fun = fn(){return "foo";};
	var extObj = new ExtObject({$m1:1});
	extObj.selfRef := extObj;
	extObj.fun := fun;

	var mFun = new (module('Std/MultiProcedure'));
	mFun += fn(arr){	arr+="foo"; };
	mFun += fn(arr){	arr+="bar"; };


	var set = new (module('Std/Set'));
	set += 1;
	set += 2;
	set += 3;

	var m = {
		"number":1,
		"bool":true,
		"array": [1,2,3],
		"identifier": $foo,
		"function": fun,
		"void" : void,
		"extObject" : extObj,
		"boundParams" : [1,2] => fn(a,b){return a+b;},
		"boundObject" : 42-> fn(){return this;},
		"reservedString" : "##REF:##",
		"multiProcedure" : mFun,
		"set" : set
	};
//	outln(ObjectSerialization.serialize(m));
	var m2 = ObjectSerialization.create(ObjectSerialization.serialize(m));
	ok &= (m["number"] === m2["number"]);
	ok &= (m["bool"] === m2["bool"]);
	ok &= ([1,2,3] == m2["array"]);
	ok &= ($foo == m2["identifier"]);
	ok &= (m2["function"]() == "foo");
	ok &= (void === m2["void"]);
	ok &= (m2["reservedString"] === m["reservedString"]);
	ok &= (m2["boundParams"]() === 3);
	ok &= (m2["boundObject"]() === 42);
	ok &= (m2["set"] == set);
	var extObj2 = m2["extObject"];
	ok &= (extObj2.m1 === 1);
	ok &= (extObj2.selfRef === extObj2);
	ok &= (extObj2.fun() === "foo");
	ok &= (extObj2.fun === m2["function"]);

	var arr = [];
	m2["multiProcedure"](arr);
	ok &= (arr == ["foo","bar"]);


	{ // serializing functions which use static variables should produce a warning
		var exception = false;
		Runtime.setTreatWarningsAsError(true);
		try{
			static s1 = "foo";
			ObjectSerialization.serialize(fn(){return s1;});
		}catch(e){
			exception = true;
		}
		ok &= exception;
	}
	{// map with reserved keys
		var map = {"##TYPE##":"foo"};
		ok &= ObjectSerialization.create(ObjectSerialization.serialize(map)) == map;
	}
//	print_r(ObjectSerialization.serialize("##TYPE##"));

	{// custom type using specialized registry
		static MyType = new Type;
		MyType.m1 := void;
		MyType._constructor ::= fn(s){	this.m1 = s;	};

		var registry = new ObjectSerialization.TypeRegistry;

		registry.registerType(MyType,"MyType")
			.enableIdentityTracking()
			.addDescriber(fn(ctxt,MyType obj,Map d){	d['m1'] = ctxt.createDescription(obj.m1);	})
			.setFactory( fn(ctxt,Type actualType,Map d){		return new actualType(d['m1']);	});

		var ctxt = new ObjectSerialization.Context(registry);
		var obj = new MyType( [1,2,3] );
//		print_r(ctxt.serialize(obj));
		var obj2 = ctxt.createObject(ctxt.serialize(obj));
//		ok &=
	}

	test("Std.ObjectSerialization",ok);
}
// ----------------------------------------------------------
{
	var JSONDataStore = module('Std/JSONDataStore');
	var ok = true;
	var value = [ true,1,void,"foo",[1,2], {"1":"bar"} ];
	var filename = "./test_JSONDataStore.json";
	var DataWrapper = module('Std/DataWrapper');
	{
		var dataStore = new JSONDataStore(true);

		dataStore.init(filename,false);
		ok &= (dataStore.getFilename() == filename);

		dataStore.clear();
		dataStore["foo.bar.dum"] = value;
		dataStore["foo.bar.di"] = "flup";
		ok &= (dataStore.get("c1.value1",42) == 42);
		var dw = DataWrapper.createFromEntry(dataStore,"c1.value2",5);
		ok &= (dataStore.get("c1.value2") == 5);
		dw(43);
		ok &= (dataStore.get("c1.value2") == 43);
	}
	{
		var dataStore = new JSONDataStore(false);
		ok &= dataStore.init(filename);
		ok &= (dataStore["foo.bar.dum"] == value);
		ok &= (dataStore["foo.bar.di"] == "flup");
		ok &= (dataStore.get("c1.value1") == 42);
		var dw = DataWrapper.createFromEntry(dataStore,"c1.value2");
		ok &= (dw() == 43);
		dataStore.unset("c1.value1");
		ok &= !dataStore.get("c1.value1");
//		outln(IO.loadTextFile(filename));
		dataStore.clear();
		dataStore.save();
	}
	test("Std.JSONDataStore",ok);
}
{	// traits
	var ok = true;

	var Traits = module('Std/Traits/basics');
	var T = new Type;
	T.value := 0;
	T._constructor ::= fn(v){	this.value = v; };

	var PrintableNameTrait = module('Std/Traits/PrintableNameTrait');
	Traits.addTrait( T, PrintableNameTrait, "MyType" );

	var DefaultComparisonOperatorsTrait = module('Std/Traits/DefaultComparisonOperatorsTrait');
	Traits.addTrait( T, DefaultComparisonOperatorsTrait, fn(other){	return this.value<other.value;	} );

	var t = new T(1);
	ok &= t.toString().contains("MyType");
	ok &= Traits.queryTrait( t, PrintableNameTrait );
	ok &= Traits.queryTrait( t, "Std.Traits.PrintableNameTrait" );
	ok &= Traits.queryTrait( t, PrintableNameTrait ) == PrintableNameTrait;
	ok &= Traits.requireTrait( t, PrintableNameTrait ) == PrintableNameTrait;
	ok &= !Traits.queryLocalTrait( t, PrintableNameTrait );
	ok &= Traits.queryLocalTrait( T, PrintableNameTrait );

	static CallableTrait = module('Std/Traits/CallableTrait');
	ok &= Traits.queryTrait(fn(){},CallableTrait);
	ok &= Traits.queryTrait(1->fn(){},CallableTrait);
	ok &= Traits.queryTrait(module('Std/MultiProcedure'),CallableTrait);
	ok &= !Traits.queryTrait(1,CallableTrait);

	// DefaultComparisonOperatorsTrait
	var t2 = new T(2);
	ok &= t2>t && t<t2 && t==t && t2>=t && t<=t2 && t<=t && t2>=t2;

	{ // removable traits
		var removableTrait = new (module('Std/Traits/GenericTrait'))("Tests/RemovableTrait");
		removableTrait.attributes.member1 := 42;
		ok &= !removableTrait.getRemovalAllowed();
		removableTrait.allowRemoval();
		ok &= removableTrait.getRemovalAllowed();
		removableTrait.onRemove := fn(obj){
			obj.member1 = void;
		};

		var object = new ExtObject;
		ok &= !Traits.queryTrait(object,removableTrait);
		Traits.addTrait(object,removableTrait);
		ok &= Traits.queryTrait(object,removableTrait);
		ok &= (object.member1 == 42);
		Traits.removeTrait(object,removableTrait);
		ok &= !Traits.queryTrait(object,removableTrait);
		ok &= !object.member1;
	}
	{// constraints
		var f = fn(CallableTrait callback){};
		f( out );
		f( new (module('Std/MultiProcedure')) );
		var exceptionCount = 0;
		try{ f(1); }catch(){++exceptionCount;};
		try{ f(CallableTrait); }catch(){++exceptionCount;};
		
		static Trait = module('Std/Traits/Trait');
		var f2 = fn(Trait aTrait){};
		f2( CallableTrait );
		try{ f2(1); }catch(){++exceptionCount;};
		try{ f2(out); }catch(){++exceptionCount;};
		
		ok &= exceptionCount==4;
	}
	{	// assureTrait
		var t = new (module('Std/Traits/GenericTrait'));
		var initCounter = [0];
		t.onInit += [initCounter] => fn( initCounter,obj ){ initCounter[0]++; };
		var obj = new ExtObject;
		ok &= !Traits.queryTrait(obj,t);
		Traits.assureTrait(obj,t);
		ok &= Traits.queryTrait(obj,t);
		Traits.assureTrait(obj,t);
		ok &= initCounter[0]==1;
	}
	
	
	test("Std.Traits", ok);
}
// ----------------------------------------------------------
{	// addRevocable
	var ok = true;
	var MultiProcedure = module('Std/MultiProcedure');
	var Set = module('Std/Set');
	var addRevocably = module('Std/addRevocably');
	
	var arr =  new Set([1,2,3]);
	
	var cleanup = new MultiProcedure;
	cleanup += addRevocably( arr, 4 );
	
	ok &= arr==new Set([1,2,3,4]);
	
	arr += 5;
	cleanup(); 
	ok &= arr==new Set([1,2,3,5]);
	ok &= cleanup.empty();
	
	test("Std.addRevocably",ok);
}
// ----------------------------------------------------------
{	// enum
	var ok = true;
	var enum = module('Std/enum');

	static myEnum = enum($Foo,$Bar,$Blub);
	
	ok &= myEnum.isSet( $Foo ) && myEnum.isSet( $Bar ) && myEnum.isSet( $Blub );
	
	var f = fn(myEnum state){
		return state.value;
	};
	ok &= 0 == f(myEnum.Foo) && 1 == f(myEnum.Bar) && 2 == f(myEnum.Blub);

	var exceptionCount = 0;
	try{ f($Foo); }catch(){++exceptionCount;};
	ok &= 1 == exceptionCount;
	
	test("Std.enum",ok);
}
// ----------------------------------------------------------
{	// declareNamespace
	var ok = true;
	var declareNamespace = module('Std/declareNamespace');

	ok &= declareNamespace( $Foo,$A,$B ) == Foo.A.B;
	Foo.v := 1;
	Foo.A.v := 2;
	Foo.A.B.v := 3;

	declareNamespace( $Foo,$A,$B ); // should not overwrite anything
	++Foo.v;
	++Foo.A.v;
	++Foo.A.B.v;

	ok &= Foo.v == 2 && Foo.A.v == 3 && Foo.A.B.v == 4;
	
	test("Std.declareNamespace",ok);
}
	
// ----------------------------------------------------------
{
	var ok = true;
	var Std = module('Std/StdNamespace');
	ok &= Std.addRevocably === module('Std/addRevocably');
	ok &= Std.DataWrapper === module('Std/DataWrapper');
	ok &= Std.DataWrapperContainer === module('Std/DataWrapperContainer');
	ok &= Std.generatorFn === module('Std/generatorFn');
	ok &= Std.info === module('Std/info');
	ok &= Std.JSONDataStore === module('Std/JSONDataStore');
	ok &= Std.MultiProcedure === module('Std/MultiProcedure');
	ok &= Std.ObjectSerialization === module('Std/ObjectSerialization');
	ok &= Std.PriorityQueue === module('Std/PriorityQueue');
	ok &= Std.Set === module('Std/Set');
	ok &= Std.Traits === module('Std/Traits/basics');
	ok &= Std.Traits.CallableTrait === module('Std/Traits/CallableTrait');
	ok &= Std.Traits.DefaultComparisonOperatorsTrait === module('Std/Traits/DefaultComparisonOperatorsTrait');
	ok &= Std.Traits.GenericTrait === module('Std/Traits/GenericTrait');
	ok &= Std.Traits.PrintableNameTrait === module('Std/Traits/PrintableNameTrait');
	ok &= Std.Traits.Trait === module('Std/Traits/Trait');
	
	test("Std.StdNamespace",ok);
}
