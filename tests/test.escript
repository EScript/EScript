/*********
 *  Testcases for HasEScript Scripting-Language 0.5
 **********/
var start=clock();

var outBackup=out;

GLOBALS.benchmark:=false;

out ("Testcases for ",VERSION,"\n");

var t=Parser.parseFile("tests/Testcases_Core.escript");

if(!benchmark)
    t.execute();
// Revision400 1000x 5.343S (TestRelease)
// Revision400 1000x 10.54S (TestRelease)

//// benchmark
//  // Rev451 500x 3.0S (DebugRelease)
//  // Rev452 500x 1.3S (DebugRelease)
//  // Rev455 500x 1.9S (DebugRelease)//
// Rev455 500x 1.9S (DebugRelease)//
// Rev463 500x 1.0S (DebugRelease)//
// Rev463(Schmörf) 500x 2.0S (DebugRelease)
// Rev491(Schmörf) 500x 2.6S (DebugRelease)
// Rev498(Schmörf) 500x 1.44S (DebugRelease O3)
// Rev504 1000x 2.8s (DebugRelease O3)
// Rev506 1000x 2.62s (DebugRelease O3)
// Rev508(Schmörf) 500x 1.36S (DebugRelease O3)
// Rev523(Schmörf) 1000x 5.76 (DebugRelease O3) + prioQueue test.
// Rev536 (Schmörf) 10000x 57.752 (DebugRelease O3) + prioQueue test.
// Rev537 (Schmörf) 10000x 56.562 (DebugRelease O3) + prioQueue test.

// 2009-11-15 (Schmörf) 1000x 5.5 (DebugRelease O3) + prioQueue test. 5.931.789 byte
// removeRef  inline 5.7 (Rel 1.093.120 Bytes )
// removeRef not inline 6.2 (Rel 920.064 byte)
// removeRef  half inline 5.3 (Rel (937.984 Bytes )
// assertParamCount half inline 5.2 (940.032 Bytes)
// assertType half inline (939.520)
//  setCallingObj inline 5.0 (939.520)
// Rev 585 2010-05-16 (Schmörf) 1000x 5.1 (DebugRelease O3) + prioQueue test.
// Rev 595 2010-06-03 (Schmörf) 1000x 5.1 (DebugRelease O3) + prioQueue test.
// Rev 596 2010-06-03 (Schmörf) 1000x 6.2 (DebugRelease O3) + prioQueue test + SuperconstructorTest
// neuer ref 6.2
// neuer RTB 6.1
// factory RTB ~6.2 + pool 6.0
// array cleanup 5.5
// array pool 1: 5.0?? ... 4.92
// new HMap -> LocalVarMap 5.0s -> 4.75s
// Operator redesign 4.75s -> 4.65s
// UserFunction Block init-fix 4.7s -> 4.55s
// Rev 691 2010-07-23 1000x 4.59 -> 4.75 (new testcase for sorting added)
if(benchmark){
	GLOBALS.out:=fn(values*){ ;};
	for(var i=0;i<1000;i++){
//		var t=Parser.parseFile("tests/Testcases_Core.escript");
		t.execute();
	}

}

//load("tests/Win32Lib.escript");
//load("tests/IOLib.escript");
loadOnce("tests/Bugs.escript");
//load("tests/Experiments.escript");
//print_r(getRuntime()._getAttributes());

GLOBALS.out=outBackup;
out("\nDuration:\t",clock()-start," sec\n");
