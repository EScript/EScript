{

	var r1 = new Math.RandomNumberGenerator();
	var r2 = r1.clone();
	var r3 = new Math.RandomNumberGenerator();
	r3.setSeed(17);

	var a1=[];
	var a2=[];
	var failure = false;
	for(var i = 0;i<100 && !failure;++i){
		var v1 = r1.equilikely(10,20);
		var v2 = r2.equilikely(10,20);
		var v3 = r3.equilikely(10,20);
		a1+=v1;
		a2+=v3;
		failure |= (v1!=v2) || v1<0 || v1>20;
	}
	test( "MathLib:", !failure && a1!=a2 );
}
