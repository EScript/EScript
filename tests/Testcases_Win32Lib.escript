// Testcases_Win32Lib.escript
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
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
