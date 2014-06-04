/*
**  Copyright (c) Claudius Jähn (claudiusj@users.berlios.de), 2007-2014
**
**  Permission is hereby granted, free of charge, to any person obtaining a copy of this
**  software and associated documentation files (the "Software"), to deal in the Software
**  without restriction, including without limitation the rights to use, copy, modify,
**  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
**  permit persons to whom the Software is furnished to do so, subject to the following
**  conditions:
**
**  The above copyright notice and this permission notice shall be included in all copies
**  or substantial portions of the Software.
**
**  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
**  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
**  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
**  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
**  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
**  OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*! Module loading. Required for loading any of the modules of the Std-library! 
	\note May be safely loaded multiple times using "load(...)"

	\code
		// init module loader:

		static module = load("./Std/module.escript"); // Path to EScript/Std/module.escript
		module.addSearchPath("."); // Path EScript/Std folder.


		module("Std/MultiProcedure");				// Load or get the given module with absolute module path
		module("./OtherModule");					// Load or get the given module with relative module path
		module.require("./OtherModule");			// Same as module("./OtherModule");
		module.on("Std/MultiProcedure",callback);	// Call callback(module) if or when the given module is loaded.
		module.get("SomeModule");					// Return the given module if loaded or return void.
	\endcode

*/
static FILE_ID = __DIR__+"/"+__FILE__;
if(!GLOBALS.isSet($__stdLibCache))
	GLOBALS.__stdLibCache := new Map;
var module = GLOBALS.__stdLibCache[FILE_ID];
if(!module)
	GLOBALS.__stdLibCache[FILE_ID] = module = load(__DIR__+"/_module.escript");
////////////////////////////	module = GLOBALS.__stdLibCache[FILE_ID] = load(__DIR__+"/_module.escript");

return module;

// ------------------------------------------
