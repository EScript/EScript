var OK="\t ok\n";
var FAILED="\t failed\n";
//---
{
	out("\nBinary expressions: ");
	var a=(1.0+2)*3/(20-(1+(1)))+0.5+-1+1;
	if(	a==1 ) out(OK); else { errors+=1; out(FAILED); }
}
//---
{
	out("Bit Operations:\t" );
	var c=(2|3|255&1040) ^ 33;
	if(c == 50){out (OK);}else { errors+=1; out(FAILED); }
}
//---
{
	out("Not Operation:\t" );
	if( !(2!=1+1) && !(!(2==2)) ){out (OK);}else { errors+=1; out(FAILED); }
}
//---
{
	out("Assignment Operations:" );
	var a=0;
	a+=10;
	a*=5;
	a/=2;
	a%=17;
	var c;
	var b=c=3;

	if( a==8 && b==3 && c==3){out (OK);}else { errors+=1; out(FAILED); }
}
// ---
{
    out("Number Funktions:");
	var n=1;
	(fn(value){value++;})(n); // test call by value
    var i=(9).sqrt();
    
    out( 3.7 % 0.7 );
    if(n==1 && i==3 && (0x01+255).toHex()=="0x100" && "-1.7".toNumber()==-1.7 && 1.getType()==Number
			&& (180).degToRad()== Math.PI && (Math.PI.radToDeg()-180).abs() < 0.001
			&& 100.log()==2 && (128).log(2)==7 && 1.sign()==1 && -2.3.sign()==-1
			&& Math.PI.format(4,false,10,"-")=="----3.1416" && Math.PI.format(5,true).beginsWith("3.14159e+00" )
			&& 1.clamp(2,3)==2 && 17.clamp(-2,20)==17 && 9.clamp(1,1.6)==1.6
			&& (180).degToRad().radToDeg().matches(180) && !(179.9999).degToRad().radToDeg().matches(180)
			&& (0.1+0.1+0.1) ~= 0.3 && !(0.9999999 ~= 1.0)
			&& (3.7 % 0.7) ~= 0.2 && (-2 % 3) == -2
		)
        {out(OK);}else{ errors+=1; out(FAILED); }
}
//---
{
	out("Strings:\t" );
	var mystring="bl\"#"+2;
	(fn(value){value+="should do nothing";})(mystring); // test call by value

    var s="foobar";
	var spacy = "\t   bla  \n\r  ";

	if(mystring*3 == "bl\"#2bl\"#2bl\"#2" && !s.endsWith("\0")
        && s.endsWith("bar")&&!s.endsWith("b")&&s.beginsWith("foo")&& s.beginsWith(s)&&  !s.beginsWith(s+s)
        && s.contains("ob") && !s.contains("oc") && "f".getType()==String
		&& "a,b,c".split(",",2) == ["a","b,c"] && "/".split("/") == ["",""]
        && "bla".fillUp(10,'.') == "bla......."
		&& spacy.lTrim()== "bla  \n\r  " && spacy.rTrim()=="\t   bla" && spacy.trim()=="bla" && "".trim().empty();
		)
        out(OK); else { errors+=1; out(FAILED); }
}
//---
{
    out("Void:\t\t");
    var v=void;
    if( void==v && v!=false && false==v && v!==false && v===void && void != 0 && 0==void && !((1)===void) && !(void===0)
    // strange, but ok. (0).==(void.toNumber())
    && 0 == void)
        out(OK); else { errors+=1; out(FAILED); }
}
//---
{
	out("Increments&&Decrements:" );
	var a=0;
	//  out(a--);
	//	out(a++==0) ;
	if( a++ == 0 && a==1 && a--==1 && a==0 && ++a==1 && --a==0)
	out(OK); else { errors+=1; out(FAILED); }
}
//---
{
	out("While: \t\t");
	var a=0;
	while(a<8){a=a+1;}
	while(a>5){a--;}
	var b=0;
	var c=0;
	while( true){
		if(++b%2 != 0) {{continue; }}
		if(++c==5) break;
	}
	var d=0;
	do{d++;}while(d<0);
	if(a == 5 && b == 10 && c==5 && d==1){out (OK);}else { errors+=1; out(FAILED); }
}
//---
{
	out("for: \t\t");
	var a=0;
	var i=0;
	for(i=5;i<10;){
		a+=i;
		//out(a,"-",i,"\n");
		i++;
	}
	for(i=10;i>5;i--){
		if( (i%2) >0) continue;
		if(i<7) break;
		a+=i;
		// out(a,"-",i,"\n");
	}
	// a==53
	var j=0;
	for(var j=0;j<5;++j)
		a+=j;
	// a==53 + 0+1+2+3+4==63

	// this $a is a different local variable and not touched.
	for(var a=0;a<5;++a){
		var a="foo";
	}

	// out(a);
	if(a == 63 && j==0 ){out (OK);}else { errors+=1; out(FAILED); }
}
//---
{
	out("Blocks:\t\t");
	var v1=1;
	{
		var v1=2;
		{
			v1=v1+1;
		}
	}
	{false?v1--:true?v1++:v1--;}
	if(v1==2){out (OK);}else { errors+=1; out(FAILED); }
}
//---
{
	out("Bool operators:\t");
	var b=false;
	(fn(value){value|=true;})(b); // test call by value

	var a=0;
	a>0 || (a=a+1)>0 && (a=a+3)>0 || (a=0)>0;
	a==7 && (a=10)>0;
	a==4 && (a=5)>0;
	if( a==5 && ! false && !b)
	{out (OK);}else { errors+=1; out(FAILED); }
}
//---

{
	out("substr:\t\t");
	var file="Dibbeldu.txt";
	if( file.substr(-3)=="txt" && file.substr(9)=="txt" && file.substr(1,1)=="i")
	{out (OK);}else { errors+=1; out(FAILED); }
}
{	// User Functions:
	//global plusRec;
	var a=10;
	GLOBALS.plusRec := fn (a,b){
		if(a<=0) return b;
		return plusRec(a-1,b+1);
	};
    var plusRec2= fn(a,b){
		if(a<=0) return b;
		return thisFn(a-1,b+1);
	};

	var minusOne=fn(a){
//		out(getRuntime().getStackInfo());
		return a-1;
	};

	var increase=fn(a,b=1){
        return a+b;
	};
    var repeat=fn(Number n,String s="-"){
        return (s*n);
    };
	var typeException=false;
	try{
	    repeat(true);
    }catch(e){
        typeException=true;
	}

	// multiParam
	var mulSum=fn(factor,Number values*){
	    var a=0;
	    foreach(values as var v) a+=v;
	    return factor * a;
	};
	var typeException2=false;
	try{
        out(mulSum(2,"foo"));
	}catch(e){
        typeException2=true;
	}

	// multi possibilities
	var f=fn( [Number,"bla"] p1,[Number] p2=7,[1,2,3] p3* = 3){

	};
	f("bla",3);
	var typeException3=false;
	try{
        f("bla",3,4);
	}catch(e){
        typeException3=true;
	}

	// skipped parameter
	var f2=fn(a=1,b=2,c=3){
		return a+b+c;
	};
//	var x=new Array(1,,,4);
//	print_r(x);

	var f3=fn(){
		if(!thisFn.isSet($staticVar))
			thisFn.staticVar:=0;
		thisFn.staticVar+=1;
		return thisFn.staticVar;
	};

	test("User Functions:",
		plusRec(a,7)==17 && plusRec2(a,7)==17 && minusOne(a)==9 && (fn(a){return a*a;})(2)==4
        && increase(3)==4 && increase(3,2)==5 && typeException==true && repeat(3,".")=="..."
        && mulSum(2,1,2,3)==12 && typeException2 && typeException3
        && f2(,,10)==13
        && f3()==1 && f3()==2 // a static counter is increased each time f3 is called
	 );
	 f3.staticVar=0; // reset staticVar for next testing loop.
}
//{
//	out("Lambda Functions:");
//
//    var sum1=0;
//    var sum2=0;
//
//	var a=10;
//	var sumA=lambda(b){
//        a+b;
//	};
//throw(" FOOO ");
//    {
//        var a=1;
//        sum1=sumA(7); // 7+1=8
//    }
//    sum2=sumA(7); // 7+10=17
//    var c=8;
//    var d=3;
//    var sum3 = (lambda(){ if(d>0){ c++;d--;thisFn();}else c;})(); // 8+3=1
//
//    if(sum1==8 && sum2==17 && sum3==11)
//	{out (OK);}else { errors+=1; out(FAILED); }

//}
//---
{
	out("Collections:\t");
	// element access
	var O=new Type();
	O._get::=fn(index){
		return index*2;
	};
	var o=new O();
	if(o[100]==200)
		{out (OK);}else { errors+=1; out(FAILED); }
}
//---
{
	out("Array:\t\t");
	var a=new Array(17,27,1000);
	a.pushBack("foo","bar");
	a.pushBack("ding");
    a.removeIndex(2); // remove 1000

	var accum="";

	a._set(2,"Hoobel");

	a[0]=a[0]+1;

	var i=a.getIterator();
	while(!i.end()){
		accum+=i.value();
		i.next();
	}


	var keyOfHoobel=fn(a){
		var key;
		var value;
		foreach(a as key,value){
			if(value==18) continue;
			// out("\n",key,":",value,"\n");
			if(value=="Hoobel") return key;
		}

	};
	i.reset();
	accum+=i.value();

	accum+=a._get(a.count()-2);
	a.pushBack(["dum","di"]);

	a[a.count()-1][100]="du";
	accum+= a[ a.count()-1 ][1];  // + di
	accum+= a[ a.count()-1 ][100];  // + du


	//out(accum);
	// accum=="1827Hoobelbarding18bardidu"

	var b=new Array(1,2);	// b: 1,2
	var c=b.clone();		// c: 1,2
	b.pushBack("foo");		// b: 1,2,foo
	c.append([3,4]);		// c: 1,2,3,4
	b.swap(c);				// b: 1,2,3,4 c: 1,2,foo

//	var ar=[3,23,7,3,100,1,35].max()==100;
//	out(ar.max());
	//print_r(ar.sort());
	//print_r(a);
	//out(a.indexOf("ding"));
	var e=[3,23,7,3,100,1,35];

	var q=new ExtObject(); // used for sorting by the distance from 59
	q.c:=59;
	if( accum=="1827Hoobelbarding18bardidu" && a ---|> Array && ! (1 ---|> Array)
			&& (new Array(1,'a','b')).implode(',')=='1,a,b'
			&& b.implode()=='1234' && c.implode()=='12foo'
			&& keyOfHoobel(a)==2 && [3,23,7,3,100,1,35].rSort()==[100,35,23,7,3,3,"1"]
			&& [3,23,7,3,100,1,35].sort( q->fn(a,b) {return (a-c).abs()<(b-c).abs(); }) == [35,23,100,7,3,3,1]
			&& ["x","bla",2,"dum"].sort()==["bla","dum","x",2]
			&& [1,2,3].map(fn(k,v){return k*v+1;})==[1,3,7]
			&& [1,2,3].map(fn(k,v,x){return x+k+":"+v;},"#")==["#0:1","#1:2","#2:3"]
			&& a.indexOf("ding")==4 && a.indexOf("ding",4)==4 && !a.indexOf("ding",5)
			&& !a.indexOf("dadong")
			&& [].pushFront(1).pushFront(2).pushFront(3).pushFront(4)==[4,3,2,1]
			&& (var x=[1,2,3]).popFront()==1 && x==[2,3]
			&& [1,2,3,4,5].reverse()==[5,4,3,2,1] && [1,2,3,4].reverse()==[4,3,2,1]
			&& [1,2,3,4,5,6,7,8,9].filter(fn(a){return (a%2)==0;})==[2,4,6,8]
			&& [1,2,3,4,5,6,7,8,9].filter(fn(a,b,c){return (a%b)==c;},3,0)==[3,6,9]
			&& e.max()==100	&& [4,2,-3,7].min()==-3
			&& e.contains(7)
			&& !e.contains(4)
			&& e.get(7)==void
			&& e.get(7,"notFound")=="notFound"
			&& e.get(0,"notFound")==3
			&& e.findValue(23)==1
			&& !e.findValue(4)
			&& e.front() == 3
			&& e.back() == 35
			&& ! ([].back())
			&& ! ([].front())
			&& e.reduce(fn(sum,key,value){return sum+value+key;},0) == (172+0+1+2+3+4+5+6)
			&& e.reduce(fn(accum,key,value,delimiter){return (accum ? accum+delimiter : "")+key+":"+value; },false,",") == "0:3,1:23,2:7,3:3,4:100,5:1,6:35"
			&& [1,2].clear().empty()
			&& [1,2,2,2,3,2,4].removeValue(2) == [1,3,4]
			&& [1,2,2,2,3,2,4].removeValue(2,3) == [1,3,2,4] // three times
			&& [1,2,2,2,3,2,4].removeValue(2,3,2) == [1,2,3,4] // three times, beginning from position 2
			)
	{out (OK);}else { errors+=1; out(FAILED); }

}
//---
{
	out("Map:\t\t");
	var m=new Map();
	m["foo"]="bar";
	m["bla"]="da";
	m["dum"]=m["bla"];

	var accum="";
	var i=m.getIterator();

	while(!i.end()){
		accum+= "|"+i.key() +":" +i.value()+m[i.key()]+"";
		i.next();
	}
	accum+=m.count();

	var m1=new Map();
	m1["foo"]="bar";
	m1["bla"]=[1,2,3];

	var m2={1:2,3:4};
	m1.swap(m2);


	if( accum=="|bla:dada|dum:dada|foo:barbar3"
			&& m2=={"foo":"bar","bla":[1,2,3]}	&& m2!={"foo":"bar","bla":[1,2,3,4]}
			&& m1=={1:2,3:4} && m1.containsKey("1") && !m1.containsKey(5)
			&& ({:} ---|> Map) && ! (({})---|> Map )
			&& {"a":"b","c":"d",1>2?"x":"y":"foo"}.map(fn(k,v){return k+v;}) =={"a":"ab","c":"cd","y":"yfoo"}
			&& {1:2,3:4,5:6}.map(fn(k,v,i){return k+v-i;},1) == {1:2,3:6,5:10}
			&& {"a":1,"b":2,"c":3}.unset("b")=={"c":3,"a":1}
			&& {"foo":"bar",1:2}.merge({3:4,1:"dum"})=={"foo":"bar",1:"dum",3:4}
			&& {"foo":"bar",1:2}.merge({3:4,1:"dum"},false)=={"foo":"bar",1:2,3:4}
			&& {8:5,1:2,3:4}.max()==5
			&& {1:2,3:4,5:6}.get(7)==void
			&& {1:2,3:4,5:6}.get(7,"notFound")=="notFound"
			&& {1:2,3:4,5:6}.get(3,"notFound")==4
			&& {1:2,3:4,5:6}.reduce(fn(sum,key,value){return sum+value+key;},27) == (27+1+2+3+4+5+6)
			&& {1:6}.clear().empty()
			&& {1:2,3:4,5:6,7:8}.filter( fn(key,value){ return key!=3 && value!=8; }) == {1:2,5:6}
			)
	{out (OK);}else { errors+=1; out(FAILED); }
}
//---
{
	out("try/catch/throw:");
	var i=0;
	try{
	//	out("FOOBA!");
		{{{1/0;}}}
		i+=1;
	}
	catch(){
//		out("\n",e);
	//	out("\nZeile:",__LINE__);
		i+=2;
	}
	try{
		throw 4;
	}catch(e){
		i+=e;
	}
	var f=fn(){
		var e=new Exception("foo");
		e.value:=8;
		throw e;
	};
	try{
		f();
	}catch(e){
		if(e---|>Exception)
			i+=e.value;
	}
	if( i==14)
	{out (OK);}else { errors+=1; out(FAILED); }
}
//
// ---
if(!benchmark)
{//disble for benchmarks
	out("load:\t\t");
	GLOBALS.loadTestVar:=0;
	var r=load(__DIR__+"/loadme.escript");
	var r2=loadOnce("loadme.escript");
	var r3=loadOnce(__DIR__+"/loadme.escript"); // should be the same file as in the previous line as __DIR__ is in the searchPath

//    out(__FILE__,"\t",__DIR__);
	if( __FILE__==__DIR__+"/Testcases_Core.escript"&&  testFunction(3)==9 && r==5 && r2==5 && r3===void && loadTestVar==2)
	{out (OK);}else { errors+=1; out(FAILED); }
}
//---
if(GLOBALS.isSet($TestObject)){  // TestObject is defined in test.cpp
	var t=new TestObject(1,1); 
	t.m1=9; // int
	t.m2=9; // float
	t.m1+=1.7; // (int) (9 + 1.7) == 10
	t.m2+=1.7; // 9+1.7 == 10.7
	t.m1++;
	t.m2++;

	var m2=t.getM2();

	test("NumberRef", t.getM1()== 11 && t.getM2().matches(11.7) );
}
//---
if(!benchmark)
{
 //disble for benchmarks
   out("Get-/setAttribute:");
   var normalPlus=GLOBALS.Number.getAttribute("+");
   Number.setObjAttribute("+",fn(i){
        return this-i;
   });
   var a=1+2; // = 1-2;
   Number.setObjAttribute("+",normalPlus);
   var b=1+2; // = 1+2//
 /// Fakultät
   Number.setObjAttribute("!_post",fn(){
       if(this<=1) return this;
       return this* ((this-1)!);
   });
   var c=7!; // 7*6*5*4*3*2*1
   if( a==-1 && b==3 && c==5040 && (1)."+"(2)==3 )
        {out (OK);}else { errors+=1; out(FAILED); }
 /// Note: 2008-02-11 When setting Attributes of Type-Objects, Debug-Object-Counting
 /// does not work properly until deleting new Attributes: \todo !!!check this
    Number.setObjAttribute("!_post",void);
/// Todo:    Number.unsetAttribute("!_post");
}

//---
{
    out("Inheritance:\t");
    var A=new Type(ExtObject);
    A._constructor:=fn(){
//        this.a:=getType().a;
//        out("FOOO",this,"\n");
		this.f:=1;
    };
    A.a:=1; /// object-member of A
    A.b:=1; /// object-member of A
//    A.setTypeAttribute("c",1); /// class-member of A \TODO????????????????????????
	A.c::=1;
    A.setObjAttribute("d",1); /// object-member of A
    A.e::=1; /// class-member of A
//    print_r(A._getAttributes());
	var a=new A();
//	print_r(a._getAttributes());
//	print_r(A._getAttributes());
//	print_r(A.getObjAttributes());
//	print_r(A.getTypeAttributes());

    var B=new Type(A); ///
    B._constructor::=fn(){
		f++;
    };
//    print_r(B._getAttributes());
//    out("\nA.a: ",A.a," A.b: ",A.b," A.c: ",A.c," A.d: ",A.d," B.a: ",B.a," B.b: ",B.b," B.c: ",B.c," B.d: ",B.d," \n");
    B.a++;
    B.b++;
    B.c++;
    B.d++;
    B.e++;

	var b=new B();
//	print_r(b._getAttributes());

//    print_r(B._getAttributes());
//    out("A.a: ",A.a," A.b: ",A.b," A.c: ",A.c," A.d: ",A.d," B.a: ",B.a," B.b: ",B.b," B.c: ",B.c," B.d: ",B.d," \n");
    var C=new Type;
    C.m1:=1;
    C."==" :=  fn(b){
        if(b---|>Number){
            return this.m1==b;
        }else return null;
    };

    var c1=new C();
//    out(c1==1);
//    out(c1==2);
//    out(c1!=2);

    if( A.a==1 &&A.b==1 &&A.c==2 &&A.d==1 &&A.e==2 &&B.a==2 &&B.b==2 &&B.c==2 &&B.d==2&&B.e==2
    && B.getType()==Type && B.getBaseType()==A && (new B()).getType()==B && (new B()).getType().getBaseType()==A
	&& (new B()).f==2 && c1==1 && !(c1==2) && c1!=2  )
        {out (OK);}else { errors+=1; out(FAILED); }

//    {
//        var A=new Object();
//        A.setClassMember("a",1);
//
//        var B=new A();
//        /// this should produce a warnnig
//        B.a=2;
//        out(A.a,"\t",B.a); /// should be 1 2
//    }
}
//---
{
    out("Superconstructor:");
 	var A=new Type(ExtObject);
	A._constructor::=fn(a,p*){
		this.m1:=a;
		foreach(p as var v){
			m1-=v;
		}
	};
	var A2=new Type(A); // useless in-between type whithout constructor
	var B=new Type(A2);
	B._constructor::=fn(v,x,y).(v*2,x,y){
		m1=-m1;
	};
	var C=new Type(B);
	var a=new A(100,30,20); // 100-30-20
	var b=new C(100,30,20); // -(100*2-30-20) = -150
	var c=new C(100,30,20);

	if( a.m1==50 && b.m1==-150 && c.m1==-150)
        {out (OK);}else { errors+=1; out(FAILED); }
}
//---
{
    out("Parser:\t\t");
    var s=(new Parser()).parse("while(a<10)a++;");
    var a=0;
    s.execute();
    //out(a);
    if( a==10)
        {out (OK);}else { errors+=1; out(FAILED); }
}
//---
{
    out("getDate:\t");
//    out(time().toHex(),"\n");
//    out(time().toIntStr(),"\n");
//    var d=getDate(time());
//    out("year-mon-mday hours:minutes.seconds".replaceAll(d));
//    print_r(d);

    if( "year-mon-mday hours:minutes.seconds".replaceAll(getDate(0x47f10600)) == "2008-3-31 17:40.48")
        {out (OK);}else { errors+=1; out(FAILED); }
}
//---
{
	out("Identifier/isSet:");
	var a=new ExtObject({ $foo2:2 });
	a.foo:=1;
	var t1=a.getAttribute($foo);
	a.$foo=2;

	if( t1==1 && a.foo==2 && a.isSet($foo) && a.isSet('foo') &&	!a.isSet('bar') && a.foo2==2 
		&& $a != "a" && $a !== "a" && "a" !== $a && "a" == $a 
		&& $a ---|> Identifier && $a == new Identifier("a") && $a!=$b )
        {out (OK);}else { errors+=1; out(FAILED); }
}
//---
{
    out("Delegate:\t");
    var a=new ExtObject();
    a.m1:=1;
    a.f:=fn(Number p1){
        return this.m1+p1;
    };
    var d=new Delegate(a,a.f);

    var d2=a -> fn(){return this.m1;};

    if(d(3)==4 && d2()==1 && d2.getObject()==a && d.getFunction()==a.f )
        {out (OK);}else { errors+=1; out(FAILED); }
}
//---
{
    out("JSON:\t\t");
    var list=[getDate(),{"foo":true,"bar":false,"dum":void},-1];
    var jsonList=toJSON(list,false);
//    out("\n",jsonList);
//    print_r(list);
//    print_r(parseJSON(jsonList));

    if(list==parseJSON(jsonList))
        {out (OK);}else { errors+=1; out(FAILED); }
}
// ---
{
    out("PrioQueueTest:\t");

    var PriorityQueue=new Type(Collection);
    PriorityQueue._constructor:=fn(comparison='<',data=void){
        this.comparison:=comparison;
        this.a:=[];
        if(data){
            foreach(data as var v)
                add(v);
        }
    };
    PriorityQueue.compare:=fn(a,b){
        return (a -> a.getAttribute(comparison)) (b);
    };
    PriorityQueue.add:=fn(e){
        a.pushBack(e);
        var i=count()-1;
        while(i>0){
            var p=( (i+1)/2).floor()-1;
            if( compare(e,a[p])){
                var t=a[p];
                a[p]=e;
                a[i]=t;
            }else{
                break;
            }
            i=p;
        }
    };
    PriorityQueue."+=":=PriorityQueue.add;
    PriorityQueue.count:=fn(){
        return a.count();
    };
    PriorityQueue.clear:=fn(){
        return a.clear();
    };
    PriorityQueue.swap:=fn(i,j){
        var tmp=a[i];
        a[i]=a[j];
        a[j]=tmp;
    };
    PriorityQueue.extract:=fn(){
        if(count()==0)
            return void;
        var min=a[0];
        if(count()>1){
            a[0]=a.popBack();
            heapify(0);
        }else a.popBack();
        return min;
    };
    PriorityQueue.heapify:=fn(i){
        var left=((i+1)*2)-1;
        var right=left+1;
        var minI=i;
        if(left>=count())
            return;
        if( compare(a[left],a[minI]) )
            minI=left;
        if(right<count() && compare(a[right],a[minI]))
            minI=right;
        if(minI!=i){
            swap(i,minI);
            heapify(minI);
        }
    };
    var a=[83,5,2,756,23,8,3,43,1,74,1,78,3,45,7,123,3];
    var q=new PriorityQueue('<',a);
    var a2=[];
    while(var v=q.extract()){
        a2+=v;
    }
    a.sort();
    if(a==a2)
        {out (OK);}else { errors+=1; out(FAILED); }


//    for(var i=0;i<100;i++){
//        q+=Rand.equilikely(0,100000);
//    }

}
{
	var f1=fn(end){
		var v=1;
		for(var i=0;i<end;++i){
			yield v;
			v*=2;
		}
	};

	var a=[];
	for(var it=f1(5);!it.end();it.next()){
		a+=it.value();
	}
//	out(a==[1,2,4,8,16]);

	var s="";
	foreach(f1(3) as var i,var j) s+=""+i+":"+j+" ";

	// ---
	// yield and return
	var f2 = fn(){
		yield 1;
		yield 2;
		return 3;
	};
	var s2="";
	foreach(f2() as var value) s2+=value;

	var s3="";
	{
		var it=f2();
		for(;!it.end();it.next())
			s3+=it.value();
		s3+=it.value();
	}

	test("yield:",
		a==[1,2,4,8,16]
	&&  s=="0:1 1:2 2:4 "
	&& s2=="12"
	&& s3=="123"
	);
	
	
}
//out("\n");
