//
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
	out("BUG[20070327] \t",a.nr==6,"\n");
	//
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
	out("BUG[20070626] \t",a.f()==14,"\n");
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

	out("BUG[20070703] \t",
		(s1=="foo")&(s2=="foobar")&
		(i1==1)&(i2==2)&
		(o1.a==2)&(o2.a==2)&
		(b==2)&(a[0]==1),"\n");


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
    out("BUG[20071030] \t",void==null,"\n");
}
{
    //Bug BUG[20080210.1] no call by value with simple types
    var f=fn(x){
        x++;
    };
    var a=1;
    f(a);
    out("BUG[20080210] \t",a==1,"\n");
}
{ // MEMLEAK
//   Bug BUG[20080210.2] wrong Parsing of fn(){}something;
//   Does not create Syntax error
    out("BUG[20080210.2]\t");
    var errorFound=false;
    try{
        parse('var f=fn(){ out("!!");  } bla bla bla;');
    }catch(e){
        errorFound=true;
    }
    out(errorFound,"\n");
}
{// MEMLEAK
    out("BUG[20080229] \t");
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

    out(errorFound,"\n");
}
{
     // Calling object is not set properly when calling member function without this.
     out("BUG[20080324] \t");
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

     out(a.f2()==2,"\n");
}
{
     // Calling object is not set properly when calling member function without this.
     out("BUG[20090107] \t");
     parse("-1;"); // resulted in crash of the tokenizer when first token is a '-'
     out(true,"\n");
}
{
     // Linebreaks inside Strings are ignored in line counting.
    out("BUG[20090107.2] ");
    var l1=__LINE__;
    "line 1: dum di dum
    line 2:da da";
    var l2=__LINE__;
     out( (l2-l1)==3,"\n");
}
{
    // Wrong operator precedence.
    out("BUG[20090417] \t");
    out( 1.0/1.0*2.0==2,"\n");
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
    out("BUG[20090418] \t");
    var C=new Type();
    C.m1:=1;
    C.setObjAttribute("==",  fn(b){
        if(b---|>Number){
            return this.m1==b;
        }else return false;
    });
    var c1=new C();
    out(c1==1 && !(c1==c1) &&  c1!=c1 ,"\n");
}
{
    // Assignements to GLOBALS does not work.
    out("BUG[20090424]\t");
    GLOBALS.g1:=1;
    {
        g1=2;
    }
    out(g1==2,"\n");
}
{
    // Wrong operator precedence.
    out("BUG[20090525] \t");
    out( 1-2+3==1+3-2,"\n");
}
{
    // Array.append appends references of primitives instead of copies
    var a=[0,0,0];
    var b=[2,2].append(a);
//    print_r(b);
    a[0]++;
    out("BUG[20090714] \t",b[2]==0,"\n");
//    print_r(b);
}
{
    // Array.indexOf() does not use EScript.== operator
    var A=new Type();
    A.'==':=fn(other){
        return (other---|>getType());
    };
    var a=[0,new A(),1,2,3];
    out("BUG[20090907] \t",(a.indexOf(new A())==1)&&(!a.indexOf(4)) ,"\n");
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
    out("BUG[20090922] \t",i==4,"\n");
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
	out("BUG[20100604] \t",m1[3]==4,"\n");
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
    out("BUG[20100605] \t",errorFound,"\n");
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

	out("BUG[20100618] \t",errorFound,"\n");
}
{ // do...while with continue does not re-check condition;
	out("BUG[20100711] \t");
	var i=0;
	do{
		++i;
		continue;
	}while(i<3);
	out(true,"\n"); // if the bug exists, the loop never returns
}
{	// parser does not accept functions as default parameter
	out("BUG[20100717]\t");
	var ok=true;
    try{
        parse("fn(a=fn(){}){}; fn(a={1:2}){};");
    }catch(e){
        ok=false;
    }
	out(ok,"\n");
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
    out("BUG[20100815] \t",errorFound,"\n");
}
