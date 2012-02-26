// Testcases for HasEScript Scripting-Language 0.5
out ("EScript Testcases\nVersion: ",EScript.VERSION_STRING,"\n","-"*79,"\n");

var a = new ExtObject();
//a.CONST (const) := $foo;
//a.CONST @(const) := $foo;
//a.CONST @(const) := $foo;
//a.CONST [[const]] := $foo;
//a.CONST (const,private) := $foo;
//a.CONST @(const,private) := $foo;
//a::CONST (const,private,type) =
//a.CONST <...> --->
//a.CONST (<...>)

////a.CONST <const,private> := $FOO;
//a.CONST @($const,$private) := $FOO;
//a.m @(allowType(Number),$private) := $FOO;
//var a = @(private);
//// @(c0...cn) ... annotation
//// fn()@(namespace(N)) { };
//// fn()@(namespace(N),log,info("Implements something.")) { };
//a @(lastParams(1,2,3)) ();
//
//a @(const,private) :=
//a @(const private) :=
//a @($const,$private) :=

////////a._setAttribute( $CONST, EScript.ATTR_CONST|EScript.ATTR_PRIVATE );
a.m1 @(const) := $CONST;
//a.m1 = 2;

a.m2 @(private) := 1;
//a.m2 = 2;

a.m2;
a.m3 @(init) := Array;

a.f := fn(){
	return m2;
};



out(a.f());
out(a.m3);

{
var T = new Type();
T.m1 @(init) := Array;
T.m2 @(init) := fn(){	return "foo";	};

var t=new T();
out(t.m1,"\n");
out(t.m2,"\n");
	
}

{
	var A = new Type();
	A.foo ::= $bar;
	
	var B = new Type(A);
	B.foo @(override) ::= $foo;
	B.foo2 @(override) ::= $foo;
	
	
}

//parser.getAnnotations ( positionOf@, end)
//result:
//Annotation*: name -> parameters*

////////
////////fn() @super( bla,blub), @log() {
////////};
////////
////////fn() @super($s) {
////////};
//
//fn() @debug  {
//
//}
//fn() @initially( { thisFn.bla:=1; } ){
//};
//
//once{
//	thisFn.bla := 0;
//};
//thisFn.bla+=1;


//a.CONST @attr(const,private,sameType) {
//}
//
//a.CONST @(attr(const))  :=
//
//fn( ) @(super(foo), info("bla"), log("fnCalled")) {
//};
//
//fn( ) @(super(1)) {
//};
//
//a.p @attr(private)  :=

//// ------



//fn()
//
//fn(){
//	runtime.getCallingAnnotations();
//};
//@once{
//};
//
//@($noOptimizations){
//
//}
//a @(log) += 2;

//----
// init
GLOBALS.benchmark:=false;
GLOBALS.errors:=0; // error count
//! new testing function
GLOBALS.test := fn(description,result,checkCoverageType=false){
	if(!result)
		++errors;
	if(!benchmark){
		out(description.fillUp(25," "),result?"ok":"failed");

		if(checkCoverageType){
			var numFunctions=0;
			var coveredFunctions=0;
			var missing = [];
			foreach(checkCoverageType._getAttributes() as var fun){
				if(! (fun---|>Function))
					continue;
				++numFunctions;
				if(fun._getCallCounter()>0){
					++coveredFunctions;
				}else{
					missing+=fun.getOriginalName();
				}
			}
			out("\t (",coveredFunctions,"/",numFunctions,")");
			if(!missing.empty()){
				out("\nMissing: ",missing.implode(", "));
			}
		}
		out("\n");

	}

};

var start=clock();
var outBackup=out;
addSearchPath(__DIR__);

//----
var t=(new Parser()).parseFile(__DIR__+"/Testcases_Core.escript");

if(benchmark){
	var progress = fn( percent ){
		var i = (percent*20).floor();
		SGLOBALS.out("\r","|"+"="*i+"|"+" "*(20-i)+"| "+percent.round(0.01)*100+"%    ");
	};

	GLOBALS.out:=fn(values*){ ;};
	var times = [];
	var tries = 200;
	var innerLoops = 20;
	var sum = 0;

	progress(0);

	for(var i=0;i<tries;i++){
		var startTime = clock();
		for(var j=0;j<innerLoops;j++){
//			var t=(new Parser()).parseFile(__DIR__+"/Testcases_Core.escript");
			t.execute();
		}
		var time = (clock()-startTime)*1000;
		sum += time;
		times += (time/innerLoops).round(0.01); // ms per execution
		progress(i/tries);
	}
	progress(1);
//	print_r(times);
	times.sort();
	SGLOBALS.out("\nMin:",times.front(),"ms\tMed:",times[ (times.count()*0.5).floor() ],"ms\tMax:",times.back(),"ms\n" );
	SGLOBALS.out("Avg:",sum/ (tries*innerLoops),"\n");
}else{
	t.execute();
}

load("Testcases_IOLib.escript");
load("Testcases_MathLib.escript");
load("Testcases_Runtime.escript");
//if(getOS()=="WINDOWS")
//	load(__DIR__+"/Testcases_Win32Lib.escript");
load("Bugs.escript");

GLOBALS.out=outBackup;

out("\n-----\n");
if(errors>0)
	out("Errors:\t\t",errors,"!\n");
else
	out("No Errors.\n");

out("Duration:\t",clock()-start," sec\n");

return "Bye!";


// ----------------------------------------------------------
/*!
Some benchmark results:

  Revision400 1000x 5.343S (TestRelease)
 Revision400 1000x 10.54S (TestRelease)

 Rev451 500x 3.0S (DebugRelease)
 Rev452 500x 1.3S (DebugRelease)
 Rev455 500x 1.9S (DebugRelease)//
 Rev455 500x 1.9S (DebugRelease)//
 Rev463 500x 1.0S (DebugRelease)//
 Rev463(Schmörf) 500x 2.0S (DebugRelease)
 Rev491(Schmörf) 500x 2.6S (DebugRelease)
 Rev498(Schmörf) 500x 1.44S (DebugRelease O3)
 Rev504 1000x 2.8s (DebugRelease O3)
 Rev506 1000x 2.62s (DebugRelease O3)
 Rev508(Schmörf) 500x 1.36S (DebugRelease O3)
 Rev523(Schmörf) 1000x 5.76 (DebugRelease O3) + prioQueue test.
 Rev536 (Schmörf) 10000x 57.752 (DebugRelease O3) + prioQueue test.
 Rev537 (Schmörf) 10000x 56.562 (DebugRelease O3) + prioQueue test.

 2009-11-15 (Schmörf) 1000x 5.5 (DebugRelease O3) + prioQueue test. 5.931.789 byte
 removeRef  inline 5.7 (Rel 1.093.120 Bytes )
 removeRef not inline 6.2 (Rel 920.064 byte)
 removeRef  half inline 5.3 (Rel (937.984 Bytes )
 assertParamCount half inline 5.2 (940.032 Bytes)
 assertType half inline (939.520)
  setCallingObj inline 5.0 (939.520)
 Rev 585 2010-05-16 (Schmörf) 1000x 5.1 (DebugRelease O3) + prioQueue test.
 Rev 595 2010-06-03 (Schmörf) 1000x 5.1 (DebugRelease O3) + prioQueue test.
 Rev 596 2010-06-03 (Schmörf) 1000x 6.2 (DebugRelease O3) + prioQueue test + SuperconstructorTest
 neuer ref 6.2
 neuer RTB 6.1
 factory RTB ~6.2 + pool 6.0
 array cleanup 5.5
 array pool 1: 5.0?? ... 4.92
 new HMap -> LocalVarMap 5.0s -> 4.75s
 Operator redesign 4.75s -> 4.65s
 UserFunction Block init-fix 4.7s -> 4.55s
 Rev 691 2010-07-23 1000x 4.59 -> 4.75 (new testcase for sorting added)
 Rev 747 2011-01-23 1000x 4.9
 Rev 755 2011-01-25 1000x 5.17
  5.89 after foreach redesign... 4.234 (new gcc 4.4)
Rev 5 2011-02-01 1000x 4.2
object.execute removed: 5.25 dynamic_cast at every rt.executeObj
typeIds added: 4.25
typeIds used to replace some additional dynamic_casts ->4.0s
Rev29 3.92s
0.5.5 Rev 56 4.12s
*/
