/*
**  Copyright (c) 2007-2011 Claudius Jaehn
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

#define ES_VERSION "EScript 0.5.6 Stoepselhase - stable (http://fetter-hase.de)"

#include "Objects/Object.h"
#include "Objects/Type.h"
#include "Objects/ExtObject.h"
#include "Objects/Delegate.h"
#include "Objects/Function.h"
#include "Objects/Number.h"
#include "Objects/NumberRef.h"
#include "Objects/Bool.h"
#include "Objects/Void.h"
#include "Objects/String.h"
#include "Objects/Array.h"
#include "Objects/Map.h"
#include "Objects/Exception.h"
#include "Objects/ReferenceObject.h"
#include "Objects/Namespace.h"
#include "Runtime/Runtime.h"
#include "Utils/Hashing.h"
#include "Utils/Macros.h"
#include "Consts.h"

#include <vector>

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
class Block;

// -----------------------------------------------

/*! @name Initialization */
//@{

typedef void ( libInitFunction)(Namespace *);

/*! Init the EScript system and all registered libraries.
	Has to be called once before any script can be executed */
void init();

/*! Calls the the given init(...) function with the SGLOBALS-Object (static global namespace).
	\note Can be used manually as an alternative to the automatic registerLibraryForInitialization-way. */
void initLibrary(libInitFunction * initFunction);

/*! (internal) Get the static super global namespace.*/
Namespace * getSGlobals();
//@}

// -----------------------------------------------

/*! @name Declaration */
//@{

/*! Add a type Function attribute to @p type with given name. */
void declareFunction( Type * type,identifierId nameId, Function::functionPtr fn);
void declareFunction( Type * type,const char * name, Function::functionPtr fn);
void declareFunction( Type * type,identifierId nameId,int minParamCount,int maxParamCount, Function::functionPtr fn);
void declareFunction( Type * type,const char * name,int minParamCount,int maxParamCount, Function::functionPtr fn);
void declareConstant( Type * type,identifierId nameId, Object * value);
void declareConstant( Type * type,const char * name, Object * value);

void declareFunction( Namespace * type,identifierId nameId, Function::functionPtr fn);
void declareFunction( Namespace * type,const char * name, Function::functionPtr fn);
void declareFunction( Namespace * type,identifierId nameId,int minParamCount,int maxParamCount, Function::functionPtr fn);
void declareFunction( Namespace * type,const char *name,int minParamCount,int maxParamCount, Function::functionPtr fn);
void declareConstant( Namespace * type,identifierId nameId, Object * value);
void declareConstant( Namespace * type,const char * name, Object * value);
//@}

// -----------------------------------------------

/*! @name Runtime helper */
//@{
/*! (internal) Non-inline part of assertParamCount(...) */
void assertParamCount_2(Runtime & runtime,const ParameterValues & params,int min,int max);

/*! Checks if the number given parameters is in the given range (min <= number <= max).
	A range value of <0 accepts arbitrary number of parameters.
	If too many parameters are given, a warning message is shown. If too few parameter
	are given, a runtime error is thrown.   */
inline void assertParamCount(Runtime & runtime,const ParameterValues & params,int min=-1,int max=-1){
	if( (static_cast<int>(params.count())<min && min>=0) || ((static_cast<int>(params.count())>max) && max>=0))
		assertParamCount_2(runtime,params,min,max);
}

/*! (internal) Non-inline part of assertType(...)   */
void assertType_throwError(Runtime & runtime,const ObjPtr & obj);

/*! Tries to cast the given object to the specified type. If the object is not of the
	appropriate type, a runtime error is thrown. */
template<class T> static T * assertType(Runtime & runtime,const ObjPtr & obj) {
	T* t=dynamic_cast<T*>(obj.get());
	if (t==NULL)
		assertType_throwError(runtime,obj);
	return t;
}

Object * callMemberFunction(Runtime & rt,ObjPtr obj,identifierId fnNameId,const ParameterValues & params);
Object * callMemberFunction(Runtime & rt,ObjPtr obj,const std::string & fnName,const ParameterValues & params);
Object * callFunction(Runtime & rt,Object * function,const ParameterValues & params);

void out(Object * obj);

Block * loadScriptFile(const std::string & filename,ERef<Block> block=NULL)throw(Exception *);

//! @return (success,result)
std::pair<bool,ObjRef> loadAndExecute(Runtime & runtime,const std::string & filename);

//@}

}

#endif // ESCRIPT_H
