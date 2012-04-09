//var f =fn(){ 
//	out("!!!");
//	Runtime.exception("foo");
////	1;
////	throw("foo");
//};
////	
//out( (1->f)());
//exit;
(fn(){})._asm();
// Testcases for HasEScript Scripting-Language
out ("EScript Testcases\nVersion: ",EScript.VERSION_STRING,"\n","-"*79,"\n");


	
//
//var v;
//out(v);
////{
////	var start = clock();
////
////	{
////		for(var i=0;i<1000000;++i){		var b = i+1;	}
////	}
////	// 1.19s
////	out(clock()-start,"\n");
////}
////{
////	var fun = _compile( "for(var i=0;i<1000000;++i){		var b = i+1;	}");
////	var start = clock();
////
////	{
////		fun._doCall();
////	}
////	// 1.09s
////	out(clock()-start,"\n");
////}
////exit(0);
//
//{
//	var expressions = [
////		"for(var i=0;i<10;++i){ var a=i%2; if( a==0 ) continue; {var b = i; out (b); if(i>5) break;  } }",
////		"var a='Excpetion caught:'; try{ var b=2;{var c=3/0; {var d=4;} true; } 17; }catch(e){out(a,e,'\\n');}",
////		"while(true){ try{ var a=0; continue; }catch(e){out(e);} }",
////		"true ? 'foo' : 'bar'; ",
////		"var f = fn(b){ var a=2; out('KatzE '*(a*b));}; f(3);",
////		"var f = fn(b){ return ; out('!!!!!');}; out(f(3));",
////		"var f = fn(b){ return b*b; out('!!!!!');}; out(f(3));",
////		"var a = []; (a->fn(b*){append(b);})(1,2,3); print_r(a); ",
////		"var a = [-100]; foreach( (a->fn(b*){append(b);return this;})(1,2,3) as var key,var value) out(key,'::',value,'\t');  ",
////		"var A = new Type(); A._constructor ::= fn(m){this.m:=m; out('!!!');}; var a = new A(1); out(a.m); out(A.m);"
//
//		
////		"var A = new Type(); A._constructor ::= fn(p){out('\\n',this,'AAAA\\n');this.n:=''+p+'foo';};	"+
////		"var B = new Type(A);B._constructor ::= fn(p)@(super(p+100)){out('\\n',this,'BBB\\n');n+='bar'+p;}; "+
////		"var C = new Type(B);C._constructor ::= fn(p)@(super(p+10)){out('\\n',this,'CCC\\n');n+='blub'+p;}; "+
////		"var c = new C(1); out(c.n,'\n'); ",
//		
////		" (fn(i){ if(i%1000 ==0)out(i,' '); return i>0 ? i+thisFn(i-1) : 0 ;})(1000000);"
//
//
////		"(fn(){exit( 77 );})(); out('FOO');",
////		"(fn(a){out(a);})(true);",
////		"(fn(a){out(a);})(true);"
////		"(fn( [Number,Bool,'a'] a,false b){})('a',true);" ,
////		"(fn( Number a){})('a');"
////		"try{ (fn(){throw 'foo!';})(); }catch(e){out(e);}",
////		"var f=new ExtObject(); f._call := fn(obj,params*){out('this:',this,'\n');out('obj:',obj);print_r(params);};  ('bar'->f)(1,2,3);"
////		"var b=17;var a=7+b; out(a);",
////		"var f = fn(a){ out(a);}; f( 3*3 );",
////		"var f = fn(a,b=2){ out(a*b);}; f( 3 );",
////		"var f = fn(a,b=2+a){ out(a*b);}; f( 3 );",
////		"var f = fn(a,b){ out(a*b);}; f( 3,4,5 );",
////		"var f = fn(a,b*){ out(a*b.count());print_r(b);}; f( 3,1,2,3,4 );",
////		"var f = fn(a='foo',b*){ out(a);print_r(b);}; f(  );",
////		"var f = fn(a=1,b=2,c=3){ out(a+b+c);}; f(,10  );", // does not work!
////		"new ExtObject();",
////		"print_r([1,2,3]);",
////		"print_r( {'a':'foo',2:$bar});",
////		"foreach( [1,2,3] as var v) out('####',v);",
////		"var v; foreach( [1,2,3] as var k,v) out(k,':',v,'\n');",
////		"foreach( {$foo : 1,$bar :2} as var k,var v) out(k,':',v,'\n');",
//		
////		"var f = fn(){ out(1);yield 1;out(2);yield 2;out(3);yield 3;out(4);}; var i=f();out('.\\n',i.next(),i.end(),'.\\n',i.next(),i.end(),'.\\n',i.next(),i.end());",
//		"foreach( (fn(){ var a =10 ;yield a; ++a;yield a; ++a;yield a;++a;return a;})() as var key,var value) { out(key,':',value,'\n'); } ",
//
////		"var v; out('(',v,')');",
////		"return 27+17;",
////		"out('Hello HasE!');",
//////		"for(var i=0;i<10;++i){	out('foo',i,'\n'); }",
////		"a.b();",
////		"a.b.c();",
////		"a+b;",
////		"(a.b())();",
////		"if(true) 'yes';",
////		"if(a) 'yes'; else if( b ) ; else 'no';",
////		"a||b;",
////		"a||b||c;",
////		"a&&b;",
////		"!a;",
////		"var a; { var b; }",
////		"var a; { var a; {var a;}} {var a;}",
////		"var a = 1;{var b = 2;}",
////		"var a = var b = 2;",
////		"var a = out; a('Foobel');",
////		"var a=4; { var b=17; }",
////		"a = var b;",
////		"a.b = 5;",
////		"foo().b = true;",
////		"a.b := true;",
////		"a.b ::= true;",
////		"a.b @(private) ::= true;",
////		"while(false);",
////		"do{17;}while(false);",
////		"for(var i=0;i<10;++i){	var a = i;}",
//////		"foreach(arr as var key,var value){	value; }",
//	];
//
//
//	foreach(expressions as var expression ){
//		out("\n","-"*30,"\n[",expression,"]\n");
//		var fun = _compile( expression);
//		out( fun._asm() );
//		var result = Runtime._callFunction2( fun );
////		var result = fun._doCall();
//		out( "\n",result );
//
//	}
//
//
//	exit(0);
//}
//
//{
//	var v;
//	out(v);
//}






//----
// init
GLOBALS.benchmark:=true;
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
