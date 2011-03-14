
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
	Runtime._setErrorConfig(Runtime.TREAT_WARNINGS_AS_ERRORS);
	var exceptionMessage;;
	try{
		Runtime.warn("foo");
	}catch(e){
		exceptionMessage = e.getMessage();
	}
	Runtime._setErrorConfig(0);
	test( "Runtime.warn",exceptionMessage=="foo");

}

{
	test( "Runtime._stackSize",
			(fn(){return Runtime._getStackSize();})() == (fn(){ return (fn(){return Runtime._getStackSize();})();})()-1 );
}
