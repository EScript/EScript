return fn(){
// -------------------
var OK="\t ok\n";
var FAILED="\t failed\n";

//---
{	// Bool operations && || !
	var a = 0;
	a>0 || (a = a+1)>0 && (a = a+3)>0 || (a = 0)>0;
	a==7 && (a = 10)>0;
	a==4 && (a = 5)>0;

	test("Bool operations:", true
		&& a==5 && !false
		&& !(2!=1+1) && !(!(2==2)) );
}
//---
{
	out("Assignment Operations:" );
	var a = 0;
	a+=10; // 0+10 = 10
	a*=5;  // 10*5 = 50
	a/=2;  // 50/2 = 25
	a%=17; // 25 % 17 = 8
	var c;
	var b = c = 3;

	if( a==8 && b==3 && c==3){out (OK);}else { errors+=1; out(FAILED); }
}
//---
{
	var b = false;
	fn(value){value|=true;}(b); // test call by value

	var b2 = true;
	var b2a = (b2&=true);
	var b2b = (b2&=false);

	test("Bool:", true
		&& !b
		&& true & true && !(true&false)
		&& false | true  && true | true && true | false && ! (false |false)
		&& true > false && false < true && !(true < false) && !(false > true)
		&& !(false > false) && !(true > true) && !(true < true) && !(false < false)
		&& b2 == false && b2a && !b2b
		&& (true ^ true) == false && (false ^ false) == false && (true ^ false) == true
		&& !false
		, Bool);
}

// ---
{	// Number
	var n = 1;
	fn(value){value++;}(n); // test call by value
	var i=(9).sqrt();

	var n2 = 7;
	n2 &= 12; // 7 & 12 == 4
	var n2b = (n2 -= 10); // 4-10 == -6

	var n3 = +17;
	n3 = +n3; // operator Number.+_pre
	var n3a = --n3;
	var n3b = n3--;

	var n4 = 7;
	var n4b = (n4 |= 9); // 7 & 9 == 15

	var n5 = 7;
	var n5b = (n5 ^= 9); // 7 xor 9 == 14 cos

	var n6 = 17;
	var n6a = ++n6;
	var n6b = n6++;

	test("Number:", true
			&& ((2|3|255&1040) ^ 33) == 50
			&& ((1.0+2)*3/(20-(1+(1)))+0.5+-1+1) == 1
			&&	n==1 && i==3
			&& (0x01+255).toHex()=="0x100" && "-1.7".toNumber()==-1.7 && 1.getType()==Number
			&& (180).degToRad()== Math.PI && (Math.PI.radToDeg()-180).abs() < 0.001
			&& 1.sign()==1 && -2.3.sign()==-1
			&& Math.PI.format(4,false,10,"-")=="----3.1416" && Math.PI.format(5,true).beginsWith("3.14159e+00" )
			&& 1.clamp(2,3)==2 && 17.clamp(-2,20)==17 && 9.clamp(1,1.6)==1.6
			&& (180).degToRad().radToDeg().matches(180) && !(179.9999).degToRad().radToDeg().matches(180)
			&& (0.1+0.1+0.1) ~= 0.3 && !(0.9999999 ~= 1.0)
			&& (3.7 % 0.7) ~= 0.2 && (-2 % 3) == -2
			&& (123.456).round(0.1) ~= 123.5
			&& (123.456).round(10) ~= 120
			&& (-123.456).round(10) ~= -120
			&& new Number("1") == 1
			&& n2 == -6 && n2b == -6
			&& n3 == 15 && n3a == 16 && n3b ==16
			&& 1<=1 && 0<=1 && ! (1<=0)
			&& 1>=1 && 1>=0 && ! (0>=1)
			&& n4 == 15 && n4b ==15
			&& n5 == 14 && n5b == 14
			&& n6 == 19 && n6a == 18 && n6b ==18
			&& (0).sin() ~= 0 && (90).degToRad().sin() ~=1 && (0.5).asin().radToDeg()~=30
			&& (0).cos() ~= 1 && (180).degToRad().cos() ~=-1 && (-0.5).acos().radToDeg()~=120
			&& (45).degToRad().tan() ~= 1 && (-30).degToRad().tan() ~= -(3.pow(-0.5)) && (-1).atan().radToDeg() ~= -45
			&& 1.0.ceil() == 1.0 && 1.5.ceil() == 2.0 && (-1.5).ceil() == -1
			&& 1.0.floor() == 1.0 && 1.5.floor() == 1.0 && (-1.5).floor() == -2
			&& 100.log()==2 && (128).log(2)==7 && 782.log(78) ~= 782.ln() / 78.ln()
			&& (-327645342.123342).toIntStr() === "-327645342"
			, Number);
}

//---
{
	var mystring="bl\"#"+2;
	fn(value){value+="should do nothing";}(mystring); // test call by value

	var s="foobar";
	var spacy = "\t   bla  \n\r  ";

	var s2 = "ab";
	var s2b = (s2*=3);

	test("String:", true
		&& new String(2) === "2"
		&& "foo".length()==3 && "\0\0".length()==2
		&& "bar"[1] == "a" 
		&& void == "bar"[3]
		&& mystring*3 == "bl\"#2bl\"#2bl\"#2" && !s.endsWith("\0")
		&& s.endsWith("bar")&&!s.endsWith("b")&&s.beginsWith("foo")&& s.beginsWith(s)&&  !s.beginsWith(s+s)
		&& s.contains("ob") && !s.contains("oc") && "f".getType()==String
		&& "a,b,c".split(",",2) == ["a","b,c"] && "/".split("/") == ["",""]
		&& "bla".fillUp(10,'.') == "bla......." && "x".fillUp(100,"") == "x" && "x".fillUp(4,"12") == "x1212"
		&& spacy.lTrim()== "bla  \n\r  " && spacy.rTrim()=="\t   bla" && spacy.trim()=="bla" && "".trim().empty()
		&& s2=="ababab" && s2b == "ababab"
		&& "abc" <= "bcd" && "A" < "a" && !("bcd" <= "abc") && !("a" < "A")
		&& "bcd" >= "abc" && "a" > "A" && !("abc" >= "bcd") && !("A" > "a")
		&& "blub".find("lu") == 1 && !("blob".find("lu"))
		&& ("foo"*3).replace("oo","x") == "fxfoofoo"
		&& ("foo"*3).replaceAll( {"oo":"ar ","f":"b"},3 ) == "bar boofoo"
		&& "fooooooxx".rFind("oo") == 5 && "fooooooxxoo".rFind("oo",8) == 5  && !("fooooxx".rFind("x",4))
		&& "FooBar".substr(1) == "ooBar" && "FooBar".substr(-4) == "oBar" && "FooBar".substr(1,-3) == "oo"  && "FooBar".substr(-3,1) == "B"
		&& " fOObaR12.3".toLower() == " foobar12.3" &&" fOObaR12.3".toUpper() == " FOOBAR12.3"

		// unicode 
		&& "blä".length() == 3
		&& "föße"[2] == "ß"
		&& "fääääääääääöße".find("ß") == 12 && "fääääääääääöße".find("ü") == false
		&& "fööööööxxöö".rFind("öö",8) == 5 
		&& "bla".fillUp(10,'ä') == "blaäääääää" 
		,String);
}

//---
{
	out("Void:\t\t");
	var v = void;
	if( void==v && v!=false && false==v && v!==false && v===void && void != 0 && 0==void && !((1)===void) && !(void===0)
	// strange, but ok. (0).==(void.toNumber())
	&& 0 == void)
		out(OK); else { errors+=1; out(FAILED); }
}

//---
{
	out("While: \t\t");
	var a = 0;
	while(a<8){a = a+1;}
	while(a>5){a--;}
	var b = 0;
	var c = 0;
	while( true){
		if(++b%2 != 0) {{continue; }}
		if(++c==5) break;
	}
	var d = 0;
	do{d++;}while(d<0);
	if(a == 5 && b == 10 && c==5 && d==1){out (OK);}else { errors+=1; out(FAILED); }
}
//---
{
	out("For: \t\t");
	var a = 0;
	var i = 0;
	for(i = 5;i<10;){
		a+=i;
		//out(a,"-",i,"\n");
		i++;
	}
	for(i = 10;i>5;i--){
		if( (i%2) >0) continue;
		if(i<7) break;
		a+=i;
		// out(a,"-",i,"\n");
	}
	// a==53
	var j = 0;
	for(var j = 0;j<5;++j)
		a+=j;
	// a==53 + 0+1+2+3+4==63

	// this $a is a different local variable and not touched.
	for(var a = 0;a<5;++a){
		var a="foo";
	}

	// out(a);
	if(a == 63 && j==0 ){out (OK);}else { errors+=1; out(FAILED); }
}
//---
{
	out("Blocks:\t\t");
	var v1 = 1;
	{
		var v1 = 2;
		{
			v1 = v1+1;
		}
	}
	{false?v1--:true?v1++:v1--;}
	if(v1==2 && ({ var a = 1; a+2;}) == 3){out (OK);}else { errors+=1; out(FAILED); }
}

//---
{	// UserFunctions:
	//global plusRec;
	var a = 10;
	GLOBALS.plusRec := fn (a,b){
		if(a<=0) return b;
		return plusRec(a-1,b+1);
	};
	var plusRec2= fn(a,b){
		if(a<=0) return b;
		return thisFn(a-1,b+1);
	};

	var minusOne = fn(a){
//		out(getRuntime().getStackInfo());
		return a-1;
	};

	var increase = fn(a,b = 1){
		return a+b;
	};
	var repeat = fn(Number n,String s="-"){
		return (s*n);
	};
	var typeException = false;
	try{
		repeat(true);
	}catch(e){
		typeException = true;
	}

	// multiParam
	var mulSum = fn(factor,Number values...){
		var a = 0;
		foreach(values as var v) a+=v;
		return factor * a;
	};
	var typeException2 = false;
	try{
		out(mulSum(2,"foo"));
	}catch(e){
		typeException2 = true;
	}

	// multi possibilities
	var f = fn( [Number,"bla"] p1,[Number] p2 = 7,[1,2,3] p3... = 3){

	};

	f("bla",3);
	var typeException3 = false;
	try{
		f("bla",3,4);
	}catch(e){
		typeException3 = true;
	}

	// skipped parameter
	var f2 = fn(a = 1,b = 2,c = 3){
		return a+b+c;
	};
//	var x = new Array(1,,,4);
//	print_r(x);

	// function members
	var f3 = fn(){
		if(!thisFn.isSet($staticVar))
			thisFn.staticVar:=0;
		thisFn.staticVar+=1;
		return thisFn.staticVar;
	};

	// cloning USerFunction Objects
	var f4 = fn(a = 1){ thisFn.m+=a; return thisFn.m; };
	f4.m:=0;
	f4(7);
	var f4b = f4.clone();
	f4(10); // 7+10 == 17
	f4b(); // 7+1 = 8
	f4b(19); // 8+19 ==27

	// Binding parameters with function wrappers
	UserFunction.bindLastParams ::= fn(params...){
		var myWrapper = thisFn.wrapperFn.clone();
		myWrapper.wrappedFun := this;
		myWrapper.boundParams := params;
		return myWrapper;
	};
	UserFunction.bindLastParams.wrapperFn := fn(params...){
		// _getCurrentCaller() is used instead of "this", as "this" may not be defined if this function
		// is called without a caller. This then results in a warning due to an undefined variable "this".
		return (Runtime._getCurrentCaller()->thisFn.wrappedFun)(params...,thisFn.boundParams...);
	};
	test("UserFunction:", true
		&& plusRec(a,7)==17 && plusRec2(a,7)==17 && minusOne(a)==9 && fn(a){return a*a;}(2)==4
		&& increase(3)==4 && increase(3,2)==5 && typeException==true && repeat(3,".")=="..."
		&& mulSum(2,1,2,3)==12 && typeException2 && typeException3
		&& f2(,,10)==13
		&& f3()==1 && f3()==2 // a static counter is increased each time f3 is called
		&& f3 ---|> UserFunction
		&& fn(){/*bla*/}.getCode() == "fn(){/*bla*/}"
		&& f4(0) == 17 && f4b(0) ==27
		&& (1->fn(a){return this+a; }.bindLastParams(27)) () == 28 // 1+27

		&& [1,2,3].map( fn(key,value,sumA,sumB){return value+sumA+sumB;}.bindLastParams(90,10) )  == [101,102,103]
		&& plusRec.getFilename() == __FILE__
		&& fn(a){}.getMinParamCount() == 1 && fn(a...){}.getMinParamCount() == 0 && fn(a,b,c = 2){}.getMinParamCount() == 2 // user function info
		&& fn(a){}.getMaxParamCount() == 1 && !fn(a...){}.getMaxParamCount() && fn(a,b,c = 2){}.getMaxParamCount() == 3 // user function info
		&& mulSum.getMultiParam() == 1 && !minusOne.getMultiParam() && minusOne.getParamCount() == 1 // user function info
		&& fn(...,a){return a;}(1,2,3) == 3 // ignored parameter
		&& fn(a,b...,c){return b;}(1,2,3,4) == [2,3] // multi parameter
		&& fn(a,b...,c){return b;}(1,4) == [] // multi parameter
		&& ({var T=new Type; T._constructor::=fn(p...){this.m:=p;}; new T(1,2,3);}).m == [1,2,3] // multi parameter in constructor call
		&& [0,1,[2,3,[4,5]...,[]...]...,6] == [0,1,2,3,4,5,6] // parameter expansion
		,UserFunction
	 );
	 f3.staticVar = 0; // reset staticVar for next testing loop.
	 UserFunction.bindLastParams = void; // need to remove the UserFunction extension for memory debug mode
}
//---
{	// User defined function (experimental!!!)

	// simple example
	var userDefinedFunction = new ExtObject( {
		$m1 : 17,
		$_call : fn(obj,params...){
			return m1+params[0];
		}
	});

	// more complex example: Bind parameter by user defined function
	UserFunction.bindLastParams2 ::= fn(params...){
		return new thisFn.Wrapper( this, params );
	};
	{	// (internals)
		UserFunction.bindLastParams2.Wrapper := new Type;
		var Wrapper = UserFunction.bindLastParams2.Wrapper;
		Wrapper.additionalParamValues := void;
		Wrapper.fun := void;

		//! (ctor)
		Wrapper._constructor ::= fn(_fun,_additionalParamValues){
			this.fun = _fun;
			this.additionalParamValues = _additionalParamValues;
		};
		Wrapper._call ::= fn(obj,params...){
//			params.append(additionalParamValues);
//			return Runtime._callFunction(fun,obj,params);
			return (obj->fun)(params...,additionalParamValues...);
		};
	}


	test("User def function(EXP!):", true
		&& userDefinedFunction(10) == 27 // 17+10
		&& (1->fn(a){return this+a; }.bindLastParams2(27)) () == 28 // 1+27
		&& [1,2,3].map( fn(key,value,sumA,sumB){return value+sumA+sumB;}.bindLastParams2(90,10) )  == [101,102,103]
	);
	UserFunction.bindLastParams2 = void; // need to remove the UserFunction extension for memory debug mode
}
//---
{	// Function
	test("Function:", true
		&& print_r ---|> Function
		&& print_r.getMinParamCount()==0 && print_r.getMaxParamCount() == false && print_r.getOriginalName() == $print_r
		&& Number."+".getMinParamCount() == 1 && Number."+".getMaxParamCount() == 1
		, Function
	);

}
//---
{	// Array
	var a = new Array(17,27,1000);
	a.pushBack("foo","bar");
	a.pushBack("ding");
	a.removeIndex(2); // remove 1000

	var accum="";

	a._set(2,"Hoobel");

	a[0]=a[0]+1;

	var i = a.getIterator();
	while(!i.end()){
		accum+=i.value();
		i.next();
	}


	var keyOfHoobel = fn(a){
		var key;
		var value;
		foreach(a as key,value){
			if(value==18) continue;
			// out("\n",key,":",value,"\n");
			if(value=="Hoobel") return key;
		}

	};
	i.reset();
	accum+=i.value();

	accum+=a._get(a.count()-2);
	a.pushBack(["dum","di"]);

	a[a.count()-1][100]="du";
	accum+= a[ a.count()-1 ][1];  // + di
	accum+= a[ a.count()-1 ][100];  // + du


	//out(accum);
	// accum=="1827Hoobelbarding18bardidu"

	var b = new Array(1,2);	// b: 1,2
	var c = b.clone();		// c: 1,2
	b.pushBack("foo");		// b: 1,2,foo
	c.append([3,4]);		// c: 1,2,3,4
	b.swap(c);				// b: 1,2,3,4 c: 1,2,foo


	var e=[3,23,7,3,100,1,35];

	var q = new ExtObject; // used for sorting by the distance from 59
	q.c:=59;
	var a2 = [1,2,3];
	a2.set(0,"foo");
	var a2a = a2.popBack();

	test("Array:", true
			&& accum=="1827Hoobelbarding18bardidu" && a ---|> Array && ! (1 ---|> Array)
			&& (new Array(1,'a','b')).implode(',')=='1,a,b'
			&& b.implode()=='1234' && c.implode()=='12foo'
			&& keyOfHoobel(a)==2 && [3,23,7,3,100,1,35].rSort()==[100,35,23,7,3,3,"1"]
			&& [3,23,7,3,100,1,35].sort( q->fn(a,b) {return (a-c).abs()<(b-c).abs(); }) == [35,23,100,7,3,3,1]
			&& ["x","bla",2,"dum"].sort()==["bla","dum","x",2]
			&& [1,2,3].map(fn(k,v){return k*v+1;})==[1,3,7]
			&& [1,2,3].map(fn(k,v,x){return x+k+":"+v;},"#")==["#0:1","#1:2","#2:3"]
			&& a.indexOf("ding")==4 && a.indexOf("ding",4)==4 && !a.indexOf("ding",5)
			&& !a.indexOf("dadong")
			&& [].pushFront(1).pushFront(2).pushFront(3).pushFront(4)==[4,3,2,1]
			&& (var x=[1,2,3]).popFront()==1 && x==[2,3]
			&& [1,2,3,4,5].reverse()==[5,4,3,2,1] && [1,2,3,4].reverse()==[4,3,2,1]
			&& [1,2,3,4,5,6,7,8,9].filter(fn(a){return (a%2)==0;})==[2,4,6,8]
			&& [1,2,3,4,5,6,7,8,9].filter(fn(a,b,c){return (a%b)==c;},3,0)==[3,6,9]
			&& e.max()==100	&& [4,2,-3,7].min()==-3
			&& e.contains(7)
			&& !e.contains(4)
			&& e.get(7)==void
			&& e.get(7,"notFound")=="notFound"
			&& e.get(0,"notFound")==3
			&& e.findValue(23)==1
			&& !e.findValue(4)
			&& e.front() == 3
			&& e.back() == 35
			&& ! ([].back())
			&& ! ([].front())
			&& e.reduce(fn(sum,key,value){return sum+value+key;},0) == (172+0+1+2+3+4+5+6)
			&& e.reduce(fn(accum,key,value,delimiter){return (accum ? accum+delimiter : "")+key+":"+value; },false,",") == "0:3,1:23,2:7,3:3,4:100,5:1,6:35"
			&& [1,2].clear().empty()
			&& [1,2,2,2,3,2,4].removeValue(2) == [1,3,4]
			&& [1,2,2,2,3,2,4].removeValue(2,3) == [1,3,2,4] // three times
			&& [1,2,2,2,3,2,4].removeValue(2,3,2) == [1,2,3,4] // three times, beginning from position 2
			&& ([1]+=2) == [1,2]
			&& a2 == ["foo",2]
			&& a2a == 3
			&& a2.size() == 2 // deprecated
			&& [0,1,2,3].splice(1,2) == [0,3]
			&& [0,1].splice(1,0,["a","b"]) == [0,"a","b",1]
			&& [].splice(100,1,["a"]) == ["a"]
			&& [ 'a','b','c','d' ].slice(1,2) == ['b','c'] // starting from #1, with length 2
			&& [ 'a','b','c','d' ].slice(0,-2) == ['a','b'] // starting from #0, excluding the last 2 elements
			&& [ 'a','b','c','d' ].slice(3) == ['d'] // starting from #3
			&& [ 'a','b','c','d' ].slice(4) == [] // starting from #4
			&& [ 'a','b','c','d' ].slice(-3) == ['b','c','d'] // starting 3 elements back from the end
			&& [ 'a','b','c','d' ].slice(-10,1) == ['a'] // starting 10 elements back from the end (clamped to 0), with length 1
			&& [1,2,3].resize(1,"foo") == [1] && [].resize(2) == [void,void] && [1,2].resize(4,"bar") == [1,2,"bar","bar"]
			,Array);

}

//---
{	// Map
	var m = new Map;
	m["foo"]="bar";
	m["bla"]="da";
	m["dum"]=m["bla"];

	var accum="";
	var i = m.getIterator();

	while(!i.end()){
		accum+= "|"+i.key() +":" +i.value()+m[i.key()]+"";
		i.next();
	}
	accum+=m.count();

	var m1 = new Map;
	m1["foo"]="bar";
	m1["bla"]=[1,2,3];

	var m2={1:2,3:4};
	m1.swap(m2);

	test("Map:", true
			&& accum=="|bla:dada|dum:dada|foo:barbar3"
			&& m2=={"foo":"bar","bla":[1,2,3]}	&& m2!={"foo":"bar","bla":[1,2,3,4]}
			&& m1=={1:2,3:4} && m1.containsKey("1") && !m1.containsKey(5)
			&& ({:} ---|> Map) && ! (({})---|> Map )
			&& {"a":"b","c":"d",1>2?"x":"y":"foo"}.map(fn(k,v){return k+v;}) =={"a":"ab","c":"cd","y":"yfoo"}
			&& {1:2,3:4,5:6}.map(fn(k,v,i){return k+v-i;},1) == {1:2,3:6,5:10}
			&& {"a":1,"b":2,"c":3}.unset("b")=={"c":3,"a":1}
			&& {"foo":"bar",1:2}.merge({3:4,1:"dum"})=={"foo":"bar",1:"dum",3:4}
			&& {"foo":"bar",1:2}.merge({3:4,1:"dum"},false)=={"foo":"bar",1:2,3:4}
			&& {8:5,1:2,3:4}.max()==5
			&& {1:2,3:4,5:6}.get(7)==void
			&& {1:2,3:4,5:6}.get(7,"notFound")=="notFound"
			&& {1:2,3:4,5:6}.get(3,"notFound")==4
			&& {1:2,3:4,5:6}.reduce(fn(sum,key,value){return sum+value+key;},27) == (27+1+2+3+4+5+6)
			&& {1:6}.clear().empty()
			&& {1:2,3:4,5:6,7:8}.filter( fn(key,value){ return key!=3 && value!=8; }) == {1:2,5:6}
			,Map);
}
//---
{
	// element access
	var O = new Type;
	O._get::=fn(index){
		return index*2;
	};
	var o = new O;
	test("Collection:",o[100]==200 ,Collection);
}

//---
{
	out("try/catch/throw:");
	var i = 0;
	try{
	//	out("FOOBA!");
		{{{1/0;}}}
		i+=1;
	}
	catch(){
//		out("\n",e);
	//	out("\nZeile:",__LINE__);
		i+=2;
	} // i == 2
	try{
		throw 4;
	}catch(e){
		i+=e;
	}
	var f = fn(){
		var e = new Exception("foo");
		e.value:=8;
		throw e;
	};
	try{
		f();
	}catch(e){
		if(e---|>Exception)
			i+=e.value;
	}
	if( i==14)
	{out (OK);}else { errors+=1; out(FAILED); }
}
//
// ---
if(!benchmark)
{//disble for benchmarks
	out("load:\t\t");
	GLOBALS.loadTestVar:=0;
	var r = load(__DIR__+"/loadme.escript");
	var r2 = loadOnce("loadme.escript");
	var r3 = loadOnce(__DIR__+"/loadme.escript"); // should be the same file as in the previous line as __DIR__ is in the searchPath

//    out(__FILE__,"\t",__DIR__);
	if( __FILE__==__DIR__+"/Testcases_Core.escript"&&  testFunction(3)==9 && r==5 && r2==5 && r3===void && loadTestVar==2)
	{out (OK);}else { errors+=1; out(FAILED); }
}
//---
if(GLOBALS.isSet($TestObject)){  // TestObject is defined in test.cpp
	var t = new TestObject(1,1);
	t.setM1(9); // int
	t.setM2(9); // float
	t.setM1( t.getM1()+=1.7); // (int) (9 + 1.7) == 10
	t.setM2( t.getM2()+=1.7); // 9+1.7 == 10.7

	test("NumberRef", t.getM1()== 10 && t.getM2()~=10.7 );
}
//---
if(!benchmark)
{
 //disble for benchmarks
   out("Get-/setAttribute:");
   var normalPlus = GLOBALS.Number.getAttribute("+");
   Number.setAttribute("+",fn(i){
		return this-i;
   },EScript.ATTR_TYPE_ATTR_BIT);
   var a = 1+2; // = 1-2;
   Number.setAttribute("+",normalPlus,EScript.ATTR_TYPE_ATTR_BIT);
   var b = 1+2; // = 1+2//
 /// Fakultät
   Number.setAttribute("!_post",fn(){
	   if(this<=1) return this;
	   return this* ((this-1)!);
   },EScript.ATTR_TYPE_ATTR_BIT);
   var c=7!; // 7*6*5*4*3*2*1

   if( a==-1 && b==3 && c==5040 && (1)."+"(2)==3
		&& Number.getAttributeProperties("+") == EScript.ATTR_TYPE_ATTR_BIT	)
		{out (OK);}else { errors+=1; out(FAILED); }
 /// Note: 2008-02-11 When setting Attributes of Type-Objects, Debug-Object-Counting
 /// does not work properly until deleting new Attributes: \todo !!!check this
	Number.setAttribute("!_post",void,EScript.ATTR_TYPE_ATTR_BIT);
/// Todo:    Number.unsetAttribute("!_post");
}

//---
{
	out("Inheritance:\t");
	var A = new Type(ExtObject);
	A._constructor:=fn(){
//        this.a:=getType().a;
//        out("FOOO",this,"\n");
		this.f:=1;
	};
	A.a:=1; /// object-member of A
	A.b:=1; /// object-member of A
	A.c::=1;
	A.setAttribute("d",1,EScript.ATTR_NORMAL_ATTRIBUTE); /// object-member of A
	A.e::=1; /// type-member of A
	A.setAttribute($f,1,EScript.ATTR_TYPE_ATTR_BIT); /// type-member of A

	A._printableName ::= "MyUserdefinedA"; /// change the type name used for toString.
//    print_r(A._getAttributes());
	var a = new A;
//	print_r(a._getAttributes());
//	print_r(A._getAttributes());
//	print_r(A.getObjAttributes());
//	print_r(A.getTypeAttributes());

	var B = new Type(A); ///
	B._constructor::=fn(){
		f++;
	};
//    print_r(B._getAttributes());
//    out("\nA.a: ",A.a," A.b: ",A.b," A.c: ",A.c," A.d: ",A.d," B.a: ",B.a," B.b: ",B.b," B.c: ",B.c," B.d: ",B.d," \n");
	B.a++;
	B.b++;
	B.c++;
	B.d++;
	B.e++;
	B.f++;

	var b = new B;

//    print_r(B._getAttributes());
//    out("A.a: ",A.a," A.b: ",A.b," A.c: ",A.c," A.d: ",A.d," B.a: ",B.a," B.b: ",B.b," B.c: ",B.c," B.d: ",B.d," \n");
	var C = new Type;
	C.m1:=1;
	C."==" :=  fn(b){
		if(b---|>Number){
			return this.m1==b;
		}else return null;
	};

	var c1 = new C;
//    out(c1==1);
//    out(c1==2);
//    out(c1!=2);

	if( A.a==1 &&A.b==1 &&A.c==2 &&A.d==1 &&A.e==2  &&A.f==2 &&B.a==2 &&B.b==2 &&B.c==2 &&B.d==2&&B.e==2&&B.f==2
	&& B.getType()==Type && B.getBaseType()==A && (new B).getType()==B && (new B).getType().getBaseType()==A
	&& (new B).f==2 && c1==1 && !(c1==2) && c1!=2
	&& b.toString().contains("MyUserdefinedA")
	&& b.isSetLocally($a) && !b.isSetLocally($c) && A.isSetLocally($c) && B.isSetLocally($a)
	&& A.getLocalAttribute($a)==1 && B.getLocalAttribute($a)==2 && b.getLocalAttribute($a)==2
	)
		{out (OK);}else { errors+=1; out(FAILED); }

//    {
//        var A = new Object;
//        A.setClassMember("a",1);
//
//        var B = new A;
//        /// this should produce a warning
//        B.a = 2;
//        out(A.a,"\t",B.a); /// should be 1 2
//    }
}

//---
{
	out("Superconstructor:");
	 var A = new Type(ExtObject);
	A._constructor::=fn(a,p*){
		this.m1:=a;
		foreach(p as var v){
			m1-=v;
		}
	};
	var A2 = new Type(A); // useless in-between type whithout constructor
	var B = new Type(A2);
	B._constructor::=fn(v,x,y)@(super(v*2,x,y)){
		m1=-m1;
	};
	var C = new Type(B);
	var a = new A(100,30,20); // 100-30-20
	var b = new C(100,30,20); // -(100*2-30-20) = -150
	var c = new C(100,30,20);

	if( a.m1==50 && b.m1==-150 && c.m1==-150)
		{out (OK);}else { errors+=1; out(FAILED); }
}
//---
{
	out("eval:\t\t");
	var a = eval("var b = 0;while(b<10)b++; b;");
	if( a==10 && eval("fn(a){return a*a;};")(5) == 25 )
		{out (OK);}else { errors+=1; out(FAILED); }
}
//---
{
	out("getDate:\t");
//    out(time().toHex(),"\n");
//    out(time().toIntStr(),"\n");
//    var d = getDate(time());
//    out("year-mon-mday hours:minutes.seconds".replaceAll(d));
//    print_r(d);

	if( "year-mon-mday hours:minutes.seconds".replaceAll(getDate(0x47f10600)) == "2008-3-31 17:40.48")
		{out (OK);}else { errors+=1; out(FAILED); }
}
//---
{
	out("Identifier/isSet:");
	var a = new ExtObject({ $foo2:2 });
	a.foo:=1;
	var t1 = a.getAttribute($foo);
	a.$foo = 2;

	if( t1==1 && a.foo==2 && a.isSet($foo) && a.isSet('foo') &&	!a.isSet('bar') && a.foo2==2
		&& $a != "a" && $a !== "a" && "a" !== $a && "a" == $a
		&& $a ---|> Identifier && $a == new Identifier("a") && $a!=$b )
		{out (OK);}else { errors+=1; out(FAILED); }
}
//---
{	// Delegate
	var a = new ExtObject;
	a.m1:=1;
	a.f:=fn(Number p1){
		return this.m1+p1;
	};
	var d = new Delegate(a,a.f);

	var d2 = a -> fn(){return this.m1;};

	// WARNING! Altough this can lead to perhaps unexpected behavior, Delegates allow a access to primitive types by reference!!!!!
	// I strongly encourage you to use this feature with care!
	var i = 0;
	(i -> fn(){	this++; })();

	test( "Delegate:", true
			&& d(3)==4 && d2()==1 && d2.getObject()==a && d.getFunction()==a.f
			&& i==1
			,Delegate);
}
//---
{
	var list=[getDate(),{"foo":true,"bar":false,"dum":void},-1];
	var jsonList = toJSON(list,false);
//    out("\n",jsonList);
//    print_r(list);
//    print_r(parseJSON(jsonList));

	var original = "foo\0	bar\0hubhub\n\n\n";

	test("JSON:",true
		&& list==parseJSON(jsonList)
		&& parseJSON('"a\\"test\\"b"') == 'a"test"b'
		&& toJSON('a"test"b') == '"a\\"test\\"b"'
		&& original == parseJSON(toJSON(original))
	);
}
// ---
{
	out("PrioQueueTest:\t");

	var PriorityQueue = new Type(ExtObject);
	PriorityQueue._constructor:=fn(comparison='<',data = void){
		this.comparison:=comparison;
		this.a:=[];
		if(data){
			foreach(data as var v)
				add(v);
		}
	};
	PriorityQueue.compare:=fn(a,b){
		return (a -> a.getAttribute(comparison)) (b);
	};
	PriorityQueue.add:=fn(e){
		a.pushBack(e);
		var i = count()-1;
		while(i>0){
			var p=( (i+1)/2).floor()-1;
			if( compare(e,a[p])){
				var t = a[p];
				a[p]=e;
				a[i]=t;
			}else{
				break;
			}
			i = p;
		}
	};
	PriorityQueue."+=":=PriorityQueue.add;
	PriorityQueue.count:=fn(){
		return a.count();
	};
	PriorityQueue.clear:=fn(){
		return a.clear();
	};
	PriorityQueue.swap:=fn(i,j){
		var tmp = a[i];
		a[i]=a[j];
		a[j]=tmp;
	};
	PriorityQueue.extract:=fn(){
		if(count()==0)
			return void;
		var min = a[0];
		if(count()>1){
			a[0]=a.popBack();
			heapify(0);
		}else a.popBack();
		return min;
	};
	PriorityQueue.heapify:=fn(i){
		var left=((i+1)*2)-1;
		var right = left+1;
		var minI = i;
		if(left>=count())
			return;
		if( compare(a[left],a[minI]) )
			minI = left;
		if(right<count() && compare(a[right],a[minI]))
			minI = right;
		if(minI!=i){
			swap(i,minI);
			heapify(minI);
		}
	};
	var a=[83,5,2,756,23,8,3,43,1,74,1,78,3,45,7,123,3];
	var q = new PriorityQueue('<',a);
	var a2=[];
	while(var v = q.extract()){
		a2+=v;
	}
	a.sort();
	if(a==a2)
		{out (OK);}else { errors+=1; out(FAILED); }


//    for(var i = 0;i<100;i++){
//        q+=Rand.equilikely(0,100000);
//    }

}
{
	var f1 = fn(end){
		var v = 1;
		for(var i = 0;i<end;++i){
			yield v;
			v*=2;
		}
	};

	var a=[];
	for(var it = f1(5);!it.end();it.next()){
		a+=it.value();
	}
//	out(a==[1,2,4,8,16]);

	var s="";
	foreach(f1(3) as var i,var j) s+=""+i+":"+j+" ";

	// ---
	// yield and return
	var f2 = fn(){
		yield 1;
		yield 2;
		return 3;
	};
	var s2="";
	foreach(f2() as var value) s2+=value;

	var s3="";
	{
		var it = f2();
		for(;!it.end();it.next())
			s3+=it.value();
		s3+=it.value();
	}

	test("yield:",
		a==[1,2,4,8,16]
	&&  s=="0:1 1:2 2:4 "
	&& s2=="12"
	&& s3=="123"
	);
}

{ // Inheritance restrictions
	var exceptionCaught = false;
	try{
		var Number2 = new Type(Number);

	}catch(e){
		exceptionCaught = true;
	}
	test("Inheritance restrictions:",exceptionCaught);
}

{
	// @(const,private)
	var A = new Type;
	A.constant @(const) := 99;
	A.privateMember @(private) := void;
	A.privateFunction @(private,const) ::= fn(){
		return privateMember;
	};
	A._constructor @(private) ::= fn(bla){
		privateMember = bla;
	};


	var B = new Type(A);
	B._constructor ::= fn(bla)@(super(bla+1)){
	};
	B.publicFunction ::= fn(){
		return privateFunction() + this.constant;
	};

	var b = new B(16); // privateMember = 16+1

	// try some illegal things ;-)
	var exceptionCount = 0;

	// throw some exception
	try{	b.constant = "foo";	}catch(e){	++exceptionCount;	}
	try{	b.privateFunction = "somethingElse";	}catch(e){	++exceptionCount;	}

	// create some warnings
	Runtime.setTreatWarningsAsError(true);
	try{	new A(5);	}catch(e){	++exceptionCount;	}
	try{	b.privateMember++;	}catch(e){	++exceptionCount;	}
	try{	b.privateFunction();	}catch(e){	++exceptionCount;	}
	Runtime.setTreatWarningsAsError(false);

	++b.constant; // constant = 100


	// private constructors and factory methods
	var C = new Type;
	C.create ::= fn(){	return new this;	};
	C._constructor @(private) ::= fn(){	};
	var D = new Type(C);
	D.create2 ::= fn(){		return new this;	};
	C.create();
	D.create();
	D.create2();
	try{	new C;	}catch(e){	++exceptionCount;	}
	

	test("@(const,private):",exceptionCount==6 && b.publicFunction()==117);
}

{	//@(override)
	var A = new Type;
	A.a := 1;
	A.b ::= 2;

	var B = new Type(A);
	B.a @(override) := 10;
	B.b @(override) := 20;

	Runtime.setTreatWarningsAsError(true);
	var exceptionCount = 0;
	try{	B.c @(override) := 20;	}catch(e){	++exceptionCount;	} // should issue a warning, but should still be executed
	Runtime.setTreatWarningsAsError(false);

	var b = new B;
	b.a @(override) := 100;

	test("@(override):", A.a == 1 && exceptionCount==1 && b.a == 100 && b.c==20 );
}


{	//@(init)
	var A = new Type;
	A.a @(init) := fn(){	return 17;	};
	A.b @(init) := 17->fn(){	return this;	};
	A.c @(init) := Array;

	var T = new Type;
	T._constructor ::= fn(){
		this.value := 17;
	};
	A.d @(init) := T;

	var B = new Type(A);
	var b = new B;

	test("@(init):", b.a == 17 && b.b==17 && b.c == [] && b.d.value==17  );
}


{	// switch/case

	var switchTestFn = fn(decision){
		var a = "";
		switch(decision){
			case void: // must be placed before false-case (false==void -> true)
				a+="void";
				break;
			case false:
				a+="false";
				break;
			case 1:
				var v1 = "foo";
				a+="1";
			case 2:
				a+="2";
			case "3":
				a+="3" + v1;
			case 4:
			case 5:
				a+="45";
				break;
			case "foo":{
				a+="foo";
			}
			default:
				a+="default";
			}
		return a;
	};
	switch(1){} // empty switch should be allowed
	
	test("switch/case", switchTestFn(false) === "false" &&
		switchTestFn(void) === "void"  &&
		switchTestFn(1) === "123foo45"  &&
		switchTestFn(2) === "23void45"  &&
		switchTestFn(3) === "3void45"  &&
		switchTestFn(4) === "45"  &&
		switchTestFn(5) === "45"  &&
		switchTestFn("foo") === "foodefault" &&
		switchTestFn("bla") === "default" );
	
	
	//! \todo if first statement is no case statement, a warning should be shown!
	
}

{	// StdLib (not complete!)
	test("StdLib:", !getEnv("PATH").empty() && !getEnv("THIS_SHOULD_NOT_EXIST") &&
		chr(65)=="A" && ord("A")==65 && ord("")==0 );
}



{	// EStdLib (...)
	
	
	Array."=>" ::= fn(callable){
		var myWrapper = thisFn.wrapperFn.clone();
		myWrapper.wrappedFun := callable;
		myWrapper.boundParams := this.clone();
		return myWrapper;
	};

	Array."=>".wrapperFn := fn(params...){
		// _getCurrentCaller() is used instead of "this", as "this" may not be defined if this function
		// is called without a caller. This then results in a warning due to an undefined variable "this".
		return (Runtime._getCurrentCaller()->thisFn.wrappedFun)(thisFn.boundParams...,params...);
	};
	var arr = [1,2,3];
	var f1 = arr => 0->fn(p...){	return [this,p...];	};
	arr+=4;
	var f2 = arr => fn(p...){	return [p...];	};

	test("EStdLib:", f1(4)==[0,1,2,3,4] && f2(5)==[1,2,3,4,5]);
}


{	// raw strings & string concatenation
	test("String literals", "foo" "" "bar" == "foobar" && "0" /*dumdidu*/ '1' "2" +"3" == "0123" && 
R"(a\
b
c\n)" == "a\\\nb\nc\\n"	&& R"#(foo)#" == "foo" && R"Delimiter()Delimiter".empty());	
}
//out(Runtime.getLocalStackInfo());


{	// utf8-support
	var variäblö = 2;
	
	test("(partial) utf8-support", variäblö==2 
		&& "ä".length() == 1
		&& "dämlich".length() == 7
		&& "äöü"[1] == "ö"
		&& "#äöüghf3%ßhksdggnkl"[2] == "ö"
		&& "#äöüghf3%ßhksdggnkl"[9] == "ß"
		&& "äöü".substr(1) == "öü"
		&& "dfgrtg gfd adsäbcßäa".substr(-3)=="ßäa"

	);
//	outln("äbc".substr(1));
//	outln("äbcßäa".substr(-3)=="ßäa");
//	for(var i=0;true;++i){
//		var s = " Fünction pärameter checks "[i];
//		out(i,":",s," ");
//		if(!s)
//		break;
//	}
//	outln([1]);

}

//
//}
//{
//
//
//	// Function parameter checks
//	out("\n Function Tests..\n");
//	Runtime.setTreatWarningsAsError(true);
//
//	var exceptionCount = 0;
//	var okCount = 0;
//	out(":",__LINE__,"\n");
//
//	try { // too few params
//		var f = fn(p1,p2,p3,p4 = 1){};
//		f(1,2);
//	}catch(e){++exceptionCount;}
//
//	out(":",__LINE__,"\n");
//	try { // too many params
//		var f = fn(p1,p2,p3,p4 = 1){};
//		f(1,2,3,4,5);
//	}catch(e){++exceptionCoun;}
//	out(":",__LINE__,"\n");
//
//	try { // wrong type
//		var f = fn([1,2,Array] p1){ return 1; };
//		okCount += f(1);
//		okCount += f(2);
//		okCount += f([]);
//		f(3);
//	}catch(e){++exceptionCount;}
//
//	out(":",__LINE__,"\n");
//	out(exceptionCount , "\t", okCount );
//	Runtime.setTreatWarningsAsError(false);
//
//}
//out("\n");
};
