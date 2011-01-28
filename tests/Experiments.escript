{
    out("\nDiverses...\n");
    out( ({}) != ({}));
    (5)*3;
    out( {:} ---|> Map) ;
    print_r({:});
   // out( {5:1} * fofsdfsfsdf);
   out( ("hallo").hash().toHex(),"\n" );
   out(system("echo foo"));

    // ----

    GLOBALS._Meta=new Object();
    _Meta.DESCRIPTION="Description";
    _Meta.AUTHOR="Author";
    _Meta.NOTE="Note";
    _Meta.getMetaInf=fn(c,memberName){
        var m=c.getAttribute("_Meta");
        var s=memberName+"("+c.getAttribute(memberName).getTypeName()+")";
        if(!m) return s;
        var info=m.getAttribute(memberName);
        if(!info---|>Map) return s;
        s+="\n";
        foreach(info as key,value){
            s+=" "+key+":\t"+value+"\n";
        }
        return s;
    };

    /***
     ** MyClass
     **/
    var MyClass=new Object();
    MyClass._Meta=new Map();

    /**
     * void f1()
     */
    MyClass._Meta.f1 = {"Description":"Do something",
                        "Author":"Claudius"};
    MyClass.f1=fn(){
            out("do do");
    };
    MyClass._Meta.v1 = {_Meta.DESCRIPTION:"Some silly variable",
                        _Meta.AUTHOR:"Claudius",
                        _Meta.NOTE:"not used..."};
    MyClass.v1=5;

    out("\n",_Meta.getMetaInf(MyClass,"v1"));
    out("\n",_Meta.getMetaInf(MyClass,"f1"));


//    class MyClass2 ---|> Object{
//        _Meta={:};
//        _constructor = fn(){
//        };
//
//        _Meta.v1={  _Meta.DESCRIPTION:"Some other silly variable",
//                    _Meta.AUTHOR:"Bärnhelm",
//                    _Meta.NOTE:"Dum di dum.."};
//        v1=5;
//
//    };
   //out(system("notepad.exe"));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------------
/*
{
	var A=new Object();
	A._constructor=function(){
		out("!A!");
		this.foo=0;
	//	return this;
	};
	var a=new A();
	var b=new A();

	a.foo=1;
	b.foo=2;
	out(a.foo);
	out(b.foo);

}
{
//	for(var i=0;i<100;i++){
//		out("\n",Math.rand(20));
//	}
}
{
	out("\nString Ops:");
	var s="foobarbladumfoo";
	out(s.find("bar"));
	out(s.find("bars"));
	out(s.find("bars")===false);
	out(s.find("foo")===false);
	if(s.find("foo")!==false) out (" did find foo ");
	out(s.rfind("foo"));
	out("\n","Der gelbe Hund".replace("gelbe","gruene große"));
	out("\n","aaa".replaceAll("a","aax",2));
	out("aaa".replaceAll("a","aax",2)=="aaxaaxa");
	print_r("bla|blubb||dings".split("|"));
	print_r("bla|blubb||dings||".split("||"));
	out("\n");
	out("aablaxfoodumdidumbla".replaceAll(_{"bla":"foo","foo":" FOO "}));
}
return "OK";
/*
*/
{
    var b=Parser.parse("out('Hallo!');");
    b.execute();
}
//{
//    var b=Parser.parseFile("tests/Testcases_Core.escript");
//    for(var i=0;i<10;i++)b.execute();
//}
//{
//    class A{
//        static _constructor:fn(bla){
//
//        }
//        static doSomething:fn(bla){
//            out(bla);
//        }
//        member fob;
//        member fab;
//
//    };
//}
