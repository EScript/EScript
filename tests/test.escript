// test.escript
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
outln ("EScript Testcases\nVersion: ",EScript.VERSION_STRING,"\n","-"*79);
[var a,var b] = [1,2];
outln(a);
outln(b);
var arr=[];
print_r( [arr[0],b] = [3,4] );
print_r(arr);
//// -->
{
	var X = [1,2];
	var a = X[0]; 
	var b = X[1]; 
	X;
}

var tolleFunction = fn(){
	return ["foo","bar"];
};
[var v1,var v2] = tolleFunction();

/*



   |         |
h0 |\       /|
   | \     / |
...|..\_|_/  |
   |    p    |___0
  or0 
 

p0 = p - off0
 
|h0-or0'|^2 + |p0-or0'|^2 = |a0| ^2 

h0-or0 = sqrt( |a0|^2 - |p0-or0'|^2 )


*/

static vec3Minus = fn(Array v0,Array v1){
	return [v0[0]-v1[0],v0[1]-v1[1],v0[2]-v1[2]];
};

static vec3Plus = fn(Array v0,Array v1){
	return [v0[0]+v1[0],v0[1]+v1[1],v0[2]+v1[2]];
};
static vec3Length = fn(Array v){
	return (v[0]*v[0] + v[1]*v[1] + v[2]*v[2]).sqrt();
};

/*

     |
     x
     |\
     | \
 hRel|  \
     |   \  x - target
o2__ x____X/ - targetOffset
     |     - pos
     |
     x Origin
*/


static ARM_LENGTH = 1;
static calc = fn(Array target,Array origin,Array targetOffset){
	var pos = vec3Plus(target,targetOffset);
	var origin2 = origin.clone();
	origin2[1] = pos[1]; 
	var hRel = (ARM_LENGTH.pow(2) - vec3Length(vec3Minus(pos,origin2)).pow(2) ).sqrt();
	return hRel + origin2[1]-origin[1];
};

var origin0 = [0,0,0];
var origin1 = [1,0,0];
var origin2 = [0,0,1];
var targetOffset0 = [0,0,0];
for(var x=0.0;x<1.11;x+=0.1){
	var p = [x,0.5,0];
	outln(x,"\t",calc( p,origin0,targetOffset0),"\t",
			calc( p,origin1,targetOffset0),"\t",
			calc( p,origin1,targetOffset0));
	
}

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
