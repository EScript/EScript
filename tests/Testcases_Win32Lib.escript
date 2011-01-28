// libwin32-testcases

// this destroys the current clipboard data, so it is skipped by default
if(false){
	Win32.setClipboard("EScriptTest");
	Win32.setClipboard(Win32.getClipboard()+"#1");
	Win32.setClipboard(Win32.getClipboard()+"#2");


	test("Win32 Clipboard:", Win32.getClipboard()=="EScriptTest#1#2" );
}

return errors;
