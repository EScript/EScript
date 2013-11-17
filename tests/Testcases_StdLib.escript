loadOnce("Std/basics.escript");
Std.addModuleSearchPath(".");

//Runtime.setLoggingLevel(Runtime.LOG_INFO);
// ----------------------------------------------------------
{
	var MultiProcedure = Std.require('Std/MultiProcedure');
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

	test("Std.MultiProcedure", Std.MultiProcedure == MultiProcedure  && ok);
}
// ----------------------------------------------------------
{
	var Set = Std.require('Std/Set');

	var ok = true;
	var s1 = new Set(4,5,1,3,4);

	var s2 = new Set(1,3,4,7);
	var s3 = s2.clone();
	s2+=5;
	s2-=7;

	var s4 = new Set("foo","blub");
	var s5 = new Set("foo","bar");

	var s6 = s4|s5;
	s4|=s5;

	ok &= 	s1==s2 && s1!=s3 && s1.count()==4 && s4==new Set("foo","blub","bar") && s4==s6 && s5!=s6 &&
			(s1 & new Set(3,4,9,"bla")) == new Set(3,4) &&
			s1.getSubstracted(new Set(3,4,9,"bla")) == new Set(1,5);


	var sum=0;
	foreach(s1 as var value)
		sum+=value;
	ok &= (sum==1+3+4+5);

	test("Std.Set",	Std.Set == Set  && ok );
}
// ----------------------------------------------------------
{
	var PriorityQueue = Std.require('Std/PriorityQueue');

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

	test("Std.PriorityQueue",
		Std.PriorityQueue == PriorityQueue  &&
		ok &&
		true );
}
// ----------------------------------------------------------
{
	var DataWrapper = Std.require('Std/DataWrapper');
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

	test("Std.DataWrapper",Std.DataWrapper == DataWrapper && ok);

}
// ----------------------------------------------------------

{
	var DataWrapperContainer = Std.require('Std/DataWrapperContainer');
	var DataWrapper = Std.require('Std/DataWrapper');
	var ok = true;

	var result = [];
	
	var container = new DataWrapperContainer({
		"a" : DataWrapper.createFromValue("a"),
		"b" : DataWrapper.createFromValue("b"),
	});
	container.onDataChanged += [result]=>fn(result, key,value){
		result += ""+key+":"+value;
	};
	container.addDataWrapper("c" , DataWrapper.createFromValue("c") ); // result += "c:c"

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

	test("Std.DataWrapperContainer",Std.DataWrapperContainer == DataWrapperContainer && ok);
}
// ----------------------------------------------------------
{
	var info = Std.require('Std/info');
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
	test("Std.info",Std.info == info && ok);
}
// ----------------------------------------------------------

{
	var ObjectSerialization = Std.require('Std/ObjectSerialization');
	var ok = true;
	
	var fun = fn(){return "foo";};
	var extObj = new ExtObject({$m1:1});
	extObj.selfRef := extObj;
	extObj.fun := fun;

	var mFun = new (Std.require('Std/MultiProcedure'));
	mFun += fn(arr){	arr+="foo"; };
	mFun += fn(arr){	arr+="bar"; };

	
	var m = {
		"number":1,
		"bool":true,
		"array": [1,2,3],
		"identifier": $foo,
		"function": fun,
		"void" : void,
		"extObject" : extObj,
		"boundParams" : [1,2] => fn(a,b){return a+b;},
		"reservedString" : "##REF:##",
		"multiProcedure" : mFun
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

	test("Std.ObjectSerialization",Std.ObjectSerialization == ObjectSerialization && ok);
}
// ----------------------------------------------------------
{
	var JSONDataStore = Std.require('Std/JSONDataStore');
	var ok = true;
	var value = [ true,1,void,"foo",[1,2], {"1":"bar"} ];
	var filename = "./test_JSONDataStore.json";
	var DataWrapper = Std.require('Std/DataWrapper');
	{
		var dataStore = new JSONDataStore(true);
	
		ok &= dataStore.init(filename,false);
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
	test("Std.JSONDataStore",Std.JSONDataStore == JSONDataStore && ok);
}
