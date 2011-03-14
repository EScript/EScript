var OK="\t ok\n";
var FAILED="\t failed\n";
{
	// Bug - Wrong Calling-Object when UserFunctionCall contains expression
	// Found 2007-03-27 fixed 2007-03-27


	GLOBALS.A:=new Type();
	A._constructor:=fn(i){
//	    out("Constructing an Object!\n");
		this.nr:=i;
	};

	var nr=2;
	var a=new A(nr*3);
	test( "BUG[20070327]",	a.nr==6 );
}
{
	// Bug[20070626] - Lost object namespace in Blocks inside functions
	// Found 2007-06-26

	var A=new Type();

	GLOBALS.f2:=fn(){
		out(this.a);
	};
	A.a:=1;
	A.f:=fn(){
		a++;
		this.a++;
		for(var i=0;i<3;++i)
		{
			a++;
			this.a++;
			{
				var a=0;
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
	var a=new A();
	test( "BUG[20070626]", a.f()==14 );

}
{
	// Bug[20070703] Value assignement "by reference" instead of "by value".


	var s1="foo";
	var s2=s1;
	s2+="bar";
	//out(s1,":",s2,"\n");

	var i1=1;
	var i2=i1;
	i2++;
	//out("\n",i1,"\n"); // ==1

	var o1=new ExtObject();
	o1.a:=1;
	var o2=o1;
	o2.a++;
	//out("\n",o1.a,"\n"); // ==2

	var a=[1,2,3];
	var b=a[0];
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
    while (true) {
    if (loader) {
        if (!loader.load(5)) {

            loader=false; // geht nicht
            GLOBALS.loader=false; // geht
        }
    */
};
{
    //Bug[20071030] null and void are not parsed correctly.
	test( "BUG[20071030]", void==null );
}
{
    //Bug BUG[20080210.1] no call by value with simple types

    var f=fn(x){
        x++;
    };
    var a=1;
    f(a);
	test( "BUG[20080210]", a==1 );

}
{ // MEMLEAK
//   Bug BUG[20080210.2] wrong Parsing of fn(){}something;
//   Does not create Syntax error
    var errorFound=false;
    try{
        parse('var f=fn(){ out("!!");  } bla bla bla;');
    }catch(e){
        errorFound=true;
    }
	test( "BUG[20080210.2]", errorFound );
}
{// MEMLEAK

    // this should create a runtime error (Block.+ not found) OR a syntax error .
    // (Not shure about what's better...)
    var errorFound=false;
    Runtime._setErrorConfig(Runtime.TREAT_WARNINGS_AS_ERRORS);
    try{
        parse('+out("");').execute();
    }catch(e){
        errorFound=true;
    }
    Runtime._setErrorConfig(0);

   	test( "BUG[20080229]", errorFound );
}
{
    // Calling object is not set properly when calling member function without this.

	var A=new Type();
	A.v:=1;
	A.f1:=fn(){
		 return this.v;
	};
	A.f2:=fn(){
		return f1(); // this should be equivalent to this.f1()
	};
	var a=new A();
	a.v=2;

   	test( "BUG[20080324]", a.f2()==2 );
}
{
    // Calling object is not set properly when calling member function without this.
    parse("-1;"); // resulted in crash of the tokenizer when first token is a '-'
   	test( "BUG[20090107]", true );
}
{
     // Linebreaks inside Strings are ignored in line counting.
    var l1=__LINE__;
    "line 1: dum di dum
    line 2:da da";
    var l2=__LINE__;
   	test( "BUG[20090107.2]", (l2-l1)==3 );
}
{
    // Wrong operator precedence.


   	test( "BUG[20090417]", 1.0/1.0*2.0==2 );
}
//{
//    // Parser does not realize forgotten ';' after function calls.
//    out("BUG[20090417.2]\t");
//    var f=fn(i){out(i);};
//
//    var errorFound=false;
//    try{
//        parse('f("a")out("b");');
//    }catch(e){
//        errorFound=true;
//    }
//    out(errorFound,"\n");
//}
{
    // '!=' is not automatically ! '==' as default

    var C=new Type();
    C.m1:=1;
    C.setObjAttribute("==",  fn(b){
        if(b---|>Number){
            return this.m1==b;
        }else return false;
    });
    var c1=new C();
	test( "BUG[20090418]", c1==1 && !(c1==c1) &&  c1!=c1 );
}
{
    // Assignements to GLOBALS does not work.

    GLOBALS.g1:=1;
    {
        g1=2;
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

    var A=new Type();
    A.'==':=fn(other){
        return (other---|>getType());
    };
    var a=[0,new A(),1,2,3];
	test( "BUG[20090907]", (a.indexOf(new A())==1)&&(!a.indexOf(4)) );
}

{
    // Array and Map cloning: Simple types are not copied but referenced.

    var i=0;
    var m1={ 'a':1 };
    var m2=m1.clone();
    if(m1==m2) i++;
    m1['a']++;
    if(m1!=m2) i++;

    var a1=[1];
    var a2=a1.clone();
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
	var m2=m1.clone();
	out("MemLeak[20100604] \n");
}
{ // MEMLEAK
	// assignInheritedAttribute not used in ExtObject

	Type.thisShouldntBeHere::=true;
	var A=new Type(ExtObject);
	var a=new A();

	var errorFound=false;
    Runtime._setErrorConfig(Runtime.TREAT_WARNINGS_AS_ERRORS);
    try{
        a.thisShouldntBeHere;
    }catch(e){
        errorFound=true;
    }
    Runtime._setErrorConfig(0);
	test( "BUG[20100605]", errorFound );

}
{	// Object member functions are accessible locally without an Object!

	var errorFound=false;
    Runtime._setErrorConfig(Runtime.TREAT_WARNINGS_AS_ERRORS);
    try{
        isSet; // this should not exist.
    }catch(e){
        errorFound=true;
    }
    Runtime._setErrorConfig(0);
	test( "BUG[20100618]", errorFound );
}
{ // do...while with continue does not re-check condition;

	var i=0;
	do{
		++i;
		continue;
	}while(i<3); // if the bug exists, the loop never returns
	test( "BUG[20100711]", i==3 );
}
{	// parser does not accept functions as default parameter

	var ok=true;
    try{
        parse("fn(a=fn(){}){}; fn(a={1:2}){};");
    }catch(e){
        ok=false;
    }
	test( "BUG[20100717]", ok );
}
{ 	// undeclared member attribute not detected

	var errorFound=false;
    Runtime._setErrorConfig(Runtime.TREAT_WARNINGS_AS_ERRORS);
    try{
	var A=new Type();
		A._constructor::=fn(){
			noMemberWithThisName = 1;
		};
		new A();
    }catch(e){
        errorFound=true;
    }
    Runtime._setErrorConfig(0);
	test( "BUG[20100815]", errorFound );
}
{	// assigning to an undefined member of a Type-Object results in a crash.
    Runtime._setErrorConfig(Runtime.TREAT_WARNINGS_AS_ERRORS);
	var ok=true;
    try{
        var a=new Type();
        a.foo = 5;
		FOO.bla = 5;
    }catch(e){
    }
    Runtime._setErrorConfig(0);
	test( "BUG[20110217]", ok );
}

{ 	// endless recursion does not throw an exception but results in a crash

	var errorFound=false;
	var oldLimit = Runtime._getStackSizeLimit();
	Runtime._setStackSizeLimit(Runtime._getStackSize()+20);
    var i=0;
    try{
		var f=i->fn(){
//			out(Runtime._getStackSize()," ");
			++this;
			(this->thisFn)();
		};
		f();
    }catch(e){
        errorFound=true;
    }
//    out("\n",i,"\n");
    Runtime._setStackSizeLimit(oldLimit);
	test( "BUG[20110314]", errorFound && i>17 && i<22);

}
