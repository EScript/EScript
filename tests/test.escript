// test.escript
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
outln ("EScript Testcases\nVersion: ",EScript.VERSION_STRING,"\n","-"*79);

//outln( "foo"._check("foo") );
//outln( String._check("foo") );
//outln( String._check(2) );
//outln( Number._check(false) );
//outln( Number._check(2) );
//outln( Type._check( String ) );
//
//outln( Object._check );
//outln( Type._check );
//outln( String._check );
//outln( "foo"._check );
//
//var f = fn( [String,Number,void] a){ a+1;};
//
//outln (Object._check);
//outln (void._check);
//
//outln (void._check(void));
//outln( f._asm() );
////f(1);
////f("foo");
////f(void);
//
//static o = new ExtObject;
//o._printableName @(override) := $ParamTester;
//o._check := fn(p){
//	outln("Checking ",p);
//	return true;
//};
//
//var f2 = fn(o p1){
//};
//
//f2(1);
//
//
//var f3 = fn( [Number,1,2] p1...){
//};
//
//f3(1,2,3,"foo");
//exit;

//----
// init
GLOBALS.benchmark:=false;
//GLOBALS.errors:=0; // error count
static errors = 0; // error count
//! new testing function
GLOBALS.test := fn(description,result,checkCoverageType=false){
	if(!result)
		++errors;
	if(!benchmark){
		out(description.fillUp(25," "),result?"ok":"failed");

		if(checkCoverageType){
			var numFunctions = 0;
			var coveredFunctions = 0;
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

(fn(){})._asm(); // ... to mark _asm as executed.

var start = clock();
var outBackup = out;
addSearchPath(__DIR__);

//----
var t = load("Testcases_Core.escript");

if(benchmark){
	var progress = fn( percent ){
		var i = (percent*20).floor();
		SGLOBALS.out("\r","|"+"="*i+"|"+" "*(20-i)+"| "+percent.round(0.01)*100+"%	");
	};

	GLOBALS.out:=fn(values*){ ;};
	var times = [];
	var tries = 200;
	var innerLoops = 20;
	var sum = 0;

	Runtime._setAddStackInfoToExceptions(false); // disable costly stack infos

	progress(0);

	for(var i = 0;i<tries;++i){
		var startTime = clock();
		for(var j = 0;j<innerLoops;++j){
			t();
		}
		var time = (clock()-startTime)*1000;
		sum += time;
		times += (time/innerLoops).round(0.01); // ms per execution
		progress(i/tries);
	}
	progress(1);
	Runtime._setAddStackInfoToExceptions(true);

//	print_r(times);
	times.sort();
	SGLOBALS.out("\nMin:",times.front(),"ms\tMed:",times[ (times.count()*0.5).floor() ],"ms\tMax:",times.back(),"ms\n" );
	SGLOBALS.out("Avg:",sum/ (tries*innerLoops),"\n");
}else{
	try{
		t();
	}catch(e){
		Runtime.log(Runtime.LOG_ERROR,e);
		++errors;
	}
}

load("Testcases_IOLib.escript");
load("Testcases_MathLib.escript");
load("Testcases_Runtime.escript");
load("Testcases_StdLib.escript");
//if(getOS()=="WINDOWS")
//	load(__DIR__+"/Testcases_Win32Lib.escript");
load("Bugs.escript");

GLOBALS.out = outBackup;

out("\n-----\n");
if(errors>0)
	out("Errors:\t\t",errors,"!\n");
else
	out("No Errors.\n");

out("Duration:\t",clock()-start," sec\n");

return "Bye!";
// ----------------------------------------------------------
