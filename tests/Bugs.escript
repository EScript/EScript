
{
	// Bug - Wrong Calling-Object when UserFunctionCall contains expression
	// Found 2007-03-27 fixed 2007-03-27


	GLOBALS.A:=new Type();
	A._constructor:=fn(i){
//	    out("Constructing an Object!\n");
		this.nr:=i;
	};

	var nr = 2;
	var a = new A(nr*3);
	test( "BUG[20070327]",	a.nr==6 );
}
{
	// Bug[20070626] - Lost object namespace in Blocks inside functions
	// Found 2007-06-26

	var A = new Type();

	GLOBALS.f2:=fn(){
		out(this.a);
	};
	A.a:=1;
	A.f:=fn(){
		a++;
		this.a++;
		for(var i = 0;i<3;++i)
		{
			a++;
			this.a++;
			{
				var a = 0;
				a--;
				this.a++;
			}
			// f2(); // throws Error as it should.
			//out(a,"\n");
		}
		a++;
		this.a++;
		return(a);
	};
	var a = new A();
	test( "BUG[20070626]", a.f()==14 );

}
{
	// Bug[20070703] Value assignement "by reference" instead of "by value".


	var s1="foo";
	var s2 = s1;
	s2+="bar";
	//out(s1,":",s2,"\n");

	var i1 = 1;
	var i2 = i1;
	i2++;
	//out("\n",i1,"\n"); // ==1

	var o1 = new ExtObject();
	o1.a:=1;
	var o2 = o1;
	o2.a++;
	//out("\n",o1.a,"\n"); // ==2

	var a=[1,2,3];
	var b = a[0];
	b++;
	//out("\n",a[0],"\n"); //==1

	test( "BUG[20070703]", (s1=="foo")&(s2=="foobar")&
		(i1==1)&(i2==2)&
		(o1.a==2)&(o2.a==2)&
		(b==2)&(a[0]==1));

}/**/

{
	// Bug [2007-08-07]
	/*
	GLOBALS.loader=...
	while(true) {
	if(loader) {
		if(!loader.load(5)) {

			loader = false; // geht nicht
			GLOBALS.loader = false; // geht
		}
	*/
};
{
	//Bug[20071030] null and void are not parsed correctly.
	test( "BUG[20071030]", void==null );
}
{
	//Bug BUG[20080210.1] no call by value with simple types

	var f = fn(x){
		x++;
	};
	var a = 1;
	f(a);
	test( "BUG[20080210]", a==1 );

}
{ // MEMLEAK
//   Bug BUG[20080210.2] wrong Parsing of fn(){}something;
//   Does not create Syntax error
	var errorFound = false;
	try{
		eval('var f = fn(){ out("!!");  } bla bla bla;');
	}catch(e){
		errorFound = true;
	}
	test( "BUG[20080210.2]", errorFound );
}
{// MEMLEAK

	// this should create a runtime error (Block.+ not found) OR a syntax error .
	// (Not shure about what's better...)
	var errorFound = false;
	Runtime.setTreatWarningsAsError(true);
	try{
		eval('+out("");');
	}catch(e){
		errorFound = true;
	}
	Runtime.setTreatWarningsAsError(false);

	   test( "BUG[20080229]", errorFound );
}
{
	// Calling object is not set properly when calling member function without this.

	var A = new Type();
	A.v:=1;
	A.f1:=fn(){
		 return this.v;
	};
	A.f2:=fn(){
		return f1(); // this should be equivalent to this.f1()
	};
	var a = new A();
	a.v = 2;

	   test( "BUG[20080324]", a.f2()==2 );
}
{
	// Calling object is not set properly when calling member function without this.
	eval("-1;"); // resulted in crash of the tokenizer when first token is a '-'
	   test( "BUG[20090107]", true );
}
{
	 // Linebreaks inside Strings are ignored in line counting.
	var l1 = __LINE__;
	"line 1: dum di dum
	line 2:da da";
	var l2 = __LINE__;
	   test( "BUG[20090107.2]", (l2-l1)==3 );
}
{
	// Wrong operator precedence.


	   test( "BUG[20090417]", 1.0/1.0*2.0==2 );
}
//{
//    // Parser does not realize forgotten ';' after function calls.
//    out("BUG[20090417.2]\t");
//    var f = fn(i){out(i);};
//
//    var errorFound = false;
//    try{
//        eval('f("a")out("b");');
//    }catch(e){
//        errorFound = true;
//    }
//    out(errorFound,"\n");
//}
{
	// '!=' is not automatically ! '==' as default

	var C = new Type();
	C.m1:=1;
	C.setAttribute("==",  fn(b){
		if(b---|>Number){
			return this.m1==b;
		}else return false;
	});
	var c1 = new C();
	test( "BUG[20090418]", c1==1 && !(c1==c1) &&  c1!=c1 );
}
{
	// Assignements to GLOBALS does not work.

	GLOBALS.g1:=1;
	{
		g1 = 2;
	}
	   test( "BUG[20090424]", g1==2 );
}
{
	// Wrong operator precedence.

	   test( "BUG[20090525]", 1-2+3==1+3-2 );
}
{
	// Array.append appends references of primitives instead of copies

	var a=[0,0,0];
	var b=[2,2].append(a);
//    print_r(b);
	a[0]++;
//    print_r(b);
	test( "BUG[20090714]", b[2]==0 );
}
{
	// Array.indexOf() does not use EScript.== operator

	var A = new Type();
	A.'==':=fn(other){
		return (other---|>getType());
	};
	var a=[0,new A(),1,2,3];
	test( "BUG[20090907]", (a.indexOf(new A())==1)&&(!a.indexOf(4)) );
}

{
	// Array and Map cloning: Simple types are not copied but referenced.

	var i = 0;
	var m1={ 'a':1 };
	var m2 = m1.clone();
	if(m1==m2) i++;
	m1['a']++;
	if(m1!=m2) i++;

	var a1=[1];
	var a2 = a1.clone();
	if(a1==a2) i++;
	a1[0]++;
	if(a1!=a2) i++;
	test( "BUG[20090922]", i==4 );
}
{

	//out("BUG[20100315] \t",(m.a==2 && m2.a==1 && m[0]==1 && m2[0]==3) && (a.a==2 && a2.a==1 && a[0]==1 && a2[0]==3),"\n");
}
{
	// map.merge: Simple types are not copied but referenced.

//	{1:2}.clone();
	var m1={1:2};
	var m2={3:4};
	m1.merge(m2);
	m2[3]++;

	test( "BUG[20100604]", m1[3]==4 );

}
{
	//Memory not freed when a Map is cloned. (this can only be tested in memory debug mode)
	var m1={1:2};
	var m2 = m1.clone();
	out("MemLeak[20100604] \t ok?\n");
}
{ // MEMLEAK
	// assignInheritedAttribute not used in ExtObject

	Type.thisShouldntBeHere::=true;
	var A = new Type(ExtObject);
	var a = new A();

	var errorFound = false;
	Runtime.setTreatWarningsAsError(true);
	try{
		a.thisShouldntBeHere;
	}catch(e){
		errorFound = true;
	}
	Runtime.setTreatWarningsAsError(false);
	test( "BUG[20100605]", errorFound );
	Type.thisShouldntBeHere = void;

}
{	// Object member functions are accessible locally without an Object!

	var errorFound = false;
	Runtime.setTreatWarningsAsError(true);
	try{
		isSet; // this should not exist.
	}catch(e){
		errorFound = true;
	}
	Runtime.setTreatWarningsAsError(false);
	test( "BUG[20100618]", errorFound );
}
{ // do...while with continue does not re-check condition;

	var i = 0;
	do{
		++i;
		continue;
	}while(i<3); // if the bug exists, the loop never returns
	test( "BUG[20100711]", i==3 );
}
{	// parser does not accept functions as default parameter

	var ok = true;
	try{
		eval("fn(a=fn(){}){}; fn(a={1:2}){};");
	}catch(e){
		ok = false;
	}
	test( "BUG[20100717]", ok );
}
{ 	// undeclared member attribute not detected

	var errorFound = false;
	Runtime.setTreatWarningsAsError(true);
	try{
	var A = new Type();
		A._constructor::=fn(){
			noMemberWithThisName = 1;
		};
		new A();
	}catch(e){
		errorFound = true;
	}
	Runtime.setTreatWarningsAsError(false);
	test( "BUG[20100815]", errorFound );
}
{	// assigning to an undefined member of a Type-Object results in a crash.
	Runtime.setTreatWarningsAsError(true);
	var ok = true;
	try{
		var a = new Type();
		a.foo = 5;
		FOO.bla = 5;
	}catch(e){
	}
	Runtime.setTreatWarningsAsError(false);
	test( "BUG[20110217]", ok );
}
{ // #17966	Wrong line number is reported when parameter type check fails
	var lineOfFunction;
	var lineOfException;
	try{
		lineOfFunction = __LINE__ + 1;
		var f = fn(Number a){};
		f("bla");
	}catch(e){
		lineOfException = e.getLine();
//		out(lineOfException,":",lineOfFunction);
	}
	// check exception line
	var exceptionLineCorrect = false;
	try{
		Runtime.exception(""+__LINE__);
	}catch(e){
		exceptionLineCorrect = e.getMessage() == ""+e.getLine();
	}

	test( "BUG[20110227]", lineOfException==lineOfFunction && exceptionLineCorrect);

}

//{ 	// endless recursion does not throw an exception but results in a crash
//
//	var errorFound = false;
//	var oldLimit = Runtime._getStackSizeLimit();
//	Runtime._setStackSizeLimit(Runtime._getStackSize()+20);
//	var i = 0;
//	try{
//		var f = i->fn(){
////			out(Runtime._getStackSize()," ");
//			++this;
//			(this->thisFn)();
//		};
//		f();
//	}catch(e){
//		errorFound = true;
//	}
////    out("\n",i,"\n");
//	Runtime._setStackSizeLimit(oldLimit);
//	test( "BUG[20110314]", errorFound && i>17 && i<22);
//
//}
{	// system crashes if a wrong parameter type is given in a user constructor call.
	var exceptionCaught = false;
	try{
		var A = new Type();
		A._constructor ::= fn( Array a){
		};
		new A("foo");
	}catch(e){
		exceptionCaught = true;
	}
	test( "BUG[20110321]", exceptionCaught);
}

{	// execution of default parameter expressions and type checks are performed in the calling context and not the function context

	// typeCheck
	var exceptionCaught = false;
	try{
		var f = fn(Array a){
		};

		{
			var Array = new Type();
			f([]);
		}
	}catch(e){
		Runtime.warn(e);
		exceptionCaught = true;
	}

	// default parameter expression
	try{
		var f = fn(a = new Array()){
		};

		{
			var Array = "noType";
			f();
		}
	}catch(e){
		Runtime.warn(e);
		exceptionCaught = true;
	}
	test( "BUG[20110328]", !exceptionCaught);
}
{	// cloned ReferenceObjects are broken (due to silly ReferenceObjectClonePolicies idea)

	var exceptionCaught = false;
	try{
		var r1 = new Math.RandomNumberGenerator();
		r1.equilikely(0,10);
		r1.clone().equilikely(0,10);

	}catch(e){
		Runtime.warn(e);
		exceptionCaught = true;
	}
	test( "BUG[20110530]", !exceptionCaught);

}
//
//{	// if too few parameters are given, the global value is used instead of a locally defined void
//
//	GLOBALS.__test20110604a := 1;
//	GLOBALS.__test20110604b := 2;
//
//	var l = Runtime.getLoggingLevel();
//	Runtime.setLoggingLevel(Runtime.LOG_ERROR); // ignore warnings
//	test( "BUG[20110604]",
//			(fn(__test20110604a,__test20110604b) { return void == __test20110604a && void == __test20110604b;	} )() );
//	Runtime.setLoggingLevel(l);
//}
{	// if a file is loaded that returns a UserFunction, this function is destroyed during loading and the system crashes.

	test( "BUG[20110616]", (load(__DIR__+"/BUG20110616.escript"))(27) == 27);
}

{	// continue without loop crashes the runtime.
	var exceptionCounter = 0;
	try{
		eval("(fn(){	continue; })();" );
	}catch(e){
		++exceptionCounter;
	}
	try{
		eval("(fn(){	break; })();" );
	}catch(e){
		++exceptionCounter;
	}
	test( "BUG[20110808]", exceptionCounter==2);
}
{	// calling library functions which do not accept parameters does not produce a warning.
	var errorFound = false;
	Runtime.setTreatWarningsAsError(true);
	try{
		clock(12,3,4);
	}catch(e){
		errorFound = true;
	}
	Runtime.setTreatWarningsAsError(false);
	test( "BUG[20110905]", errorFound );
}
{	// late added object attributes result in unexpected behavior and should issue a warning on access.
	var A = new Type();
	var B = new Type(A);
	var a = new A();

	// adding object attribute AFTER creating an inherited Type and an instance.
	A.m := 1;

	var exceptionCounter = 0;
	Runtime.setTreatWarningsAsError(true);
	try{	a.m;		}catch(e){	++exceptionCounter;	}
	try{	a.m = 1;	}catch(e){	++exceptionCounter;	}
	try{	B.m;		}catch(e){	++exceptionCounter;	}
	try{	B.m = 1;	}catch(e){	++exceptionCounter;	}

	// accessing of object attributes of types which can't store object attributes should result in a warning.

	var C = new Type(Object);
	C.m := 1;
	var c = new C();
	try{	c.m;		}catch(e){	++exceptionCounter;	}
	try{	c.m = 1;	}catch(e){	++exceptionCounter;	}


	Runtime.setTreatWarningsAsError(false);
	test( "BUG[20110918]", exceptionCounter == 6 );
}

{	// assignment to inherited static attributes does not work.
//	Runtime.setTreatWarningsAsError(true);
	var errorFound = false;
	try{
		var A = new Type();
		A.staticAttribute ::= 0;

		var B = new Type(A);
		B.assignStaticAttribute ::= fn(){
			staticAttribute = 1;
		};	

		B.assignStaticAttribute();
	}catch(e){
		errorFound = true;
	}
//	Runtime.setTreatWarningsAsError(false);
	test( "BUG[20120215]", !errorFound );

}
{	// Using invalid superconstructor parameters for the base-constructor is not detected and can crash the program
	var errorFound = false;
	try{
		var A = new Type();
		A._constructor ::= fn()@(super("A Map is expected here!")){};
		new A();
	}catch(e){
		errorFound = true;
	}
	test( "BUG[20120226]", errorFound );
}
{	// local variables can be assigned by using ':=', which should produce a warning.
	var errorFound = false;
	var result1 = false;
	var result2 = false;
	
	Runtime.setTreatWarningsAsError(true);
	try{
		/* this should produce a warning, which is transformed
			into an exception before the 'execute()' is executed. */
		result1 = eval("var a; a:=true; out('Should not be executed!'); a;"); 
	}catch(e){
		errorFound = true;
	}
	Runtime.setTreatWarningsAsError(false);
	// but it should work like an ordenary assignment
	var l = Runtime.getLoggingLevel();
	Runtime.setLoggingLevel(Runtime.LOG_ERROR); // ignore warnings
	result2 = eval("var a; a:=true; a;"); 
	Runtime.setLoggingLevel(l);
	
	test( "BUG[20120226.2]", !result1 && result2 && errorFound );
}
{ // Attributes with @(init) are initialized twice, if an object has an user defined constructor
	var A = new Type();
	A.m @(init) := fn(){	return 5;	};
	A._constructor ::= fn(){	this.result := m;	};

	var a = new A();

	test( "BUG[20120228]", a.result == 5 );
}
{ // Global attributes can not be assigned from within an member function
	GLOBALS.foo := 1;
	var f = fn(){
		foo = 2;
	};
	(17->f)();
	test( "BUG[20120412]", GLOBALS.foo == 2 );
}

{ // Superconstructor parameters do not work together with user functions
	var A = new Type();
	A.userFunction ::= fn(){	return "foo";	};
	A.result1 := void;
	A.result2 := void;

	A._constructor ::= fn(x,y){ result1 = x; result2 = y;	};


	A.TypeWithUserConstructor := new Type();
	A.TypeWithUserConstructor._constructor ::= fn(x){this.x:=x;};

	var B = new Type(A);
	B._constructor ::= fn()@(super( userFunction(),new TypeWithUserConstructor("bar") )){};


	var b;
	var errorFound = false;
	try{
		b = new B();
	}catch(e){
		errorFound = true;
	}
	test( "BUG[20120715]", !errorFound && b.result1=="foo" && b.result2.x=="bar" );
}

{ 	// berliOS bug #18694 (https://developer.berlios.de/bugs/?func=detailbug&bug_id=18694&group_id=12246)
	// internal call member functions do not check for nullptr. Result: segmentation 
	// faults and undetected errors.
	
	var exceptionCount = 0;
	try{
		[new ExtObject(),new ExtObject()].sort(); // ExtObject has no '<' operator, so this should fail
	}catch(e){
		++exceptionCount;
	}

	try{
		// ExtObject has no .getIterator attribute, this should fail and NOT throw
		// a segmentation fault
		foreach(new ExtObject() as var foo){}
			
	}catch(e){
		++exceptionCount;
	}
	
}
{	// line numbers are not counted in raw strings
	var l1 = __LINE__;
	R"(+1
	+2
	+3
	+4)";
	var l2 = __LINE__; // +5
	
	test( "BUG[20120814]", l2 == l1+5);
}

{ // constructors shouldn't need parameters
	var ok = true;
	try{
		eval(R"(
			var A = new Type;
			A.B := new Type;
			new A.B;
				
		)");
	}catch(e){
		ok = false;
	}
	test( "BUG[20120815.1]", ok);
}
{ // tokens after attributes and arrays should not be ignored
	var errorsFound = 0;
	try{
		eval("		Array.count 2 1 2 'foo' bla;	");
	}catch(e){
		++errorsFound;
	}	
	try{
		eval("		fn( a = [2,3] foo 'bla' ){};	");
	}catch(e){
		++errorsFound;
	}
	try{
		eval("		var a = [1,2] bla;	");
	}catch(e){
		++errorsFound;
	}	
	try{
		eval("		var a = []; a[1] 'foo';	");
	}catch(e){
		++errorsFound;
	}
	test( "BUG[20120815.2]", errorsFound==4);
	if(errorsFound<4)
		outln( "...", errorsFound,"/4");
}
{ // controls with empty values in brackets can not be parsed.
	var errorsFound = 0;
	try{
		eval("		fn(){exit();};");
	}catch(e){
		++errorsFound;
	}
	test( "BUG[20120816]", errorsFound==0);
}
{ // Empty loop body resulted in a statement being nullptr
	while(false); // Crashes the test with a segmentation fault if the test fails.
	test("BUG[20120821]", true);
}

{	// wrong _printableName can result in an endless loop.
	var e = new ExtObject;
	e._printableName @(override) := e;
	new String(e); // endless loop
	test("BUG[20121010.1]", true);
}

{	// setting type attributes on a extObject should issue a warning (which can be overriden by using setAttribute)
	var errorFound = false;
	Runtime.setTreatWarningsAsError(true);
	try{
		var e = new ExtObject;
		e.foo ::= 1;
	}catch(e){
		errorFound = true;
	}
	Runtime.setTreatWarningsAsError(false);
	test("BUG[20121010.2]", errorFound);
}
{	// The process crashes when parsing a constructor call with an object expression in brackets and without parameters.
	var errorFound = false;
	new (Array); // this is ok and should NOT crash!
	try{
		eval("new ();"); // this should result in an exception (syntax error) -- not a crash.
	}catch(e){
		errorFound = true;
	}
	test("BUG[20121025]", errorFound);
}

{	// Calling a function with a multiParameter and default values produces an access violation (crash)
	var f = fn(a,b="bar",c...){
		return a=="foo" && b=="bar" && c==[];
	};
	
	test("BUG[20130614]", f("foo"));
}


{	// An exception in a yielded function does not invalidate the yieldIterator. When called again, this produces a "Empty Stack" error.
	var yieldIt = fn(){
		yield;
		1/0; // exception
		return 1; // do something after the exception
	}();
	
	var exceptionsCaught = 0;
	try{	yieldIt.next();	}catch(e){		++exceptionsCaught;	}
	
	test("BUG[20130715]", exceptionsCaught == 1 && yieldIt.end() );
}



{	// Invalid number parsing for large numbers (depending on the occurrence of a dot in the number)
	
	test("BUG[20130723]",	1.0e+10==1e+10 && // bug
							1.0e+09==1e+09 &&
							2.0e+10==2e+10 && // bug
							2.0e+09==2e+09 );
}
{	// Calling a delegate with an invalid (=nullptr; not void) function leads to a SEGFAULT.
	
	var l = Runtime.getLoggingLevel();
	Runtime.setLoggingLevel(Runtime.LOG_ERROR); // ignore warnings
	
	var exceptionsCaught = 0;
	try{	(1->void())();	}catch(e){		++exceptionsCaught;	} // should throw an exception but no SEGFAULT.

	Runtime.setLoggingLevel(l);
	
	test("BUG[20130815]", exceptionsCaught == 1 );
}

{	// replaceAll with one parameter may crash.
	
	var exceptionsCaught = 0;
	try{	"foo".replaceAll("o");	}catch(e){		++exceptionsCaught;	} // should throw an exception not crash

	test("BUG[20130922]",	exceptionsCaught == 1 );
}

