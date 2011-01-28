
out("\n Clipboard: ",Win32.getClipboard());
var s="foo";
out("\n Setting Clipboard to ",s,"\n");
Win32.setClipboard(s);
out("\n Clipboard: ",Win32.getClipboard(),"\n");
