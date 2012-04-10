
{
	var exceptionMessage;
	var stackInfo;
	try{
		Runtime.exception("foo");
	}catch(e){
		exceptionMessage = e.getMessage();
		stackInfo = e.getStackInfo();
	}
	test( "Runtime.exception", exceptionMessage=="foo" && stackInfo.find(__FILE__) );
}

{
	Runtime.setTreatWarningsAsError(true);
	var exceptionMessage;;
	try{
		Runtime.warn("foo");
	}catch(e){
		exceptionMessage = e.getMessage();
	}
	Runtime.setTreatWarningsAsError(false);
	test( "Runtime.warn",exceptionMessage.contains("foo"));

}

{
	test( "Runtime._stackSize",
			(fn(){return Runtime._getStackSize();})() == (fn(){ return (fn(){return Runtime._getStackSize();})();})()-1 );
}
//Runtime.enableLogCounting();

//out("-",Runtime.getLogCounter(Runtime.LOG_ERROR),"\n");
