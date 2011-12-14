var OK="\t ok\n";
var FAILED="\t failed\n";

{
	out("Win32 Clipboard:" );

	Win32.setClipboard(Win32.getClipboard()+"#1");
	Win32.setClipboard(Win32.getClipboard()+"#2");


	if( Win32.getClipboard().endsWith("#1#2") )
		out(OK); else { errors+=1; out(FAILED); }
}

return errors;
