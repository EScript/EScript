#ifndef ESCRIPT_H
#define ESCRIPT_H

#define ES_VERSION "EScript 0.5 Stoepselhase - alpha (http://fetter-hase.de)"

#include "Object.h"
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
#include "Objects/Script.h"
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

/*! Add a type Function attribute to @p type with given name. */
void declareFunction( Type * type,identifierId nameId, Function::functionPtr fn);
void declareFunction( Type * type,const char *name, Function::functionPtr fn);
void declareConstant( Type * type,identifierId nameId, Object * value);
void declareConstant( Type * type,const char *name, Object * value);

void declareFunction( Namespace * type,identifierId nameId, Function::functionPtr fn);
void declareFunction( Namespace * type,const char *name, Function::functionPtr fn);
void declareConstant( Namespace * type,identifierId nameId, Object * value);
void declareConstant( Namespace * type,const char *name, Object * value);
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
Block * loadScriptFile(const std::string & filename,Block * b=NULL);

//@}

}

#endif // ESCRIPT_H
