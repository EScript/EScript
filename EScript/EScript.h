/*
**  Copyright (c) Claudius Jaehn (claudiusj@users.berlios.de), 2007-2013
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
#ifndef ESCRIPT_H
#define ESCRIPT_H

#include "Version.h"

#include "Basics.h"
#include "StdObjects.h"

#ifdef ES_DEBUG_MEMORY
#include "Utils/Debug.h"
#endif

#if defined(_MSC_VER)
#pragma warning( disable : 4290 )
#endif

/*! \mainpage A short introduction into EScript
 *
 * \htmlinclude docs/Introduction.html
 *
 */
namespace EScript {

//! @name Initialization
//@{

typedef void ( libInitFunction)(Namespace *);

/*! Init the EScript system and all registered libraries.
	Has to be called once before any script can be executed */
void init();

/*! Calls the the given init(...) function with the SGLOBALS-Object (static global namespace).
	\note Can be used manually as an alternative to the automatic registerLibraryForInitialization-way. */
void initLibrary(libInitFunction * initFunction);

//! (internal) Get the static super global namespace.
Namespace * getSGlobals();
//@}

}

#endif // ESCRIPT_H
