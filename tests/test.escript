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
var t = load("Testcases_Core.escript");

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
			t();
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
	t();
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
